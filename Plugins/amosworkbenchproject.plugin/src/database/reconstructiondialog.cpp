#include "reconstructiondialog.h"
#include "ui_reconstructiondialog.h"
#include "chooseinputimagesdialog.h"
#include "imageslistdialog.h"
#include "qvariantptr.h"
#include "utils.hpp"
#include "rightclickmenu.h"

#include "utils.hpp"

#include <qewdialogfactory.h>
#include <qewsimpledialog.h>
#include <QMessageBox>

ReconstructionDialog::ReconstructionDialog(QWidget *parent) :
    QewTabDialog(parent),
    ui(new Ui::ReconstructionDialog)
{
    ui->setupUi(this);

    ui->parametersTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->subjectsTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->subjectsTableWidget->setColumnCount(ui->subjectsTableWidget->columnCount() + 1);
    ui->subjectsTableWidget->setColumnHidden(ui->subjectsTableWidget->columnCount() - 1, true);

    connect(ui->idLineEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect(ui->descriptionTextEdit, SIGNAL(textChanged()), this, SLOT(onDescriptionModified()));

    connect(ui->programPathLineEdit, SIGNAL(editingFinished()), this, SLOT(setProgramPath()));
    connect(ui->programPathToolButton, SIGNAL(clicked(bool)), this, SLOT(chooseProgramPath()));
    
    connect(ui->enableEditionCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableParametersEdition(bool)));

    stateButtons << ui->unsetRadioButton << ui->setRadioButton << ui->runningRadioButton
                 << ui->partiallyFinishedRadioButton << ui->finishedRadioButton;
    for(int i = 0; i < stateButtons.size(); i++)
        ui->executionStatusButtonGroup->setId(stateButtons.at(i), i);
    connect(ui->executionStatusButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(stateButtonClicked(int)));

    connect(ui->addParameterToolButton, SIGNAL(clicked(bool)), this,
            SLOT(addParameter()));
    connect(ui->removeParameterToolButton, SIGNAL(clicked(bool)), this,
            SLOT(removeParameters()));
    
    connect(ui->runPushButton, SIGNAL(clicked()), this, SLOT(performProcess()));

    connect(ui->subjectsTableWidget, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(onSubjectItemChecked(QTableWidgetItem*)));
/*    connect(ui->subjectsTableWidget, SIGNAL(cellDoubleClicked(int, int)),
            this, SLOT(subjectForReprocessing(int, int, bool))); */   
    connect(ui->selectSubjectToolButton, SIGNAL(clicked(bool)), this,
            SLOT(selectSubjects()));
    connect(ui->unselectSubjectToolButton, SIGNAL(clicked(bool)), this,
            SLOT(unselectSubjects()));
    connect(ui->reprocessSubjectToolButton, SIGNAL(clicked(bool)), this,
            SLOT(reprocessSubjects()));    
    connect(ui->loadSubjectsComboBox, SIGNAL(activated(int)), this,
            SLOT(loadSubjectsFileFromList(int)));
    connect(ui->saveSubjectsPushButton, SIGNAL(clicked(bool)), this,
            SLOT(saveSubjectsFile()));

    connect(ui->parametersTableWidget, SIGNAL(cellChanged(int, int)),
            this, SLOT(parameterChanged(int, int)));
    connect(ui->parametersTableWidget, SIGNAL(cellClicked(int,int)),
            this, SLOT(parameterToBeChanged(int, int)));

    ui->parametersTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->parametersTableWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showActionsMenu(const QPoint &)));
    
    connect(ui->threadsSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(setNumberOfThreads(int)));
    connect(ui->subjectsSpinBox, SIGNAL(valueChanged(int)), this,
            SLOT(setNumberOfSubjects(int)));    
        
    displayTime("00:00:00:000");

    selectableImages = 0;
    reconstruction = 0;
    execState = Reconstruction::UNSET;
    subjectsFilename = QDir::homePath();
    setAcceptMeansClose(false);
    reconstructionSet = false;
    programDataSet = false;
    paramRow = -1;
    inputDialog = 0;
    outputDialog = 0;
    prevRunEnabled = false;
    maximumThreads = ui->threadsSpinBox->value();
    maximumSubjects = ui->subjectsSpinBox->value();
}

ReconstructionDialog::~ReconstructionDialog()
{
    delete ui;
    outputImages.clear();
    if(selectableImages)
        selectableImages->updateList(this, reconstruction->getReconstructionUuid().toString(),
                                     outputImages);
    qDebug("Deleting ReconstructionDialog");
}

bool ReconstructionDialog::setUpDialog()
{
    bool ret = QewTabDialog::setUpDialog();
    QewDialogFactory factory;

    bool connectDialog = true;
    bool withButtons = false;
    const char *name = "ImagesDialog";
    imagesDialog = factory.createDialog(QewSimpleWidget, this, connectDialog, withButtons, name);
    
    bool forInputList = true;
    inputDialog = new ImagesListDialog(imagesDialog, forInputList);

    connect(inputDialog, SIGNAL(imageNameChanged(QString, QString)),
            this, SLOT(imageNameChangedInList(QString, QString)));
    connect(inputDialog, SIGNAL(imageRemoved(QString, QString)),
            this, SLOT(imageRemovedFromList(QString, QString)));

    inputDialog->setWindowTitle(tr("Input images"));
    if(!(ret = inputDialog->setUpDialog())) {

    }

    forInputList = false;
    outputDialog = new ImagesListDialog(imagesDialog, forInputList);
    outputDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
    outputDialog->setAnyFile(true);

    connect(outputDialog, SIGNAL(imageNameChanged(QString, QString)),
            this, SLOT(imageNameChangedInList(QString, QString)));
    connect(outputDialog, SIGNAL(imageRemoved(QString, QString)),
            this, SLOT(imageRemovedFromList(QString, QString)));

    outputDialog->setImageDialogTitle(tr("Choose output images"));
    outputDialog->setWindowTitle(tr("Output images"));
    outputDialog->setTypesToRemove(typesToRemove);

    if(!(ret = outputDialog->setUpDialog())) {

    }

    imagesDialog->addExtensibleChild(inputDialog, "Input images");
    imagesDialog->addExtensibleChild(outputDialog, "Output images");
    QLayout* layout = imagesDialog->layout();
    if(layout->inherits("QBoxLayout")) {
        QBoxLayout* boxLayout = (QBoxLayout*)layout;
        boxLayout->addWidget(inputDialog, 6);
        boxLayout->addWidget(outputDialog, 7);
    }
    addExtensibleChild(imagesDialog, tr("Images"), 0, 1);

    return ret;
}

QString ReconstructionDialog::saveXML()
{
    QDomDocument doc(XMLDocName());
    doc.appendChild( doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"") );
    QDomElement root = doc.createElement("Subjects");
    writeToXml(doc, root);
    return doc.toString(4);    
}

void ReconstructionDialog::writeToXml(QDomDocument &doc, QDomElement &e)
{
    // TODO mirar esto
    XMLOperator* xml_operator = XMLOperator::instance();
    doc.appendChild(e);
    QMapIterator<QString, bool> its(subjects);
    while(its.hasNext()) {
        its.next();
        QDomElement tag_subject = doc.createElement("subject");
        e.appendChild(tag_subject);

        tag_subject.appendChild(xml_operator->createTextElement(doc,
                      "id", its.key()));
        tag_subject.appendChild(xml_operator->createTextElement(doc,
                      "selected", its.value() ? "1" : "0"));
    }    
}

void ReconstructionDialog::readFromXML(QDomElement &e)
{
    // TODO mirar esto
    XMLOperator* xml_operator = XMLOperator::instance();
    subjects.clear();

    QDomNodeList n_subjects = e.childNodes();
    int cnt = n_subjects.count();
    QDomElement e_sub;
    for(int i = 0; i < cnt; ++i) {
        QDomNode n = n_subjects.item(i);
        e_sub = n.toElement();
        if(!e_sub.isNull()) {
            QDomElement e_id = xml_operator->findTag("id", e_sub);
            QDomElement e_select = xml_operator->findTag("selected", e_sub);
            if(!e_id.isNull() && !e_select.isNull())
                subjects.insert(e_id.text(), e_select.text() == "1" ? true : false);
        }
    }
}

