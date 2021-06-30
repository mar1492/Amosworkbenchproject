#include "imagedialog.h"
#include "ui_imagedialog.h"
#include "qvariantptr.h"
#include "rightclickmenu.h"

#include <QFileDialog>
#include <QMessageBox>

ImageDialog::ImageDialog(QWidget *parent) :
    QewExtensibleDialog(parent),
    ui(new Ui::ImageDialog)
{
    ui->setupUi(this);
    image = 0;
    type = 0;
    name = "";
    path = "";
    hideTypeSelect = false;
    hidePathSelect = false;
    anyFile = true;
    invalidFile = false;
    changedOtherThanDescription = false;
    topDir = "";
    oneValidSubjectId = "";
    
    connect(ui->pathLineEdit, SIGNAL(editingFinished()), this, SLOT(setPath()));
    connect(ui->nameLineEdit, SIGNAL(editingFinished()), this, SLOT(setName()));
    connect(ui->descriptionTextEdit, SIGNAL(textChanged()), this, SLOT(onDescriptionModified()));

    connect(ui->pathToolButton, SIGNAL(clicked(bool)), this, SLOT(choosePath()));
    connect(ui->nameToolButton, SIGNAL(clicked(bool)), this, SLOT(chooseName()));
    connect(ui->typeComboBox, SIGNAL(activated(int)), this, SLOT(chooseType(int)));
    
    connect(ui->cleanToolButton, SIGNAL(clicked(bool)), this, SLOT(eraseData()));
    setAcceptMeansClose(false);

    fillListTypes();
}

ImageDialog::~ImageDialog()
{
    delete ui;
    qDebug("Deleting ImageDialog");
}

void ImageDialog::fillListTypes()
{
    for(int i = 1; i < AmosImage::TypesCount; i++) {
        QString typeName = AmosImage::getTypeName(static_cast<AmosImage::ImageType>(i));
        ui->typeComboBox->addItem(typeName);
        itemTypeToPos[typeName] = i;
    }
}

void ImageDialog::hideTypeSelection()
{
    ui->typeWidget->hide();
    hideTypeSelect = true;
}

void ImageDialog::hidePathSelection()
{
    ui->pathWidget->hide();
    hidePathSelect = true;
}

void ImageDialog::showNameToolButton(bool show)
{
    ui->nameToolButton->setVisible(show);
}

void ImageDialog::removeTypes(QList<AmosImage::ImageType> types)
{
   for(int i = types.size() - 1; i >= 0; i--) {
       ui->typeComboBox->removeItem(types.at(i) - 1); // -1 NONE
   }
}

void ImageDialog::hideTitleLabel()
{
    ui->titleLabel->hide();
}

void ImageDialog::setTitle(const QString &title)
{
    ui->titleLabel->setText(title);
}

void ImageDialog::setImage(AmosImage *image)
{
    this->image = image;
    if(image) {
        path = image->getPath();
        name = image->getFileName();
        type = image->getType();
        description = image->getDescription();
    }
    else {
        path = "";
        name = "";
        type = AmosImage::NONE;
        description = "";
    }
    fillUi();

}

bool ImageDialog::getSubjectsAsMainDirs()
{
    return subjectsAsMainDirs;
}

void ImageDialog::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
}

void ImageDialog::setPath(const QString &path, bool fromUser)
{
    this->path = path;
    ui->pathLineEdit->setText(path);
    if(fromUser)
        changedOtherThanDescription = true;

//     enableOtherThanPath(path);
    if(subjectsAsMainDirs)
        enableOtherThanPath(parentPath);
    else
        enableOtherThanPath(path);    
}

QString ImageDialog::getParentPath() const
{
    return parentPath;
}

void ImageDialog::setParentPath(const QString& parentPath)
{
    this->parentPath = parentPath;
}

