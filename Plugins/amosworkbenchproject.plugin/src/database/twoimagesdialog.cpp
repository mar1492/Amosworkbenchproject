#include "twoimagesdialog.h"
#include "ui_twoimagesdialog.h"
#include "imagedialog.h"
#include "amosimage.h"
#include "reconstructioninputlist.h"
#include "qvariantptr.h"

#include <QMessageBox>

TwoImagesDialog::TwoImagesDialog(QWidget *parent) : QewSimpleDialog(parent),
                        ui(new Ui::TwoImagesDialog)
{
    ui->setupUi(this);
    setAcceptMeansClose(false);
    firstTitle = tr("Image");
    secondTitle = tr("Image");

    saveActionText = tr("Update images");
    saveActionToolTip = tr("Update images information");

    restoreActionText = tr("Restore images");
    restoreActionToolTip = tr("Restore images information\nto previously saved");
    setCountInitPages(false);

    connect(ui->pathLineEdit, SIGNAL(editingFinished()), this, SLOT(setPath()));
    connect(ui->pathToolButton, SIGNAL(clicked(bool)), this, SLOT(choosePath()));

    showPath = true;
    inputList = 0;
    dialogUuid = QUuid().toString();
    anyFile = true;
    invalidFile = false;
    changedOtherThanDescription = false;
    subjectsAsMainDirs = false;
}

TwoImagesDialog::~TwoImagesDialog()
{
    delete ui;
    qDebug("Deleting TwoImagesDialog");
}

void TwoImagesDialog::showPathWidget(bool show)
{
    showPath = show;
    if(showPath) {
        firstDialog->hidePathSelection();
        secondDialog->hidePathSelection();
    }
    else
        ui->pathWidget->hide();
}

bool TwoImagesDialog::setUpDialog()
{
    bool ret = QewSimpleDialog::setUpDialog();
    if(ret) {
        firstDialog = new ImageDialog(this);
        if(firstDialog->setUpDialog()) {
            firstDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
            firstDialog->hideTypeSelection();
            firstDialog->setTitle(firstTitle);
            addExtensibleChild(firstDialog, firstTitle);
        }
        secondDialog = new ImageDialog(this);
        if(secondDialog->setUpDialog()) {
            secondDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
            secondDialog->hideTypeSelection();
            secondDialog->setTitle(secondTitle);
            addExtensibleChild(secondDialog, secondTitle);
        }
    }
    return ret;
}

void TwoImagesDialog::setPath()
{

    firstDialog->enableOtherThanPath(ui->pathLineEdit->text().trimmed());
    secondDialog->enableOtherThanPath(ui->pathLineEdit->text().trimmed());

    if(path == ui->pathLineEdit->text().trimmed())
        return;

    if(!anyFile) {
        QFileInfo fileInfo;
        if(!subjectsAsMainDirs)
            fileInfo.setFile(ui->pathLineEdit->text().trimmed());
        else
            fileInfo.setFile(rootPath);
        if(!fileInfo.exists()) {
            ui->pathLineEdit->blockSignals(true);
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
            invalidFile = true;
            ui->pathLineEdit->blockSignals(false);
//            changedOtherThanDescription = false;
            return;
        }
    }
//    if(path != ui->pathLineEdit->text()) {
        if(showPath) {
            bool fromUser = true;
            firstDialog->setPath(ui->pathLineEdit->text().trimmed(), fromUser);
            secondDialog->setPath(ui->pathLineEdit->text().trimmed(), fromUser);
        }
        changedOtherThanDescription = true;
        setDataChangedThis(true);
//    }
}

QString TwoImagesDialog::getDialogUuid() const
{
    return dialogUuid;
}

AmosImage *TwoImagesDialog::getSecondImage() const
{
    return secondImage;
}

AmosImage *TwoImagesDialog::getFirstImage() const
{
    return firstImage;
}

void TwoImagesDialog::setAnyFile(bool anyFile)
{
    this->anyFile = anyFile;
    firstDialog->setAnyFile(anyFile);
    secondDialog->setAnyFile(anyFile);
}

void TwoImagesDialog::setDialogUuid(const QString &dialogUuid)
{
    this->dialogUuid = dialogUuid;
}

void TwoImagesDialog::setFirstImage(AmosImage *image)
{
    firstImage = image;
    firstDialog->setImage(image);
}

void TwoImagesDialog::setSecondImage(AmosImage *image)
{
    secondImage = image;
    secondDialog->setImage(image);
}