void ReconstructionDialog::readProgramData()
{
//    if(parametersData.count() > 0)
//        return;
    XMLOperator* xml_operator = XMLOperator::instance();
//     QString filename = QDir::homePath() + "/mitk/qtprojects/amos-projects/processdata.apd";
    QString filename = ":/database/processdata.apd";
    QString docName = "Process_data";
    QString sender = tr("Open process data file");
    QDomDocument doc = xml_operator->loadXML(filename, docName, sender);

    QDomNode n = doc.firstChild();
    if(n.isNull())
        return;
    QDomElement e_root = n.toElement();
    if(e_root.isNull())
        return;
    if(e_root.tagName() != "Processes_list")
        return;

    QDomNodeList n_processes = e_root.childNodes();

    QDomElement e_process = xml_operator->findParentTag("program_id", e_root, reconstruction->getProgramId());
    ui->reconsProgramIDLabel->setText(reconstruction->getProgramId());
    
    if(reconstruction->getProgramId() == "AmosSegmentation") {
        setMaxSubjects(maximumThreads);
    }    
    else if(reconstruction->getProgramId() == "AmosResampling") {
        enableMaxSubjects(false);
        setMaxSubjects(1);
    }
    else if(reconstruction->getProgramId() == "FreeSurferLabelling") {
        size_t freeRam = Utils::getTotalSizeOfFreeRam();
        cout << " Free Ram " << freeRam << " bytes" << endl;
        QList<quint64> memoryUsage;
        memoryUsage << 2471878656 << 4707016704 << 5449793536 << 6774882304 << 8099135488 << 9426309120 << 10689093632 << 11943464960 << 12789882880 << 14047309824 << 14789459968 << 15290544128 << 15529488384 << 16542081024 << 17032024064 << 17358585856 << 18295316480 << 19622510592 << 20947599360 << 22272688128 << 23596941312 << 24842964992 << 25332908032 << 253974896646;
        int i = 0;
        while(true) {
            if((double)freeRam * 1.1 < memoryUsage.at(i))
                break;
            i++;
            
        }
        i = i == 0 ? 1 : i;
        setMaxSubjects(i);
    }    

    if(!e_process.isNull()) {
        QDomElement e_procname = xml_operator->findTag("program_id", e_process);        
        if(!e_procname.isNull()) { 
            showPrefixParamColumn(reconstruction->getHasPrefixes());
            QDomElement e_hasprefixes = xml_operator->findTag("has_prefixes", e_process);
            
            QDomElement e_params = xml_operator->findTag("parameters_list", e_process);
            QDomNodeList n_params = e_params.childNodes();
            int cnt = n_params.count();
            QDomElement e_param;

            for(int i = 0; i < cnt; ++i) {
                QDomNode n = n_params.item(i);
                e_param = n.toElement();
                if(!e_param.isNull()) {
                    ParameterData paramData;
                    QDomElement e_name = xml_operator->findTag("name", e_param);
                    paramData.name = e_name.text();
                    QDomElement e_prefix = xml_operator->findTag("prefix", e_param);
                    paramData.prefix = e_prefix.text();
                    QDomElement e_type = xml_operator->findTag("type", e_param);
                    paramData.type = e_type.text();
                    QDomElement e_default = xml_operator->findTag("defaultvalue", e_param);
                    paramData.defaultValue = e_default.text();
                    QDomElement e_cond1 = xml_operator->findTag("condition1", e_param);
                    paramData.condition1 = e_cond1.text();
                    QDomElement e_cond2 = xml_operator->findTag("condition2", e_param);
                    paramData.condition2 = e_cond2.text();
                    QDomElement e_tooltip = xml_operator->findTag("tooltip", e_param);
                    paramData.toolTip = e_tooltip.text();

                    parametersData.append(paramData);
                    if(!parameters.contains(paramData.name)) {
                        Reconstruction::Parameter param;
                        param.name = paramData.name;
                        param.prefix = paramData.prefix;
                        param.value = paramData.defaultValue;
                        parameters[paramData.name] = param;
//                         parameters.replace(paramData.name, param);
                        reconstruction->getParameters().insert(paramData.name, param);
                    }
                }
            }
        }
    }

}

void ReconstructionDialog::setProgramData()
{
    if(programDataSet)
        return;
    
    ui->runPushButton->setEnabled(true && (execState == Reconstruction::SET || execState == Reconstruction::PARTIAL_FINISH));
    prevRunEnabled = ui->runPushButton->isEnabled();     

    disconnect(ui->parametersTableWidget, SIGNAL(cellChanged(int, int)),
            this, SLOT(parameterChanged(int, int)));       

    int cnt = parametersData.size();
    for(int i = 0; i < cnt; ++i) {
        ParameterData paramData = parametersData.at(i);

        int row = ui->parametersTableWidget->rowCount();
        ui->parametersTableWidget->insertRow(row);

        QTableWidgetItem* itemName = new QTableWidgetItem(paramData.name);
        ui->parametersTableWidget->setItem(row, 0, itemName);

        QTableWidgetItem* itemPrefix = new QTableWidgetItem(paramData.prefix);
        ui->parametersTableWidget->setItem(row, 1, itemPrefix);

        QTableWidgetItem* itemValue = new QTableWidgetItem();
        ui->parametersTableWidget->setItem(row, 2, itemValue);
        setCellWidgetValue(paramData, itemValue);
    }
    
    programDataSet = true;
    enableParametersEdition(false);
    
    connect(ui->parametersTableWidget, SIGNAL(cellChanged(int, int)),
            this, SLOT(parameterChanged(int, int)));
}

void ReconstructionDialog::setCellWidgetValue(ParameterData paramData, QTableWidgetItem *itemValue)
{
    QWidget* widget = 0;
//     Reconstruction::Parameter parameter = parameters.values(paramData.name).at(0);
        Reconstruction::Parameter parameter = parameters.value(paramData.name);
    if(paramData.type == "bool") {
        QCheckBox* checkBox = new QCheckBox;
        checkBox->setObjectName(QString::number(itemValue->row()));
    //        bool check = paramData.defaultValue == "true" ? true : false;
        bool check = parameter.value == "true" ? true : false;
        checkBox->setChecked(check);
        connect(checkBox, SIGNAL(stateChanged(int )), this, SLOT(boolParamChanged()));
        widget = checkBox;
    }
    else if(paramData.type == "float") {
        QDoubleSpinBox* spinBox = new QDoubleSpinBox;
        spinBox->setObjectName(QString::number(itemValue->row()));
//         float val = paramData.defaultValue.toDouble();
        float val = parameter.value.toDouble();
        float min = paramData.condition1.toDouble();
        bool ok;
        float max = paramData.condition2.toDouble(&ok);
        if(!ok)
            max = 99.99;        
        spinBox->setMinimum(min);
        spinBox->setMaximum(max);
        spinBox->setDecimals(4);
        spinBox->setSingleStep(0.1);
        spinBox->setValue(val);
        connect(spinBox, SIGNAL(valueChanged(QString)), this, SLOT(doubleParamChanged()));
        widget = spinBox;
    }
    else if(paramData.type == "int" || paramData.type == "time") {
        QSpinBox* spinBox = new QSpinBox;
        spinBox->setObjectName(QString::number(itemValue->row()));
//         int val = paramData.defaultValue.toInt();
        int val = parameter.value.toInt();
        int min = 0;
        bool ok = true;
        if(paramData.type == "int") {
            min = paramData.condition1.toInt(&ok);
            if(!ok)
                min = 0;
        }
        else if(paramData.type == "time") {
            if(parameter.prefix.isEmpty()) {
                parameter.prefix = paramData.condition1;
                 parameters[paramData.name].prefix = parameter.prefix;
//                 parameters.replace(parameters.value(paramData.name).prefix, parameter.prefix);
                saveThis();
                QString title(tr("Parameter updated"));
                QString text(tr("Parameter ") + paramData.name + tr(" of type time has been updated"));
                QMessageBox::information(0, title, text);                
                QTimer::singleShot(100, this, SLOT(delayedEmitDialogSaved()));
            }            
        }        
        int max = paramData.condition2.toInt(&ok);
        if(!ok)
            max = 100;        
        spinBox->setMinimum(min);
        spinBox->setMaximum(max);
        spinBox->setSingleStep(1);
        spinBox->setValue(val);
        connect(spinBox, SIGNAL(valueChanged(QString)), this, SLOT(intParamChanged()));
        widget = spinBox;
    }
    else if(paramData.type == "enum") {
        QComboBox* comboBox = new QComboBox;
        comboBox->setObjectName(QString::number(itemValue->row()));
        int val = parameter.value.toInt();
        QString items = paramData.condition1;
        QStringList itemsList = items.split(",");
        comboBox->addItems(itemsList);        
        comboBox->setCurrentIndex(val);
        connect(comboBox, SIGNAL(currentIndexChanged(int )), this, SLOT(enumParamChanged()));
        widget = comboBox;          
    }     

    if(widget) {
        ui->parametersTableWidget->setCellWidget(itemValue->row(), itemValue->column(),
                                                widget);
        if(!paramData.toolTip.isEmpty())
            widget->setToolTip(paramData.toolTip);
    }
    else
        itemValue->setText(paramData.defaultValue);
}