void ImageDialog::getExternVal(const QString &var_name, const QVariant &var_val)
{
    if(var_name == "addReconstructionToImage") {
        if(!image)
            return;
        QStringList list = var_val.toStringList();
        if(list.size() != 3)
            return;
        QWidget* w = parentWidget();
        QewExtensibleDialog* p = QewExtensibleDialog::extensibleParent(w);
        if(p && p->windowTitle() == list.at(2))
            return;

        if(image->getImageUuid().toString() == list.at(0)) {
            if(!QUuid(list.at(1)).isNull())
                image->addReconstruction(list.at(1), list.at(2));
        }
    }
    else if(var_name == "removeReconstructionFromImage") {
        if(!image)
            return;
        QStringList list = var_val.toStringList();
        if(list.size() != 2)
            return;
        if(image->getImageUuid().toString() == list.at(0)) {
            if(!QUuid(list.at(1)).isNull())
                image->removeReconstruction(list.at(1));
        }
    }
    else if(var_name == "returnReconstructionState") {
        if(!image)
            return;

        QStringList response = var_val.toStringList();
        if(response.size() != 3)
            return;
        QString imageUuid = response.at(2);
        if(imageUuid != image->getImageUuid().toString())
            return;
        bool blocked = response.at(1) == "true" ? true : false;
        QString reconsUuid = response.at(0);
        int index = image->getReconstructionsUsing().indexOf(reconsUuid);
        if(index > -1) {
            image->setCntReconstructionsState(image->getCntReconstructionsState() + 1);
            if(blocked) {
                QString id = image->getReconstructionsUsingIds().at(index);
                if(!image->getBlockedReconstructionsId().contains(id)) {
                    image->getBlockedReconstructionsId().append(id);
                    saveThis();
                }
            }
        }
        if(image->getCntReconstructionsState() == image->getReconstructionsUsing().size()) {
            image->setEnableEdit(image->getBlockedReconstructionsId().size() == 0);
            enableEdition();
            if(!image->getEnableEdit()) {
                QStringList text = image->getBlockedReconstructionsId();
                text.prepend(tr("Image ") + image->getFileName() + tr(" is blocked by:"));
                emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::WarningBox), QVariant(text));
            }
        }
    }

    if(this != QVariantPtr<QewExtensibleDialog>::asPtr(var_val))
        return;
    if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Restore))
        restoreThis();
}


void ImageDialog::fillUi()
{
    ui->pathLineEdit->setText(path);
    ui->nameLineEdit->setText(name);
    if(!hideTypeSelect) {
        int index = ui->typeComboBox->findText(AmosImage::getTypeName(static_cast<AmosImage::ImageType>(type)));
        ui->typeComboBox->setCurrentIndex(index);
    }
    ui->descriptionTextEdit->setText(description);
    enableEdition();
    if(subjectsAsMainDirs)
        enableOtherThanPath(parentPath);
    else
        enableOtherThanPath(path);
    changedOtherThanDescription = false;
    setDataChangedThis(false);
}

bool ImageDialog::dialogActivated(bool activated, QewDialogInactivate from,
                                  bool noSaveChildren)
{
    if(activated) {
        askReconstructionState();
    }

    bool ret = QewExtensibleDialog::dialogActivated(activated, from, noSaveChildren);
    return ret;
}

void ImageDialog::askReconstructionState()
{
    image->getBlockedReconstructionsId().clear();
    image->setCntReconstructionsState(0);
    if(image->getReconstructionsUsing().size() == 0) {
        image->setEnableEdit(true);
        enableEdition();
    }
    else {
        for(int i = 0; i < image->getReconstructionsUsing().size(); i++) {
            QStringList list;
            list << image->getReconstructionsUsing().at(i)
                 << image->getImageUuid().toString();
            emit emitExternVal("askReconstructionState", QVariant::fromValue(list));
        }
    }
}

void ImageDialog::setAnyFile(bool anyFile)
{
    this->anyFile = anyFile;
}