void TwoImagesDialog::changeEvent(QEvent *e)
{
    QewSimpleDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TwoImagesDialog::saveThis()
{
    path = ui->pathLineEdit->text().trimmed();
    changedOtherThanDescription = false;
    setDataChanged(false);
    qDebug("Saving TwoImagesDialog");
}

bool TwoImagesDialog::validateThis()
{
    qDebug("Validating TwoImagesDialog");
    return true;
}

void TwoImagesDialog::cleanThis()
{
    qDebug("Cleaning TwoImagesDialog");
}

void TwoImagesDialog::restoreThis()
{
    emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::Restore),
                       QVariantPtr<QewExtensibleDialog>::asQVariant(firstDialog));
    emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::Restore),
                       QVariantPtr<QewExtensibleDialog>::asQVariant(secondDialog));
    qDebug("Restoring TwoImagesDialog");
    fillUi();
}

void TwoImagesDialog::setInputList(ReconstructionInputList *list)
{
    inputList = list;
}

// void TwoImagesDialog::choosePath()
// {
//     XMLOperator* xml_operator = XMLOperator::instance();
//     QString filename;
//     if(rootPath.isEmpty())
//         filename = QDir::homePath();
//     else
//         filename = rootPath;    
//     if(!ui->pathLineEdit->text().trimmed().isEmpty())
//         filename = ui->pathLineEdit->text().trimmed();
//     QString sender = tr("Scans directory");
//     QString type_file = "";
//     QString extensions = "";
//     bool anyFile = false;
//    QFileDialog::FileMode filemode = anyFile ? QFileDialog::AnyFile : QFileDialog::ExistingFile;
//     QFileDialog::FileMode filemode = QFileDialog::Directory;
// 
//     bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
//                                            filemode, type_file, extensions, QFileDialog::ShowDirsOnly, rootPath);
//     if(!ret)
//         return;
//     
//     if(path != filename.trimmed()) {
//         if(showPath) {
//             bool fromUser = true;
//             firstDialog->setPath(filename.trimmed(), fromUser);
//             secondDialog->setPath(filename.trimmed(), fromUser);
//         }
// 
//         remove names ???
//         changedOtherThanDescription = true;
//         setDataChangedThis(true);
//     }
//     if(subjectsAsMainDirs) {
//         pathWithSubject = filename.trimmed();
//         QFileInfo fileInfo(filename.trimmed());  
//         ui->pathLineEdit->setText(fileInfo.fileName());        
//     }
//     else
//         ui->pathLineEdit->setText(filename.trimmed());
// }

void TwoImagesDialog::choosePath()
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename;
    if(rootPath.isEmpty())
        filename = QDir::homePath();
    else
        filename = rootPath;    
    if(!ui->pathLineEdit->text().trimmed().isEmpty())
        filename = ui->pathLineEdit->text().trimmed();
    QString sender = tr("Scans directory");
    QString type_file = "";
    QString extensions = "";
//     bool anyFile = false;
//    QFileDialog::FileMode filemode = anyFile ? QFileDialog::AnyFile : QFileDialog::ExistingFile;
    QFileDialog::FileMode filemode = QFileDialog::Directory;

    bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
                                           filemode, type_file, extensions, QFileDialog::ShowDirsOnly, rootPath);
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
        if(showPath) {
            bool fromUser = true;
            firstDialog->setPath(newPath, fromUser);
            secondDialog->setPath(newPath, fromUser);
        }

        // remove names ???
        changedOtherThanDescription = true;
        setDataChangedThis(true);
    }

    ui->pathLineEdit->setText(newPath);
}

QString TwoImagesDialog::getPath() const
{
    return path;
}

bool TwoImagesDialog::dialogActivated(bool activated, QewDialogInactivate from,
                                      bool noSaveChildren)
{
    if(invalidFile) {
       invalidFile = false;
       return false;
    }
    if(firstDialog->getInvalidFile()) {
        firstDialog->setInvalidFile(false);
        return false;
    }
    if(secondDialog->getInvalidFile()) {
        secondDialog->setInvalidFile(false);
        return false;
    }

    bool changedOtherFirst = firstDialog->getChangedOtherThanDescription();
    bool changedOtherSecond = secondDialog->getChangedOtherThanDescription();
    bool changed = changedOtherThanDescription || changedOtherFirst
                        || changedOtherSecond;

    if(changed)
        preDialogActivated(activated, &from);

    bool ret = QewExtensibleDialog::dialogActivated(activated, from, noSaveChildren);

    if(activated) {
        ret = ret && firstDialog->dialogActivated(activated, from, noSaveChildren);
        ret = ret && secondDialog->dialogActivated(activated, from, noSaveChildren);
        fillUi();
    }

    if(ret && inputList && changed) {        
        if(changedOtherFirst || changedOtherSecond) {
            QList<AmosImage*> list;
            list << firstImage << secondImage;
            inputList->updateList(this, dialogUuid, list);
        }

        QStringList uuidList;
        uuidList.append(firstImage->getImageUuid().toString());
        uuidList.append(dialogUuid);
        uuidList.append(windowTitle());
        if(firstDialog->getChangedOtherThanDescription())
            emit emitExternVal("addReconstructionToImage", QVariant::fromValue(uuidList));
        uuidList[0] = secondImage->getImageUuid().toString();
        if(secondDialog->getChangedOtherThanDescription())
            emit emitExternVal("addReconstructionToImage", QVariant::fromValue(uuidList));
    }

    postDialogActivated(ret);

    return ret;
}