void ReconstructionDialog::setCellWidgetValue(QWidget *widget, QString value)
{
    if(widget->inherits("QCheckBox")) {
        QCheckBox* checkBox = (QCheckBox*) widget;
        bool check = value == "true" ? true : false;
        checkBox->setChecked(check);
    }
    else if(widget->inherits("QDoubleSpinBox")) {
        QDoubleSpinBox* dSpinBox = (QDoubleSpinBox*) widget;
        float val = value.toDouble();
        dSpinBox->setValue(val);
    }
    else if(widget->inherits("QSpinBox")) {
        QSpinBox* spinBox = (QSpinBox*) widget;
        int val = value.toInt();
        spinBox->setValue(val);
    }    
}

void ReconstructionDialog::setSelectableImages(ReconstructionInputList *selectableImages)
{
    this->selectableImages = selectableImages;
    if(selectableImages) {
        for(int i = 0; i < outputImages.size(); i++) {
            selectableImages->addInputInfo(outputImages.at(i), this,
                                    reconstruction->getReconstructionUuid().toString());
        }
    }
    inputDialog->setSelectableImages(selectableImages);
}

void ReconstructionDialog::removeAllChildren()
{
    for(int i = 0; i < extensibleChildren().size(); i++) {
        removeExtensibleChild(extensibleChildren().at(0));
    }
}