void ImageDialog::choosePath()
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename = QDir::homePath();
    if(!ui->pathLineEdit->text().trimmed().isEmpty())
        filename = ui->pathLineEdit->text().trimmed();
    else if(subjectsAsMainDirs)
        filename = parentPath;
    QString sender = tr("Images directory");
    QString type_file = "";
    QString extensions = "";
//    QFileDialog::FileMode filemode = anyFile ? QFileDialog::AnyFile : QFileDialog::ExistingFile;
    QFileDialog::FileMode filemode = QFileDialog::Directory;

    bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
                                           filemode, type_file, extensions,
                                           QFileDialog::ShowDirsOnly, topDir);
    if(!ret)
        return;
    QString newPath;
    if(subjectsAsMainDirs) {
        QFileInfo fileInfo(filename.trimmed());  
        newPath = fileInfo.fileName();        
    }
    else
        newPath = filename.trimmed();    
    
    if(path != newPath) {
        changedOtherThanDescription = true;
        setDataChangedThis(true);
    }
//     if(subjectsAsMainDirs) {
//         QFileInfo fileInfo(filename.trimmed());
//         ui->pathLineEdit->setText(fileInfo.fileName());
//     }
//     else
    ui->pathLineEdit->setText(newPath);

    enableOtherThanPath(filename.trimmed());
}

void ImageDialog::chooseName()
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename;
    QString topDir;
    if(subjectsAsMainDirs) {
        filename = parentPath;
        topDir = parentPath;
    }
    else {
        filename = QDir::homePath();
        if(!ui->pathLineEdit->text().trimmed().isEmpty())
            topDir = ui->pathLineEdit->text().trimmed();
        else
            topDir = "";
    }
   if(!ui->nameLineEdit->text().trimmed().isEmpty())
       filename = ui->nameLineEdit->text().trimmed();
//    else if(!ui->pathLineEdit->text().trimmed().isEmpty())
//        filename = ui->pathLineEdit->text().trimmed();
//     if(!ui->pathLineEdit->text().trimmed().isEmpty()) {
//         filename = ui->pathLineEdit->text().trimmed();
//         if(!subjectsAsMainDirs)
//             topDir = filename;
//     }

    QString sender = tr("Image name");
    QString type_file = "";
    QString extension = "";
    QFileDialog::FileMode filemode = anyFile ? QFileDialog::AnyFile : QFileDialog::ExistingFile;
    QFileDialog::Options options = QFlags<QFileDialog::Option>();
    bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
                    filemode, type_file, extension, options, topDir);

//    fd = new QFileDialog(0, sender);
//    fd->setFileMode(filemode);
//    fd->setOption(QFileDialog::DontUseNativeDialog);
//    QFileInfo file(filename);

//    if(file.isDir())
//        fd->setDirectory(filename);
//    else {
//        fd->setDirectory(file.path());
//        fd->selectFile(file.fileName());
//    }
//    fd->setAcceptMode(QFileDialog::AcceptOpen);
//    connect(fd, SIGNAL(directoryEntered(QString)), this, SLOT(restrictDir(QString)));

//    bool ret = fd->exec() == QDialog::Accepted;
    if(!ret)
        return;
//    filename = fd->selectedFiles().at(0);
    QFileInfo fileInfo(filename);
//    if(fileInfo.absolutePath().left(path.size()) != path) {
//        QMessageBox msgBox;
//        msgBox.setStandardButtons(QMessageBox::Ok);
//        msgBox.setText(fileInfo.fileName() + tr(" does not exist in a subdirectory of") + "\n" +
//                       fileInfo.absolutePath() + "\n" +
//                       tr("changing to previous."));
//        msgBox.exec();
//        ui->nameLineEdit->setText(name);
//        ui->nameLineEdit->setFocus();
//        return;
//    }
    if(name != fileInfo.fileName().trimmed()) {
        changedOtherThanDescription = true;
        setDataChangedThis(true);
    }
    ui->nameLineEdit->setText(fileInfo.fileName().trimmed());
}

