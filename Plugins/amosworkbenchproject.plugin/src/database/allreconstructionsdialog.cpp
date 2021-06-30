#include "allreconstructionsdialog.h"
#include "ui_allreconstructionsdialog.h"
#include "reconstruction.h"
#include "qvariantptr.h"

// #include <QInputDialog>
#include <QMessageBox>

AllReconstructionsDialog::AllReconstructionsDialog(QWidget *parent) :
    QewTreeViewDialog(parent),
    ui(new Ui::AllReconstructionsDialog)
{
    ui->setupUi(this);
    setAcceptMeansClose(false);
    typesToRemove.clear();
    typesToRemove << AmosImage::FLAIR << AmosImage::T1 <<
             AmosImage::MASK << AmosImage::EXPERT <<
             AmosImage::AMOS_AC << AmosImage::AMOS_BC;
             
    maximumThreads = 1;
    
//     readProgramsIdAndInternals();
}

AllReconstructionsDialog::~AllReconstructionsDialog()
{
    delete ui;
    qDebug("Deleting AllReconstructionsDialog");
}

void AllReconstructionsDialog::addExtensibleChild(QewExtensibleDialog *extensible_child, const QString &caption,
                               QTreeWidgetItem *parentItem, int index)
{
    if(extensible_child->inherits("ReconstructionDialog"))
        dialogs.append((ReconstructionDialog*)extensible_child);
    QewTreeViewDialog::addExtensibleChild(extensible_child, caption, parentItem, index);
}

bool AllReconstructionsDialog::setUpDialog()
{
    bool ret = QewTreeViewDialog::setUpDialog();
    if(ret)
        readProgramsIdAndInternals();
    return ret;
}

void AllReconstructionsDialog::setSelectableImages(ReconstructionInputList *selectableImages)
{
    this->selectableImages = selectableImages;
    for(int i = 0; i < dialogs.size(); i++)
        dialogs.at(i)->setSelectableImages(selectableImages);
}