void ReconstructionDialog::changeEvent(QEvent *e)
{
    QewExtensibleDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ReconstructionDialog::fillSubjectsTable()
{
    for(int i = ui->subjectsTableWidget->rowCount() - 1; i >= 0 ; i--)
        ui->subjectsTableWidget->removeRow(i);

    disconnect(ui->subjectsTableWidget, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(onSubjectItemChecked(QTableWidgetItem*)));

    int i = 0;
    processedSubjects = reconstruction->getProcessedSubjects();
    QMapIterator<QString, bool> its(subjects);
    while(its.hasNext()) {
        its.next();
        if(processedSubjects.contains(its.key()))
            fillSubjectRow(i, its.value(), its.key(), true);
        else
            fillSubjectRow(i, its.value(), its.key(), false);
        i++;
    }

    ui->subjectsTableWidget->sortByColumn(1, Qt::AscendingOrder);
    // first position for selected items
    ui->subjectsTableWidget->sortByColumn(2, Qt::DescendingOrder);

    connect(ui->subjectsTableWidget, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(onSubjectItemChecked(QTableWidgetItem*)));

}

void ReconstructionDialog::fillSubjectRow(int row, bool selected, QString id, bool processed)
{
    ui->subjectsTableWidget->insertRow(row);
    QTableWidgetItem* itemCheck = new QTableWidgetItem();
    QTableWidgetItem* itemId = new QTableWidgetItem(id);
    QString checkText = selected ? "1" : "0";
    QTableWidgetItem* itemCheckText = new QTableWidgetItem(checkText);
    itemCheck->setFlags(itemCheck->flags() | Qt::ItemIsUserCheckable);
    itemCheck->setCheckState(selected ? Qt::Checked : Qt::Unchecked);
    itemId->setFlags(itemId->flags() & ~Qt::ItemIsEditable);

    ui->subjectsTableWidget->setItem(row, 0, itemCheck);
    ui->subjectsTableWidget->setItem(row, 1, itemId);
    ui->subjectsTableWidget->setItem(row, 2, itemCheckText);
    if(!processed) {        
        itemCheck->setFlags(itemCheck->flags() | Qt::ItemIsSelectable);
        itemId->setFlags(itemId->flags() ^ Qt::ItemIsSelectable);        
    }
    else {
        if(itemCheck->checkState() == Qt::Checked) // if user choose Restore reconstruction and item is ckecked
            itemCheck->setCheckState(Qt::Unchecked); // unchecked it
        itemCheck->setFlags(itemCheck->flags() ^ Qt::ItemIsSelectable);
        itemId->setFlags(itemId->flags() | Qt::ItemIsSelectable);
        itemId->setBackground(QBrush(QColor(255, 0, 0, 128)));
    }
        
}

void ReconstructionDialog::disableProcessedSubjects(QStringList fullyProcessedSubjects)
{
    for(int i = 0; i < ui->subjectsTableWidget->rowCount(); i++) {
        QTableWidgetItem* itemCheck = ui->subjectsTableWidget->item(i, 0);
        QTableWidgetItem* itemId = ui->subjectsTableWidget->item(i, 1);
        if(fullyProcessedSubjects.contains(itemId->text())) {
            itemCheck->setCheckState(Qt::Unchecked);
            itemCheck->setFlags(itemCheck->flags() ^ Qt::ItemIsSelectable);
            itemId->setFlags(itemId->flags() | Qt::ItemIsSelectable); 
            itemId->setBackground(QBrush(QColor(255, 0, 0, 128)));
            selectedSubjects.removeOne(itemId->text());
        }
    }
    processedSubjects.append(fullyProcessedSubjects);
    setDataChangedThis(true);
}

void ReconstructionDialog::reprocessSubjects()
{
    QList<QTableWidgetItem*> items = ui->subjectsTableWidget->selectedItems();
    QList<int> rows = Utils::getRowsForItems(items);
    int cnt = rows.size();
    
    if(cnt == 0)
        return;
    bool plural = (cnt > 1);
    if(!reprocessMessage(plural))
        return;    
    
//     bool show_message = false;
    for(int i = 0; i < cnt; i++) {
        subjectForReprocessing(rows.at(i), 1);   
    }
}

bool ReconstructionDialog::reprocessMessage(bool plural)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Reprocess subject"));
    if(plural)
        msgBox.setText(tr("Do you want to reprocess these subjects?"));
    else
        msgBox.setText(tr("Do you want to reprocess this subject?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int ret = msgBox.exec();

    return ret == QMessageBox::Ok;
}

void ReconstructionDialog::subjectForReprocessing(int row, int col)
{
    if(col != 1)
        return;
    QTableWidgetItem* itemId = ui->subjectsTableWidget->item(row, 1);
	QTableWidgetItem* itemCheck = ui->subjectsTableWidget->item(row, 0);
// 	if (itemCheck->flags() & Qt::ItemIsSelectable)
//         return;
    
    itemCheck->setFlags(itemCheck->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    itemId->setFlags(itemId->flags() | Qt::ItemIsSelectable);
    itemId->setBackground(QBrush(QColor(Qt::white)));
    itemId->setSelected(false);
    processedSubjects.removeOne(itemId->text());
    if(processedSubjects.size() == 0) {
        execState = Reconstruction::SET;
        ui->unsetRadioButton->setEnabled(true);
        ui->setRadioButton->setEnabled(true);
    }
    else {
        execState = Reconstruction::PARTIAL_FINISH;
        ui->unsetRadioButton->setEnabled(false);
        ui->setRadioButton->setEnabled(false);        
    }
    stateButtons.at(execState)->setChecked(true);
    setDataChanged(true);
    
}

void ReconstructionDialog::fillParametersTable()
{
//    if(parameters.size() == 0)
//        return;
    disconnect(ui->parametersTableWidget, SIGNAL(cellChanged(int, int)),
            this, SLOT(parameterChanged(int, int)));
    parameters.clear();
    if(reconstruction->getProgramId() == tr("Custom")) {
        for(int i = ui->parametersTableWidget->rowCount() - 1; i >= 0; i--)
            ui->parametersTableWidget->removeRow(i);
    }
    QMapIterator<QString, Reconstruction::Parameter> itp(reconstruction->getParameters());
    while(itp.hasNext()) {
        itp.next();
        addParameter(itp.key(), itp.value());
    }
    connect(ui->parametersTableWidget, SIGNAL(cellChanged(int, int)),
            this, SLOT(parameterChanged(int, int)));
}

void ReconstructionDialog::onDescriptionModified()
{
    setDataChangedThis(true);
}

void ReconstructionDialog::chooseProgramPath()
{  
    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename = QDir::homePath();
    if(!ui->programPathLineEdit->text().trimmed().isEmpty())
        filename = ui->programPathLineEdit->text().trimmed();
    QString sender = tr("Program path");
    QString type_file = "";
    QString extensions = "";
    QFileDialog::FileMode filemode = QFileDialog::Directory;

    bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
                                           filemode, type_file, extensions, QFileDialog::ShowDirsOnly);    
//     bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
//                               QFileDialog::ExistingFile, type_file, extensions);
    if(!ret)
        return;
    if(programPath != filename.trimmed()) {
        setDataChangedThis(true);
    }
    ui->programPathLineEdit->setText(filename.trimmed());
}

void ReconstructionDialog::setProgramPath()
{
    if(programPath != ui->programPathLineEdit->text().trimmed()) {
        setDataChangedThis(true);
    }
}

void ReconstructionDialog::enableParametersEdition(bool enable)
{
    ui->parametersTableWidget->setEnabled(enable);
    if(reconstruction->getProgramId() == tr("Custom")) {
        ui->addParameterToolButton->setEnabled(enable);
        ui->removeParameterToolButton->setEnabled(enable);
    }
    conditionalEnableRun(enable);
}

void ReconstructionDialog::conditionalEnableRun(bool enable)
{
    if(enable) {
        prevRunEnabled = ui->runPushButton->isEnabled();
        ui->runPushButton->setEnabled(false);
    }
    else
        ui->runPushButton->setEnabled(!getDataChanged() && prevRunEnabled);    
}

void ReconstructionDialog::addParameter()
{
    QString num = QString::number(parameters.size() + 1);
    Reconstruction::Parameter param;
    param.name = tr("name") + num;
    param.prefix = "";
    param.value = tr("value") + num;
    addParameter(param.name, param);
    setDataChangedThis(true);
}

void ReconstructionDialog::addParameter(const QString& name, const Reconstruction::Parameter &parameter)
{
    if(name.trimmed().isEmpty()) {
        QString title(tr("Parameter name"));
        QString text(tr("Empty parameter name. Please set a name."));
        QMessageBox::warning(0, title, text);
        return;
    }

    int cnt = ui->parametersTableWidget->rowCount();
    int i = 0;
    while(i < cnt) {
        if(name == ui->parametersTableWidget->item(i, 0)->text())
            break;
         i++;
    }
    if(i < cnt) {
        if(ui->parametersTableWidget->cellWidget(i, 2) == 0)
            ui->parametersTableWidget->item(i, 2)->setText(parameter.value);
        else
            setCellWidgetValue(ui->parametersTableWidget->cellWidget(i, 2), parameter.value);
    }
    else {
        int row = ui->parametersTableWidget->rowCount();
        ui->parametersTableWidget->insertRow(row);
        QTableWidgetItem* itemName = new QTableWidgetItem(name);
        QTableWidgetItem* itemPrefix = new QTableWidgetItem(parameter.prefix);
        QTableWidgetItem* itemValue = new QTableWidgetItem(parameter.value);
        ui->parametersTableWidget->setItem(row, 0, itemName);
        ui->parametersTableWidget->setItem(row, 1, itemPrefix);
        ui->parametersTableWidget->setItem(row, 2, itemValue);
    }
    parameters.insert(name, parameter);

}

QString ReconstructionDialog::getTitle() const
{
    QString title = tr("Reconstruction output image");
    return title;
}

void ReconstructionDialog::setTypesToRemove(QList<AmosImage::ImageType> types)
{
    typesToRemove = types;
}

void ReconstructionDialog::removeParameters()
{
    QList<QTableWidgetItem*> items = ui->parametersTableWidget->selectedItems();
    QList<int> rows = Utils::getRowsForItems(items);

    int cnt = rows.size();
    for(int i = cnt - 1; i >= 0; i--) {
        parameters.remove(ui->parametersTableWidget->item(rows.at(i), 0)->text());
        ui->parametersTableWidget->removeRow(rows.at(i));
    }
    setDataChangedThis(true);
}

Reconstruction *ReconstructionDialog::getReconstruction() const
{
    return reconstruction;
}

void ReconstructionDialog::setReconstruction(Reconstruction *reconstruction)
{
    this->reconstruction = reconstruction;

    id = reconstruction->getId();
    prevId = id;
    description = reconstruction->getDescription();
    subjects = reconstruction->getSubjects();
    subjectsFileList = reconstruction->getSubjectsFileList();
    outputPath = reconstruction->getOutputPath();

    programPath = reconstruction->getProgramPath();
    execState = reconstruction->getExecStatus();

    parameters.clear();
    QMapIterator<QString, Reconstruction::Parameter> itp(reconstruction->getParameters());
    while(itp.hasNext()) {
        itp.next();
        parameters.insert(itp.key(), itp.value());
    }

    parametersData.clear();
    programDataSet = false;
    readProgramData();

    inputInfoImages.clear();
    for(int i = 0; i < reconstruction->getInputInfoImages().size(); i++) {
        ReconstructionInputInfo inputInfo(reconstruction->getInputInfoImages().at(i));
        inputInfoImages.append(inputInfo);
    }
    inputDialog->setInputInfoImages(&inputInfoImages);
    inputDialog->setSource(this, reconstruction->getReconstructionUuid().toString());

    outputImages.clear();
    for(int i = 0; i < reconstruction->getImagesOut().size(); i++) {
        AmosImage* image = new AmosImage(*(reconstruction->getImagesOut().at(i)));
        outputImages.append(image);
    }
    outputDialog->setImagesList(&outputImages);
    outputDialog->setSource(this, reconstruction->getReconstructionUuid().toString());

    subjectsFileList.clear();

    for(int i = 0; i < reconstruction->getSubjectsFileList().size(); i++) {
        QFileInfo fileInfo(reconstruction->getSubjectsFileList().at(i));
        if(fileInfo.isFile() && fileInfo.exists()) {
            subjectsFileList.append(reconstruction->getSubjectsFileList().at(i));
        }
    }
    fillUi();
    reconstructionSet = true;
}

void ReconstructionDialog::updateSubjects()
{
    subjects = reconstruction->getSubjects();
}

void ReconstructionDialog::fillUi()
{
    ui->idLineEdit->setText(id);
    setWindowTitle(id);
    ui->descriptionTextEdit->setPlainText(description);

    //inputInfoImages can be changed from outside
    inputInfoImages.clear();
    for(int i = 0; i < reconstruction->getInputInfoImages().size(); i++) {
        ReconstructionInputInfo inputInfo(reconstruction->getInputInfoImages().at(i));
        inputInfoImages.append(inputInfo);
    }
    inputDialog->fillUi();

    //outputImages reconstructionsUsing can be changed from outside
    outputDialog->setImagesPath(outputPath);
    for(int i = 0; i < outputImages.size(); i++) {
        reconstruction->getImagesOut().at(i)->
                setEnableEdit(outputImages.at(i)->getEnableEdit());
        delete outputImages.at(i);
    }
    outputImages.clear();
    for(int i = 0; i < reconstruction->getImagesOut().size(); i++) {
        AmosImage* image = new AmosImage(*(reconstruction->getImagesOut().at(i)));
        outputImages.append(image);
    }
    outputDialog->fillUi();

    fillSubjectsTable();
    for(int i = ui->loadSubjectsComboBox->count() - 2; i >= 0; i--) // not remove Load other
            ui->loadSubjectsComboBox->removeItem(i);
    subjectsFileList.clear();

    for(int i = 0; i < reconstruction->getSubjectsFileList().size(); i++) {
        QFileInfo fileInfo(reconstruction->getSubjectsFileList().at(i));
        if(fileInfo.isFile() && fileInfo.exists()) {
            subjectsFileList.append(reconstruction->getSubjectsFileList().at(i));
            ui->loadSubjectsComboBox->insertItem(i, fileInfo.fileName());
        }
    }
    
    if(reconstruction->getInternalExec()) {
        ui->internalExecutionCheckBox->setChecked(true);
        ui->internalExecutionCheckBox->setEnabled(false);
        ui->programPathWidget->hide(); 
    }

//    readProgramData();
    setProgramData();
    fillParametersTable();

    ui->programPathLineEdit->setText(programPath);
    stateButtons.at(execState)->setChecked(true);
    if(execState == Reconstruction::PARTIAL_FINISH || execState == Reconstruction::FINISH) {
        ui->unsetRadioButton->setEnabled(false);
        ui->setRadioButton->setEnabled(false);        
    }
}

bool ReconstructionDialog::dialogActivated(bool activated, QewDialogInactivate from,
                                           bool noSaveChildren)
{

    bool changed = getDataChangedThis();
    bool ret = QewExtensibleDialog::dialogActivated(activated, from, noSaveChildren);

    if(ret && !activated && changed) {
        selectableImages->updateList(this, reconstruction->getReconstructionUuid().toString(),
                                     outputImages);

//      "addReconstructionToImage" not for outputImages
        
        if(from != QewExtensibleDialog::FromDeleting) {

            for(int i = 0; i < inputInfoImages.size(); i++) {
                QStringList uuidList;
                uuidList.append(inputInfoImages.at(i).getImageUuid());
                uuidList.append(reconstruction->getReconstructionUuid().toString());
                uuidList.append(reconstruction->getId());
                emit emitExternVal("addReconstructionToImage", QVariant::fromValue(uuidList));
            }
        }
        else {
            QString reconstructionUuid = reconstruction->getReconstructionUuid().toString();
            for(int i = 0; i < reconstruction->getInputInfoImages().size(); i++) {
                QStringList uuidList;
                uuidList.append(reconstruction->getInputInfoImages().at(i).getImageUuid());
                uuidList.append(reconstructionUuid);
                removedInputInfos.append(uuidList);
            }            
        }

        for(int i = 0; i < removedInputInfos.size(); i++)
            emit emitExternVal("removeReconstructionFromImage",
                                   QVariant::fromValue(removedInputInfos.at(i)));
        removedInputInfos.clear();
    }
    if(ret && activated) {
        outputDialog->dialogActivated(activated, from, noSaveChildren);
        fillUi();
        setDataChanged(false);
    }

    return ret;
}

void ReconstructionDialog::askReconstructionState()
{
    outputDialog->askReconstructionState();
}

void ReconstructionDialog::editingFinished()
{
    if(prevId != ui->idLineEdit->text()) {
        emit sendIdChanged(getItemTree(), ui->idLineEdit->text().trimmed());
        prevId = ui->idLineEdit->text().trimmed();
        setWindowTitle(prevId);
        setDataChangedThis(true);
    }
}

void ReconstructionDialog::parameterChanged(int row, int /*col*/)
{
    QTableWidgetItem * itemName = ui->parametersTableWidget->item(row, 0);
    QTableWidgetItem * itemPrefix = ui->parametersTableWidget->item(row, 1);
    QTableWidgetItem * itemValue = ui->parametersTableWidget->item(row, 2);
    setDataChangedThis(true);
    if(!itemName || !itemValue) // adding row
        return;

    disconnect(ui->parametersTableWidget, SIGNAL(cellChanged(int, int)),
            this, SLOT(parameterChanged(int, int)));

    Reconstruction::Parameter param;

    param.name = itemName->text().trimmed();
    bool validName = true;
    if((parameters.contains(param.name) && param.name != prevParameterName)
            || param.name.isEmpty()) {
        QString title(tr("Parameter name"));
        QString text(tr("Duplicated or empty parameter name.") + "\n" +
                     tr("Restoring previuos name: ") + prevParameterName);
        QMessageBox::warning(0, title, text);
        param.name = prevParameterName;
        itemName->setText(prevParameterName);
        validName = false;
    }
    param.value = itemValue->text().trimmed();
    if(param.value.isEmpty()) {
        QString title(tr("Parameter value"));
        QString text(tr("Empty value for: ") + param.name + "\n" +
                     tr("It will not be used in execution."));
        QMessageBox::warning(0, title, text);
    }
    param.prefix = itemPrefix->text().trimmed();

    parameters.remove(prevParameterName);
    parameters.insert(param.name, param);
    if(validName)
        prevParameterName = "";

    connect(ui->parametersTableWidget, SIGNAL(cellChanged(int, int)),
            this, SLOT(parameterChanged(int, int)));
}

void ReconstructionDialog::parameterToBeChanged(int row, int /*col*/)
{
    QTableWidgetItem * itemName = ui->parametersTableWidget->item(row, 0);
    prevParameterName = itemName->text();

}

void ReconstructionDialog::boolParamChanged()
{
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
    if (checkBox) {
        int row = checkBox->objectName().toInt();
        QTableWidgetItem * itemName = ui->parametersTableWidget->item(row, 0);
        setDataChangedThis(true);

        QString paramName = itemName->text().trimmed();
        if(parameters.contains(paramName)) {
            Reconstruction::Parameter& param = parameters[paramName];
            QString value = checkBox->isChecked() ? "true" : "false";
            param.value = value;            
        }
    }    
}

void ReconstructionDialog::doubleParamChanged()
{
    QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    if (spinBox) {
        int row = spinBox->objectName().toInt();
        QTableWidgetItem * itemName = ui->parametersTableWidget->item(row, 0);
        setDataChangedThis(true);

        QString paramName = itemName->text().trimmed();
        if(parameters.contains(paramName)) {
            Reconstruction::Parameter& param = parameters[paramName];
            param.value = QString::number(spinBox->value());            
        }
    }
}

void ReconstructionDialog::intParamChanged()
{
    QSpinBox* spinBox = qobject_cast<QSpinBox*>(sender());
    if (spinBox) {
        int row = spinBox->objectName().toInt();
        QTableWidgetItem * itemName = ui->parametersTableWidget->item(row, 0);
        setDataChangedThis(true);

        QString paramName = itemName->text().trimmed();
        if(parameters.contains(paramName)) {
            Reconstruction::Parameter& param = parameters[paramName];
            param.value = QString::number(spinBox->value());            
        }
    }
}

void ReconstructionDialog::enumParamChanged()
{
    QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox) {
        int row = comboBox->objectName().toInt();
        QTableWidgetItem * itemName = ui->parametersTableWidget->item(row, 0);
        setDataChangedThis(true);

        QString paramName = itemName->text().trimmed();
        if(parameters.contains(paramName)) {
            Reconstruction::Parameter& param = parameters[paramName];
            param.value = QString::number(comboBox->currentIndex());            
        }
    }
}

void ReconstructionDialog::selectSubjects()
{
    selectSubjects(true);
}

void ReconstructionDialog::unselectSubjects()
{
    selectSubjects(false);
}

void ReconstructionDialog::onSubjectItemChecked(QTableWidgetItem *item)
{
    if(item->column() == 0) {
        QString val;
        bool select = false;
        if(item->checkState() == Qt::Checked) {
            val = "1";
            select = true;
        }
        else if(item->checkState() == Qt::Unchecked)
            val = "0";
        ui->subjectsTableWidget->item(item->row(), 2)->setText(val);
        subjects.insert(ui->subjectsTableWidget->item(item->row(), 1)->text(), select);
        //setDataChangedThis(true);
    }
}

void ReconstructionDialog::loadSubjectsFileFromList(int index)
{
    if(index >= subjectsFileList.size())
        loadSubjectsFile();
    else {
        subjectsFilename = subjectsFileList.at(index);
        bool openDialog = false;
        loadSubjectsFile(openDialog);
    }
}

void ReconstructionDialog::showActionsMenu(const QPoint &pos)
{
    QTableWidgetItem *item = ui->parametersTableWidget->itemAt(pos);
    if(!item)
        return;
    if(item->column() != 2)
        return;
    selectedParamItem = item;
    QList<QAction*> actions;

    QAction* pathAction = new QAction(tr("Select path"), this);
    pathAction->setToolTip(tr("Select path"));
    connect(pathAction, SIGNAL(triggered()), this, SLOT(selectParameterPath()));
    actions.append(pathAction);

    QAction* inputAction = new QAction(tr("Select input image"), this);
    inputAction->setToolTip(tr("Select input image"));
    connect(inputAction, SIGNAL(triggered()), this, SLOT(selectInputImage()));
    actions.append(inputAction);

    QAction* outputAction = new QAction(tr("Select output image"), this);
    outputAction->setToolTip(tr("Select output image"));
    connect(outputAction, SIGNAL(triggered()), this, SLOT(selectOutputImage()));
    actions.append(outputAction);

    if(actions.size() > 0) {
        RightCLickMenu menu;
        menu.setActions(actions);
        menu.exec(ui->parametersTableWidget->mapToGlobal(pos));
    }
}

void ReconstructionDialog::selectParameterPath()
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename = QDir::homePath();
    QString sender = tr("Choose parameter path");
    QString type_file = "";
    QString extensions = "";
    bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
                                           QFileDialog::ExistingFile, type_file, extensions);
    if(!ret)
        return;
    QString param = selectedParamItem->text();
    param += " " + filename;
    selectedParamItem->setText(param);
    setDataChangedThis(true);
}