//void ImageDialog::restrictDir(QString dir)
//{
//    if(dir.left(ui->pathLineEdit->text().trimmed().size()) != ui->pathLineEdit->text().trimmed())
//        fd->setDirectory(ui->pathLineEdit->text().trimmed());
//}

void ImageDialog::setName()
{
    ui->nameLineEdit->blockSignals(true);
    if(!anyFile) {
        QFileInfo fileInfo;
        QFileInfo dirInfo;
        if(subjectsAsMainDirs) {            
            if(oneValidSubjectId.isEmpty())
                dirInfo.setFile(parentPath, ui->pathLineEdit->text().trimmed());
            else {
                dirInfo.setFile(parentPath, oneValidSubjectId);
                dirInfo.setFile(dirInfo.absoluteFilePath(), ui->pathLineEdit->text().trimmed());
            }                
            fileInfo.setFile(dirInfo.absoluteFilePath(),
                           ui->nameLineEdit->text().trimmed());                
        }
        else
            fileInfo.setFile(ui->pathLineEdit->text().trimmed(),
                           ui->nameLineEdit->text().trimmed());
            
        if(!fileInfo.exists()) {
            bool existsFile = false;
            if(!ui->nameLineEdit->text().trimmed().isEmpty()) {
                QDir dir(ui->pathLineEdit->text().trimmed());
                QFileInfoList subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
//                 subdirs.removeFirst(); // .
//                 subdirs.removeFirst(); // ..

                for(int i = 0; i < subdirs.size(); i++) {
                   fileInfo.setFile(subdirs.at(i).absoluteFilePath(),
                                    ui->nameLineEdit->text().trimmed());
                   existsFile = fileInfo.exists();
                   if(existsFile)
                       break;
                }
            }
            if(!existsFile) {
                QMessageBox msgBox;
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setText(fileInfo.absoluteFilePath() + "\n" +
                               tr("does not exist, changing to previous."));
                msgBox.exec();
                ui->nameLineEdit->setText(name);
                ui->nameLineEdit->setFocus();
    //            changedOtherThanDescription = false;
                invalidFile = true;
                ui->nameLineEdit->blockSignals(false);
                return;
            }
            else
                invalidFile = false;
        }
    }
    if(name != ui->nameLineEdit->text().trimmed()) {
        changedOtherThanDescription = true;
        setDataChangedThis(true);
    }
    ui->nameLineEdit->blockSignals(false);
}

void ImageDialog::setPath()
{
    ui->pathLineEdit->blockSignals(true);
    enableOtherThanPath(ui->pathLineEdit->text().trimmed());

    if(path == ui->pathLineEdit->text().trimmed()) {
        ui->pathLineEdit->blockSignals(false);
        return;
    }

    if(!anyFile && !hidePathSelect) {
        QFileInfo fileInfo;
        if(subjectsAsMainDirs) {
            QFileInfo dirInfo(parentPath, ui->pathLineEdit->text().trimmed());
            fileInfo.setFile(dirInfo.absoluteFilePath());
        }
        else
            fileInfo.setFile(ui->pathLineEdit->text().trimmed());         
        
        if(!fileInfo.exists()) {
            QMessageBox msgBox;
            msgBox.setStandardButtons(QMessageBox::Ok);

            QString text;
            if(ui->pathLineEdit->text().trimmed().isEmpty())
                text = tr("Empty path.");
            else
                text = ui->pathLineEdit->text().trimmed() + "\n" +
                            tr("Does not exist, changing to previous.");
            msgBox.setText(text);
            msgBox.exec();
            ui->pathLineEdit->setText(path);
            ui->pathLineEdit->setFocus();
//            changedOtherThanDescription = false;
            invalidFile = true;
            enableOtherThanPath(path);
            ui->pathLineEdit->blockSignals(false);
            return;
        }
        else
            invalidFile = false;
    }
//    if(path != ui->pathLineEdit->text().trimmed()) {
        changedOtherThanDescription = true;
        setDataChangedThis(true);
        ui->pathLineEdit->blockSignals(false);
//    }
}