void TwoImagesDialog::askReconstructionState()
{
    firstDialog->askReconstructionState();
    secondDialog->askReconstructionState();
}

void TwoImagesDialog::preDialogActivated(bool /*activated*/, QewExtensibleDialog::QewDialogInactivate */*from*/)
{

}

void TwoImagesDialog::postDialogActivated(bool /*response*/)
{

}

void TwoImagesDialog::setSecondTitle(const QString &secondTitle)
{
    this->secondTitle = secondTitle;
}

void TwoImagesDialog::setFirstTitle(const QString &firstTitle)
{
    this->firstTitle = firstTitle;
}

void TwoImagesDialog::setSaveActionText(const QString &saveActionText)
{
    this->saveActionText = saveActionText;
}

void TwoImagesDialog::setSaveActionToolTip(const QString &saveActionToolTip)
{
    this->saveActionToolTip = saveActionToolTip;
}

void TwoImagesDialog::setRestoreActionText(const QString &restoreActionText)
{
    this->restoreActionText = restoreActionText;
}

void TwoImagesDialog::setRestoreActionToolTip(const QString &restoreActionToolTip)
{
    this->restoreActionToolTip = restoreActionToolTip;
}

bool TwoImagesDialog::getDataChangedThis() const
{
    bool ret = QewExtensibleDialog::getDataChangedThis() || firstDialog->getDataChangedThis() || secondDialog->getDataChangedThis();
    return ret;
}

void TwoImagesDialog::setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction *> &menuActions)
{
    this->menuActions = menuActions;
}

QList<QAction *> TwoImagesDialog::setMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(menuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::Save) {
            it.value()->setText(saveActionText);
            it.value()->setToolTip(saveActionToolTip);
        }
        else if(it.key() == RightCLickMenu::Restore) {
            it.value()->setText(restoreActionText);
            it.value()->setToolTip(restoreActionToolTip);
        }
        actions.append(it.value());
    }
    return actions;
}

void TwoImagesDialog::fillUi()
{
    ui->pathLineEdit->setText(path);
    firstDialog->fillUi();
    secondDialog->fillUi();
    changedOtherThanDescription = false;
    setDataChangedThis(false);
    if(!firstImage->getEnableEdit() || !secondImage->getEnableEdit()) {
        ui->pathLineEdit->setEnabled(false);
        ui->pathToolButton->setEnabled(false);
    }
    else {
        ui->pathLineEdit->setEnabled(true);
        ui->pathToolButton->setEnabled(true);
    }
}

void TwoImagesDialog::getExternVal(const QString &var_name, const QVariant &var_val)
{
    if(this != QVariantPtr<QewExtensibleDialog>::asPtr(var_val))
        return;

    if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Save))
        dialogActivated(false, QewExtensibleDialog::FromSave);
    else if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Restore))
        dialogActivated(false, QewExtensibleDialog::FromRestore);
}

void TwoImagesDialog::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
    firstDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
    secondDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);   
}

void TwoImagesDialog::setRootPath(QString rootPath)
{
    this->rootPath = rootPath;
    firstDialog->setParentPath(rootPath);
    secondDialog->setParentPath(rootPath);
}

void TwoImagesDialog::changeRootPath(QString rootPath)
{
    setRootPath(rootPath);
    firstImage->setParentPath(rootPath);
    secondImage->setParentPath(rootPath);
    if(!subjectsAsMainDirs) {
        firstImage->setPath(rootPath);
        secondImage->setPath(rootPath);
    }
}

void TwoImagesDialog::setOneValidSubjectId(QString oneValidSubjectId) const
{
    firstDialog->setOneValidSubjectId(oneValidSubjectId);
    secondDialog->setOneValidSubjectId(oneValidSubjectId);
}