void ReconstructionDialog::selectInputImage()
{
    selectParamImage(inputInfoImages, tr("Choose input image"));
}

void ReconstructionDialog::selectOutputImage()
{
    QList<ReconstructionInputInfo > list;
    for(int i = 0; i < outputImages.size(); i++) {
        QString source = this->windowTitle();
        QString type = outputImages.at(i)->getTypeName();
        QString name = outputImages.at(i)->getFileName();
        QString imageUuid = outputImages.at(i)->getImageUuid().toString();
        QString reconsUuid = reconstruction->getReconstructionUuid().toString();
        ReconstructionInputInfo inputInfo(source, type, name, imageUuid, reconsUuid);
        list.append(inputInfo);
    }
    selectParamImage(list, tr("Choose output image"));
}

void ReconstructionDialog::selectParamImage(QList<ReconstructionInputInfo> list, QString dialogTitle)
{
    ChooseInputImagesDialog dialog;
    setWindowTitle(dialogTitle);
    dialog.hideSource();
    dialog.setSelectableImages(list);
    int ret = dialog.exec();

    if(ret == QDialog::Rejected)
        return;
    QList<ReconstructionInputInfo> dialogList = dialog.getSelectedImages();
    if(dialogList.size() == 0 || dialogList.size() == 0)
        return;
    QString param = selectedParamItem->text();
    for(int i = 0; i < dialogList.size(); i++)
        param += " " + dialogList.at(i).getImageName();
    selectedParamItem->setText(param);  
    setDataChangedThis(true);
}