void AllReconstructionsDialog::changeEvent(QEvent *e)
{
    QewTreeViewDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QList<ReconstructionDialog *> AllReconstructionsDialog::getDialogs() const
{
    return dialogs;
}

void AllReconstructionsDialog::removeDialogs()
{
    for(int i = dialogs.size() - 1; i >= 0; i--)
        removeExtensibleChild(extensibleChildren().at(i));
    extensibleChildren().clear();
    dialogs.clear();
}

void AllReconstructionsDialog::getExternVal(const QString &var_name, const QVariant &var_val)
{
    if(this != QVariantPtr<QewExtensibleDialog>::asPtr(var_val))
        return;

    if(var_name == "addElement")
        newReconstruction();
}

QMap<QString, QPair<QString, QString> > AllReconstructionsDialog::getProgramIdAndInternals()
{
    return programIdAndInternals;
}

void AllReconstructionsDialog::readProgramsIdAndInternals()
{
    XMLOperator* xml_operator = XMLOperator::instance();
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
    int cnt = n_processes.count();
    QDomElement e_process;
    programIdAndInternals.clear();
    for(int i = 0; i < cnt; ++i) {
        QDomNode n = n_processes.item(i);
        e_process = n.toElement();
        if(!e_process.isNull()) {
            QDomElement e_progId = xml_operator->findTag("program_id", e_process);
            QDomElement e_type = xml_operator->findTag("process_type", e_process);
            QDomElement e_internal = xml_operator->findTag("internal_exec", e_process);
            QDomElement e_prefixes = xml_operator->findTag("has_prefixes", e_process);
            if(!e_progId.isNull() && !e_type.isNull()) {            
                if(e_type.text().toLower() == getProcessType()) {
                    programIdAndInternals[e_progId.text().trimmed()] = QPair<QString, QString>(e_internal.text().trimmed(), e_prefixes.text().trimmed());
                }

            }
        }
    }    
}

void AllReconstructionsDialog::setupNewReconstructionDialog(NewReconstructionDialog& newDialog)
{
    newDialog.setProgramIDAndInternals(programIdAndInternals);
}

void AllReconstructionsDialog::newReconstruction()
{
//     bool ok;
//     QString id = QInputDialog::getText(this, tr("New reconstruction"),
//                                          tr("Reconstruction id:"), QLineEdit::Normal, "", &ok);
    NewReconstructionDialog newDialog(this);
    setupNewReconstructionDialog(newDialog);
    int result = newDialog.exec();
    QString id = newDialog.getID();
    QString programID = newDialog.getProgramID();
    bool internalExec = newDialog.getInternalExec();
    bool hasPrefixes = newDialog.getHasPrefixes();
//     if (ok && !id.isEmpty()) {
    if (result == QDialog::Accepted && !id.isEmpty()) {        
        int index = childExists(id);
        if(index == -1) {
            Reconstruction* reconstruction = createNewReconstruction(id, programID, internalExec, hasPrefixes);
            addReconstruction(reconstruction);

            QewTreeViewDialog* extensibleParent = 0;
            QTreeWidget* treeWidget = getSuperTreeView(&extensibleParent);
            index = childExists(id, treeWidget->currentItem());
            QTreeWidgetItem* item = treeWidget->currentItem()->child(index);
            treeWidget->setCurrentItem(item);
            extensibleParent->setCurrentWidget(item);

            emit emitDialogSaved();
        }
        else
            currentItemTextNotChanged(tr("Existing reconstruction ") + id);
    }
}

Reconstruction* AllReconstructionsDialog::createNewReconstruction(const QString& id, const QString& programID, bool internalExec, bool hasPrefixes)
{
    Reconstruction* reconstruction = new Reconstruction;
    reconstruction->setId(id);
    reconstruction->setProgramId(programID);
    reconstruction->setInternalExec(internalExec);
    reconstruction->setHasPrefixes(hasPrefixes);
    reconstruction->setSubjectIds(subjectIds);
    reconstructions->append(reconstruction);
    return reconstruction;
}

void AllReconstructionsDialog::addReconstruction(Reconstruction* reconstruction)
{
    ReconstructionDialog* dialog = getDialog();
    dialog->setTypesToRemove(typesToRemove);
    dialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
    dialog->setUpDialog();
    dialog->setMaximumThreads(maximumThreads);
//    dialog->createOutputDialog();

    connect(dialog, SIGNAL(emitActivated(const QString&)), this, SIGNAL(emitActivated(const QString&)));
    connect(dialog, SIGNAL(sendIdChanged(QTreeWidgetItem*, QString)), this, SIGNAL(sendIdChanged(QTreeWidgetItem*, QString)));
    connect(dialog, SIGNAL(emitDialogSaved()), this, SIGNAL(emitDialogSaved()));
    connect(dialog, SIGNAL(sendRemoveDialog(ReconstructionDialog* )),
            this, SLOT(removeReconstructionDialog(ReconstructionDialog* )));
    connect(dialog, SIGNAL(sendPerformReconstruction(ReconstructionDialog* )), this, SIGNAL(sendPerformReconstruction(ReconstructionDialog* )));

    dialog->setRootOutputPath(rootOutputPath);
    dialog->setReconstruction(reconstruction);
    addExtensibleChild(dialog, reconstruction->getId(), getItemTree());

    dialog->setSelectableImages(selectableImages);
    dialog->setMenuActions(childMenuActions);
}

ReconstructionDialog *AllReconstructionsDialog::getDialog()
{
    ReconstructionDialog* dialog = new ReconstructionDialog(this);
    dialog->setSaveButtonShowsUpdate(true);
    return dialog;
}

void AllReconstructionsDialog::setRootOutputPath(const QString &rootOutputPath)
{
    this->rootOutputPath = rootOutputPath;
    for(int i = 0; i < dialogs.size(); i++)
        dialogs.at(i)->setRootOutputPath(rootOutputPath);
}

void AllReconstructionsDialog::setSubjectIds(const QStringList &subjectIds)
{
    this->subjectIds = subjectIds;
}

void AllReconstructionsDialog::changeSubjectIds(const QStringList &subjectIds)
{
    setSubjectIds(subjectIds);
    for(int i = 0; i < reconstructions->size(); i++)
        reconstructions->at(i)->setSubjectIds(subjectIds); 
    for(int i = 0; i < dialogs.size(); i++)
        dialogs.at(i)->updateSubjects();        
}

void AllReconstructionsDialog::removeReconstructionDialog(ReconstructionDialog *dialog)
{
    QString type;
    if(this->inherits("AllAmosSegmentationsDialog"))
        type = tr("segmentation");
    else
        type = tr("reconstruction");
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Removing ") + type);
    msgBox.setText(tr("Removing ") + type + tr("\nDo you want to continue?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int ret = msgBox.exec();
    if(ret == QMessageBox::Cancel)
        return;

    // For removing inputinfos
    dialog->setDataChanged(true);
    dialog->dialogActivated(false, QewExtensibleDialog::FromDeleting);
    
    removeExtensibleChild(dialog, getItemTree());
    dialogs.removeOne(dialog);
    Reconstruction* recons = dialog->getReconstruction();
    delete recons;
    reconstructions->removeOne(recons);    
    emit sendSubChildRemoved(getItemTree());
    emit emitDialogSaved();
}

void AllReconstructionsDialog::setReconstructions(QList<Reconstruction *> *reconstructions)
{
    this->reconstructions = reconstructions;
    for(int i = 0; i < this->reconstructions->size(); i++)
        addReconstruction(this->reconstructions->at(i));
}

void AllReconstructionsDialog::askReconstructionState()
{
    for(int i = 0; i < dialogs.size(); i++)
        dialogs.at(i)->askReconstructionState();
}

void AllReconstructionsDialog::setChildMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction *> &childMenuActions)
{
    this->childMenuActions = childMenuActions;
}

void AllReconstructionsDialog::setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction *> &menuActions)
{
    this->menuActions = menuActions;
}

QList<QAction *> AllReconstructionsDialog::setMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(menuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::Add) {
            it.value()->setText(tr("Add reconstruction"));
            it.value()->setToolTip(tr("Add a new reconstruction"));
        }
        actions.append(it.value());
    }
    return actions;
}

void AllReconstructionsDialog::setMaximumThreads(int maximumThreads)
{
    this->maximumThreads = maximumThreads;
}


bool AllReconstructionsDialog::getSubjectsAsMainDirs()
{
    return subjectsAsMainDirs;
}

void AllReconstructionsDialog::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
}

void AllReconstructionsDialog::currentItemTextNotChanged(const QString &text)
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

QString AllReconstructionsDialog::getProcessType()
{
    return "reconstruction";
}