void ImageDialog::chooseType(int type)
{
    QString typeName = ui->typeComboBox->itemText(type);
    if(this->type != itemTypeToPos.value(typeName)) {
        changedOtherThanDescription = true;
        setDataChangedThis(true);
    }
}

void ImageDialog::onDescriptionModified()
{
    setDataChangedThis(true);
}

void ImageDialog::changeEvent(QEvent *e)
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

void ImageDialog::saveThis()
{
    qDebug("Saving ImageDialog");

    QString typeName = ui->typeComboBox->currentText();
    type = itemTypeToPos.value(typeName);
    path = ui->pathLineEdit->text().trimmed();
    name = ui->nameLineEdit->text().trimmed();
    description = ui->descriptionTextEdit->toPlainText();

    if(image) {
        image->setPath(path.trimmed());
        if(subjectsAsMainDirs)
            image->setParentPath(parentPath);
        else
            image->setParentPath(path.trimmed());
        image->setFileName(name.trimmed());
        if(!hideTypeSelect)
            image->setType(static_cast<AmosImage::ImageType>(type));
        image->setDescription(description);
        changedOtherThanDescription = false;
        setDataChangedThis(false);
    }
}

bool ImageDialog::validateThis()
{
    qDebug("Validating ImageDialog");
    if(getAcceptMeansClose()) {
        QString typeName = ui->typeComboBox->currentText();
        bool ret = itemTypeToPos.value(typeName) > 0 &&
                (!ui->pathLineEdit->text().trimmed().isEmpty() ||
                 hidePathSelect) &&
                !ui->nameLineEdit->text().trimmed().isEmpty();
        return ret;
    }
    else
        return true;
}

void ImageDialog::restoreThis()
{
    qDebug("Restoring ImageDialog");
    fillUi();
}

void ImageDialog::enableEdition()
{
    bool enableEdit = image ? image->getEnableEdit() : true;
    ui->pathLineEdit->setEnabled(enableEdit);
    ui->pathToolButton->setEnabled(enableEdit);
    ui->nameLineEdit->setEnabled(enableEdit);
    ui->nameToolButton->setEnabled(enableEdit);
    ui->typeComboBox->setEnabled(enableEdit);
}

void ImageDialog::setTopDir(const QString &topDir)
{
    this->topDir = topDir;
}

void ImageDialog::enableOtherThanPath(QString pathText)
{
    bool enable;
    if(anyFile)
        enable = !pathText.trimmed().isEmpty();
    else {
        QFileInfo fileInfo(pathText.trimmed());
        enable = fileInfo.exists() && fileInfo.isDir();
    }

    bool enableEdit = image ? image->getEnableEdit() : true;
    ui->nameLineEdit->setEnabled(enable && enableEdit);
    ui->nameToolButton->setEnabled(enable && enableEdit);
    ui->typeComboBox->setEnabled(enable && enableEdit);
    ui->descriptionTextEdit->setEnabled(enable);
}

void ImageDialog::setInvalidFile(bool invalidFile)
{
    this->invalidFile = invalidFile;
}

bool ImageDialog::getInvalidFile() const
{
    return invalidFile;
}

bool ImageDialog::getChangedOtherThanDescription() const
{
    return changedOtherThanDescription;
}

void ImageDialog::eraseData()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Erasing image data"));
    msgBox.setText(tr("Do you want to continue?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int ret = msgBox.exec();
    if(ret == QMessageBox::Cancel)
        return;    
    
    ui->pathLineEdit->setText("");
    path = "";
    ui->nameLineEdit->setText("");
    name = "";
    ui->typeComboBox->clear();
    type = AmosImage::NONE;
    ui->descriptionTextEdit->setText("");
    description = "";
}

void ImageDialog::hideCleanWidget()
{
    ui->cleanWidget->hide();
}

void ImageDialog::setOneValidSubjectId(QString oneValidSubjectId)
{
    this->oneValidSubjectId = oneValidSubjectId;
}