void ReconstructionDialog::selectSubjects(bool select)
{
    disconnect(ui->subjectsTableWidget, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(onSubjectItemChecked(QTableWidgetItem*)));

    QList<QTableWidgetItem*> items = ui->subjectsTableWidget->selectedItems();
    QList<int> rows = Utils::getRowsForItems(items);
    int cnt = rows.size();
    for(int i = 0; i < cnt; i++) {
        subjects.insert(ui->subjectsTableWidget->item(rows.at(i), 1)->text(), select);
        ui->subjectsTableWidget->item(rows.at(i), 0)->setCheckState(select ? Qt::Checked : Qt::Unchecked);
        ui->subjectsTableWidget->item(rows.at(i), 2)->setText(select ? "1" : "0");
        ui->subjectsTableWidget->item(rows.at(i), 0)->setSelected(false);
        ui->subjectsTableWidget->item(rows.at(i), 1)->setSelected(false);
    }
    if(cnt > 0 || select) {
        ui->subjectsTableWidget->sortByColumn(1, Qt::AscendingOrder);
        // first position for selected items
        ui->subjectsTableWidget->sortByColumn(2, Qt::DescendingOrder);
    }
    else { // if nothing selected the unselect buttom sort only by ids
        ui->subjectsTableWidget->sortByColumn(1, Qt::AscendingOrder);
    }

    //setDataChangedThis(true);

    connect(ui->subjectsTableWidget, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(onSubjectItemChecked(QTableWidgetItem*)));
}


void ReconstructionDialog::loadSubjectsFile(bool openDialog)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename = subjectsFilename;
    QString docName = XMLDocName();
    QString sender = tr("Open subjects file");
    if(openDialog) {
        QString type_file = "";
        QString extensions = "asl";
        bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
                              QFileDialog::ExistingFile, type_file, extensions);
        if(!ret)
            return;
    }
    QDomDocument doc = xml_operator->loadXML(filename, docName, sender);

    QDomNode n = doc.firstChild();
    if(n.isNull())
        return;
    QDomElement e_root = n.toElement();
    if(openDialog) {
        if(e_root.isNull())
            return;
        if(e_root.tagName() != "Subjects")
            return;
        subjectsFilename = filename;
    }

    setXMLElement(e_root);

    fillSubjectsTable();
    selectSubjects();

    QFileInfo fileInfo(subjectsFilename);
    if(!subjectsFileList.contains(subjectsFilename)) {
        subjectsFileList.append(subjectsFilename);
        ui->loadSubjectsComboBox->insertItem(subjectsFileList.size() - 1,
                                             fileInfo.fileName());
        ui->loadSubjectsComboBox->setCurrentIndex(subjectsFileList.size() - 1);
        //setDataChangedThis(true);
    }
    else
       ui->loadSubjectsComboBox->setCurrentIndex(
                   ui->loadSubjectsComboBox->findText(fileInfo.fileName()));
    
}

void ReconstructionDialog::saveSubjectsFile()
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString content = saveXML();
    QString sender = tr("Save subjects file");
    QString type_file = "";
    QString extensions = "asl";
    bool change_name = true;
    bool ret = xml_operator->saveDocument(subjectsFilename, content, type_file,
                                          extensions, sender, change_name);
    if(ret && !subjectsFileList.contains(subjectsFilename)) {
        subjectsFileList.append(subjectsFilename);
        QFileInfo fileInfo(subjectsFilename);
        ui->loadSubjectsComboBox->insertItem(subjectsFileList.size() - 1,
                                             fileInfo.fileName());
        ui->loadSubjectsComboBox->setCurrentIndex(subjectsFileList.size() - 1);

        QString title(tr("Saving subjects file"));
        QString text(tr("Consider the inclusion of information") + "\n" +
                     tr("about this file in the description field."));
        QMessageBox::information(0, title, text);
    }
    setDataChangedThis(true);
}

QString ReconstructionDialog::XMLDocName()
{
    return "Subjects_execution";
}


void ReconstructionDialog::getExternVal(const QString &var_name, const QVariant &var_val)
{
    bool paramsChanged = false;
    if(var_name == "addReconstructionToImage") {
        QStringList list = var_val.toStringList();
        if(list.size() != 3)
            return;
        if(reconstruction->getReconstructionUuid().toString() == list.at(1))
            return;
        QString source = list.at(2);

        for(int i = 0; i < reconstruction->getImagesOut().size(); i++) {
            if(reconstruction->getImagesOut().at(i)->getImageUuid().toString() == list.at(0)) {
                if(!QUuid(list.at(1)).isNull()) {
                    outputImages.at(i)->addReconstruction(list.at(1), source);
                    reconstruction->getImagesOut().at(i)->addReconstruction(list.at(1), source);
                }
            }
        }

        bool changeInReconstruction = true;
        QList<ReconstructionInputInfo> listSelect = selectableImages->getList(this);
        int cnt = reconstruction->getInputInfoImages().size();
        for(int i = 0; i < cnt; i++) {
            if(reconstruction->getInputInfoImages().at(i).getReconstructionUuid() == list.at(1)) {
                int index = listSelect.indexOf(reconstruction->getInputInfoImages().at(i));
                if(index > -1) {
                    QString oldImageName = reconstruction->getInputInfoImages().at(i).getImageName();
                    QString imageType = listSelect.at(index).getImageType();
                    QString imageName = listSelect.at(index).getImageName();
                    QString imageUuid = listSelect.at(index).getImageUuid();
                    QString sourceUuid = listSelect.at(index).getReconstructionUuid();
                    reconstruction->getInputInfoImages().removeAt(i);
                    reconstruction->getInputInfoImages().insert(i, ReconstructionInputInfo(source, imageType,
                                                          imageName, imageUuid, sourceUuid));
                    paramsChanged = changeImageNameInParams(oldImageName, imageName, 
                                                            changeInReconstruction);

                }
            }
        }
    }
    else if(var_name == "removeReconstructionFromImage") {
        QStringList list = var_val.toStringList();
        if(list.size() != 2)
            return;
//        for(int i = 0; i < outputImages.size(); i++) {
//            if(outputImages.at(i)->getImageUuid().toString() == list.at(0)) {
//                if(!QUuid(list.at(1)).isNull()) {
//                    outputImages.at(i)->removeReconstruction(list.at(1));
//                }
//            }
//        }
        for(int i = 0; i < reconstruction->getImagesOut().size(); i++) {
            if(reconstruction->getImagesOut().at(i)->getImageUuid().toString() == list.at(0)) {
                if(!QUuid(list.at(1)).isNull()) {
                    outputImages.at(i)->removeReconstruction(list.at(1));
                    reconstruction->getImagesOut().at(i)->removeReconstruction(list.at(1));
                }
            }
        }
        // TODO params as in addReconstructionToImage
    }
    else if(var_name == "askReconstructionState") {
        QStringList list = var_val.toStringList();
        QString reconsUuid = list.at(0);
        QString imageUuid = list.at(1);
        if(reconsUuid != reconstruction->getReconstructionUuid().toString())
            return;

        QStringList response;
        response.append(reconstruction->getReconstructionUuid().toString());
        response.append(execState == Reconstruction::UNSET ? "false" : "true");
        response.append(imageUuid);
        emit emitExternVal("returnReconstructionState", QVariant(response));
    }
//    else if(var_name == "outputPathChanged") {
//        QStringList response = var_val.toStringList();
//        if(response.size() != 2)
//            return;
//        reconstruction->changeRootOutputPath(response.at(0), response.at(1));
//        rootOutputPath(response.at(1));
//    }

    if(paramsChanged) {
        fillParametersTable();
        setDataChangedThis(true);
    }

    if(this != QVariantPtr<QewExtensibleDialog>::asPtr(var_val))
        return;

    if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Save))
        dialogActivated(false, QewExtensibleDialog::FromSave);
    else if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Restore))
        dialogActivated(false, QewExtensibleDialog::FromRestore);
    else if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Remove))
        emit sendRemoveDialog(this);
}


bool ReconstructionDialog::changeImageNameInParams(QString oldName, QString newName,
                                     bool changeInReconstruction)
{
   bool changed = false;
   if(oldName != newName) {
       QMap<QString, Reconstruction::Parameter>::iterator it;
       QMap<QString, Reconstruction::Parameter>::iterator itb;
       QMap<QString, Reconstruction::Parameter>::iterator ite;
       if(changeInReconstruction) {
           itb = reconstruction->getParameters().begin();
           ite = reconstruction->getParameters().end();
       }
       else {
           itb = parameters.begin();
           ite = parameters.end();           
       }
       for (it = itb; it != ite; ++it) {
           Reconstruction::Parameter param = it.value();
           if(param.value.contains(oldName)) {
               param.value.replace(oldName, newName);
               it.value() = param;
               changed = true;
           }
       }
   }
   return changed;
}

bool ReconstructionDialog::removeImageFromParams(QString name, bool changeInReconstruction)
{
    bool changed = false;
    QMap<QString, Reconstruction::Parameter>::iterator it;
    QMap<QString, Reconstruction::Parameter>::iterator itb;
    QMap<QString, Reconstruction::Parameter>::iterator ite;
    if(changeInReconstruction) {
        itb = reconstruction->getParameters().begin();
        ite = reconstruction->getParameters().end();
    }
    else {
        itb = parameters.begin();
        ite = parameters.end();           
    }
    for (it = itb; it != ite; ++it) {
        Reconstruction::Parameter param = it.value();
        if(param.value.contains(name)) {
            param.value.remove(name);
            it.value() = param;
            changed = true;
        }
    }
    return changed;
}

void ReconstructionDialog::imageRemovedFromList(QString uuid, QString name)
{
    bool found = false;
    bool changeParams = false;
    bool changeInReconstruction = false;
    for(int i = 0; i < inputInfoImages.size(); i++) {
        if(inputInfoImages.at(i).getImageUuid() == uuid) {
            found = true;
            changeParams = removeImageFromParams(name, changeInReconstruction);
        }
    }
    if(!found) {
        for(int i = 0; i < outputImages.size(); i++) {
            if(outputImages.at(i)->getImageUuid() == uuid) {
                changeParams = removeImageFromParams(name, changeInReconstruction);
            }
        }
    }
    if(changeParams)
        fillParametersTable();
}

void ReconstructionDialog::imageNameChangedInList(QString uuid, QString name)
{
    bool changeParams = false;
    bool changeInReconstruction = false;

    for(int i = 0; i < outputImages.size(); i++) {
        if(outputImages.at(i)->getImageUuid() == uuid) {
            changeParams = changeImageNameInParams(name, outputImages.
                             at(i)->getFileName(), changeInReconstruction);
        }
    }

    if(changeParams) {
        fillParametersTable();
        setDataChangedThis(true);
    }
}

void ReconstructionDialog::stateButtonClicked(int index)
{
    if(execState != static_cast<Reconstruction::ExecStatus>(index)) {
        execState = static_cast<Reconstruction::ExecStatus>(index);
        if(execState != Reconstruction::RUNNING)
            setDataChangedThis(true);
        if(execState == Reconstruction::UNSET)
            ui->subjectsTableWidget->setEnabled(false);
        else
            ui->subjectsTableWidget->setEnabled(true);
    }
}

void ReconstructionDialog::setRootOutputPath(const QString &rootOutputPath)
{
    if(reconstructionSet)
        reconstruction->changeRootOutputPath(this->rootOutputPath, rootOutputPath, subjectsAsMainDirs);
    if(!subjectsAsMainDirs)        
        outputPath.replace(this->rootOutputPath, rootOutputPath);
    this->rootOutputPath = rootOutputPath;
    outputDialog->setRootPath(rootOutputPath);
}

void ReconstructionDialog::setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction *> &menuActions)
{
    this->menuActions = menuActions;
}

QList<QAction *> ReconstructionDialog::setMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(menuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::Save) {
            it.value()->setText(tr("Update reconstruction"));
            it.value()->setToolTip(tr("Update selected reconstruction"));
        }
        else if(it.key() == RightCLickMenu::Remove) {
            it.value()->setText(tr("Remove reconstruction"));
            it.value()->setToolTip(tr("Remove selected reconstruction"));
        }
        else if(it.key() == RightCLickMenu::Restore) {
            it.value()->setText(tr("Restore reconstruction"));
            it.value()->setToolTip(tr("Restore reconstruction information\nto previously saved"));
        }
        actions.append(it.value());
    }
    return actions;
}

bool ReconstructionDialog::getDataChangedThis() const
{
    return QewExtensibleDialog::getDataChangedThis() || inputDialog->getDataChangedThis()
            || outputDialog->getDataChangedThis();
}

void ReconstructionDialog::saveThis()
{
    id = prevId;
    description = ui->descriptionTextEdit->toPlainText();
    programPath = ui->programPathLineEdit->text().trimmed();
    outputPath = outputDialog->getImagesPath();

    reconstruction->setId(id);    
    reconstruction->setDescription(description);

    removedInputInfos.clear();
    QString reconstructionUuid = reconstruction->getReconstructionUuid().toString();
    for(int i = 0; i < reconstruction->getInputInfoImages().size(); i++) {
        if(!inputInfoImages.contains(reconstruction->getInputInfoImages().at(i))) {
            QStringList uuidList;
            uuidList.append(reconstruction->getInputInfoImages().at(i).getImageUuid());
            uuidList.append(reconstructionUuid);
            removedInputInfos.append(uuidList);
        }
    }
    reconstruction->getInputInfoImages().clear();
    for(int i = 0; i < inputInfoImages.size(); i++)
        reconstruction->getInputInfoImages().append(inputInfoImages.at(i));

    reconstruction->setOutputPath(outputPath);
    for(int i = 0; i < reconstruction->getImagesOut().size(); i++)
        delete reconstruction->getImagesOut().at(i);
    reconstruction->getImagesOut().clear();
    for(int i = 0; i < outputImages.size(); i++) {
        if(subjectsAsMainDirs)
            outputImages.at(i)->setParentPath(rootOutputPath);
        else
            outputImages.at(i)->setParentPath(outputPath);
        outputImages.at(i)->setPath(outputPath);
        AmosImage* image = new AmosImage(*(outputImages.at(i)));
        reconstruction->getImagesOut().append(image);
    }

    reconstruction->getSubjects().clear();
    QMapIterator<QString, bool> its(subjects);
    while(its.hasNext()) {
        its.next();
        reconstruction->getSubjects().insert(its.key().trimmed(), its.value());
    }
    reconstruction->setProcessedSubjects(processedSubjects);
    reconstruction->setSubjectsFileList(subjectsFileList);

    reconstruction->setProgramPath(programPath);
    reconstruction->setExecStatus(execState);
    reconstruction->getParameters().clear();
    QMapIterator<QString, Reconstruction::Parameter> it(parameters);
    while(it.hasNext()) {
        it.next();
        Reconstruction::Parameter param = it.value();
        param.name = it.key().trimmed();
        param.prefix = param.prefix.trimmed();
        param.value = param.value.trimmed();
        reconstruction->getParameters().insert(it.key().trimmed(), param);
    }

    setDataChanged(false);
    qDebug("Saving ReconstructionDialog");
}

bool ReconstructionDialog::validateThis()
{
//    QMapIterator<QString, QString> it(parameters);
//    while(it.hasNext()) {
//        it.next();
//        if(it.key().isEmpty())
//    }
    return true;
}

void ReconstructionDialog::cleanThis()
{
    for(int i = 0; i < outputImages.size(); i++)
        delete outputImages.at(i);
    outputImages.clear();
}

void ReconstructionDialog::restoreThis()
{
    qDebug("Restoring ReconstructionDialog");
    prevId = id;

    QFileInfo fileInfo(subjectsFilename);

    if(fileInfo.isFile() && fileInfo.exists()) {
        bool openDialog = false;
        loadSubjectsFile(openDialog);
    }
    execState = reconstruction->getExecStatus();
    fillUi();
    emit sendIdChanged(getItemTree(), id);
    setDataChanged(false);
}

void ReconstructionDialog::setMaximumThreads(int maximumThreads)
{
    this->maximumThreads = maximumThreads;
    ui->threadsSpinBox->setMaximum(maximumThreads);
    ui->threadsSpinBox->setValue(maximumThreads); 
}

int ReconstructionDialog::getNumberOfThreads()
{
    return numThreads;
}

int ReconstructionDialog::getNumberOfSubjects()
{
    return numSubjects;
}

void ReconstructionDialog::enableMaxSubjects(bool enable)
{
    ui->subjectsSpinBox->setEnabled(enable);
}

void ReconstructionDialog::setMaxSubjects(int maximumSubjects)
{
    maximumSubjects = maximumSubjects > maximumThreads ? maximumThreads : maximumSubjects;
    this->maximumSubjects = maximumSubjects;
    ui->subjectsSpinBox->setMaximum(maximumSubjects);
//     if(ui->subjectsSpinBox->value() > maximumSubjects)
        ui->subjectsSpinBox->setValue(maximumSubjects);
}

void ReconstructionDialog::setNumberOfThreads(int numThreads)
{
    this->numThreads = numThreads;
    if(maximumSubjects != numThreads) 
        ui->subjectsSpinBox->setMaximum(numThreads);
    if(maximumSubjects > numThreads) 
        maximumSubjects = numThreads;        
    if(numSubjects > numThreads) {
        numSubjects = maximumSubjects;
        ui->subjectsSpinBox->setValue(maximumSubjects);
    }
        
}

void ReconstructionDialog::setNumberOfSubjects(int numSubjects)
{
    numSubjects = numSubjects > numThreads ? numThreads : numSubjects;
    this->numSubjects = numSubjects;
}

void ReconstructionDialog::setDataChangedThis(bool dataChanged)
{
    QewTabDialog::setDataChangedThis(dataChanged);
    if(dataChanged)
        ui->runPushButton->setEnabled(false);
    else
        ui->runPushButton->setEnabled(true && (execState == Reconstruction::SET || execState == Reconstruction::PARTIAL_FINISH));
}

QStringList ReconstructionDialog::getSelectedSubjects()
{
    return selectedSubjects;
}

void ReconstructionDialog::performProcess()
{
    if(ui->runningRadioButton->isChecked()) { // reconstruction is running
        emit sendPerformReconstruction(this);
        return;
    }
        
//     QStringList selectedSubjects;
    selectedSubjects.clear();
    QMapIterator<QString, bool> it(subjects);
    while(it.hasNext()) {
        it.next();
        if(it.value())
            selectedSubjects.append(it.key());
    }
    
    if(selectedSubjects.size() == 0) {
        QString title(tr("No subjects"));
        QString text(tr("Select some subjects in subjects tab\n before running the process."));
        QMessageBox::warning(0, title, text);
        return;
    }
    
    ui->runningRadioButton->setChecked(true);
    QList<QWidget*> tabs = getContainerPages();
    for(int i = 0; i < tabs.size() - 1; i++)
        tabs.at(i)->setEnabled(false);
    QWidget* programTab = tabs.at(tabs.size() - 1);
    QList<QWidget *> childWidgets = programTab->findChildren<QWidget *>();
    for(int i = 0; i < childWidgets.size(); i++)
        childWidgets.at(i)->setEnabled(false);
    ui->runPushButton->setEnabled(true);
    prevRunEnabled = true;
    ui->runPushButton->setText(tr("Stop"));
    
    ui->progressBatchLabel->setEnabled(true);
    ui->processBatchProgressBar->setEnabled(true);
    ui->processBatchProgressBar->reset();
    ui->caseLabel->setEnabled(true);
    ui->caseProgressBar->setEnabled(true);
    ui->caseProgressBar->reset();
    ui->clockLcdNumber->setEnabled(true);
    
    emit sendPerformReconstruction(this);
    
}

void ReconstructionDialog::processFinished(QStringList fullyProcessedSubjects)
{
    QList<QWidget*> tabs = getContainerPages();
    for(int i = 0; i < tabs.size() - 1; i++)
        tabs.at(i)->setEnabled(true);
    QWidget* programTab = tabs.at(tabs.size() - 1);
    QList<QWidget *> childWidgets = programTab->findChildren<QWidget *>();
    for(int i = 0; i < childWidgets.size(); i++)
        childWidgets.at(i)->setEnabled(true);
    ui->runPushButton->setText(tr("Run"));
    enableParametersEdition(false);
    selectedSubjects.clear();
    
    int totalProcessedSubjects = fullyProcessedSubjects.size() + reconstruction->getProcessedSubjects().size();
    
    if(totalProcessedSubjects < subjects.size() && totalProcessedSubjects > 0) {
        execState = Reconstruction::PARTIAL_FINISH;
        ui->unsetRadioButton->setEnabled(false);
        ui->setRadioButton->setEnabled(false); 
    }        
    else if(totalProcessedSubjects == subjects.size()) {
        execState = Reconstruction::FINISH;
        ui->unsetRadioButton->setEnabled(false);
        ui->setRadioButton->setEnabled(false);         
    }
    else {
        ui->unsetRadioButton->setEnabled(true);
        ui->setRadioButton->setEnabled(true);         
    }
    stateButtons.at(execState)->setChecked(true);
    
    if(fullyProcessedSubjects.size() > 0) {
        disableProcessedSubjects(fullyProcessedSubjects);
        setDataChangedThis(true);
    }
}

void ReconstructionDialog::setProgressBarVisible(bool visible)
{
    ui->progressBatchLabel->setVisible(visible);
    ui->processBatchProgressBar->setVisible(visible);    
}

void ReconstructionDialog::setProcessProgressBarValue(int progress)
{
    ui->processBatchProgressBar->setValue(progress);
}

void ReconstructionDialog::setProgressProcessType(QString progressType)
{
    ui->progressBatchLabel->setText(progressType);
}

void ReconstructionDialog::setCaseName(QString caseName)
{
    ui->caseLabel->setText(caseName);
}

void ReconstructionDialog::setCaseProgressRange(int numCases)
{
    ui->caseProgressBar->setMaximum(numCases);
    ui->caseProgressBar->reset();    
}

void ReconstructionDialog::setCaseProgress(int caseNum)
{
   ui->caseProgressBar->setValue(caseNum); 
}

void ReconstructionDialog::setMaximumBatchProgressBar(int numSlices)
{
    ui->processBatchProgressBar->setVisible(true);
    ui->processBatchProgressBar->setMaximum(numSlices);
    ui->processBatchProgressBar->reset();
    ui->progressBatchLabel->setVisible(true);    
}

void ReconstructionDialog::displayTime(QString time)
{
    ui->clockLcdNumber->display(time);
}

bool ReconstructionDialog::getSubjectsAsMainDirs()
{
    return subjectsAsMainDirs;
}

void ReconstructionDialog::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
    if(outputDialog)
        outputDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
}

void ReconstructionDialog::showPrefixParamColumn(bool show)
{
    ui->parametersTableWidget->setColumnHidden(1, !show);
}

void ReconstructionDialog::programDataChangedFromMenu()
{
    readProgramData();
    removeParameters();
    setProgramData();
    fillParametersTable();    
    
}

void ReconstructionDialog::delayedEmitDialogSaved()
{
    emit emitDialogSaved();
}
