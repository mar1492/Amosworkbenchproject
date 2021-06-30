#include "externimagesdialog.h"
#include "ui_externimagesdialog.h"
#include "imageslistdialog.h"
#include "qvariantptr.h"

ExternImagesDialog::ExternImagesDialog(QWidget *parent) :
    QewSimpleDialog(parent),
    ui(new Ui::ExternImagesDialog)
{
    ui->setupUi(this);
    selectableImages = 0;
}

ExternImagesDialog::~ExternImagesDialog()
{
    delete ui;
}

bool ExternImagesDialog::setUpDialog()
{
    bool ret = QewSimpleDialog::setUpDialog();

    bool forInputList = false;
    bool individualPaths = true;
    listDialog = new ImagesListDialog(this, forInputList, individualPaths);
    listDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
    listDialog->setRootPath(rootPath);
    listDialog->setWindowTitle(tr("External images"));
    listDialog->setImageDialogTitle(tr("Choose extern image"));
    QString title = tr("List dialog");
    addExtensibleChild(listDialog, title);

    return ret = listDialog->setUpDialog();
}

void ExternImagesDialog::changeEvent(QEvent *e)
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

void ExternImagesDialog::fillUi()
{
    for(int i = 0; i < externImages.size(); i++)
        delete externImages.at(i);
    externImages.clear();
    for(int i = 0; i < originalExternImages->size(); i++) {
        AmosImage* image = new AmosImage(*(originalExternImages->at(i)));
        externImages.append(image);
    }
    listDialog->setImagesList(&externImages);
    listDialog->fillUi();
}

void ExternImagesDialog::saveThis()
{
    for(int i = 0; i < originalExternImages->size(); i++)
        delete originalExternImages->at(i);
    originalExternImages->clear();
    for(int i = 0; i < externImages.size(); i++) {
        AmosImage* image = new AmosImage(*(externImages.at(i)));
        originalExternImages->append(image);
    }

}

void ExternImagesDialog::cleanThis()
{
    for(int i = 0; i < externImages.size(); i++)
        delete externImages.at(i);
    externImages.clear();
}

void ExternImagesDialog::restoreThis()
{
    qDebug("Restoring ExternImagesDialog");
    fillUi();
    setDataChanged(false);
}

QString ExternImagesDialog::getDialogUuid() const
{
    return externUuid;
}

void ExternImagesDialog::setDialogUuid(const QString &externUuid)
{
    this->externUuid = externUuid;
}

void ExternImagesDialog::setAnyFile(bool anyFile)
{
    listDialog->setAnyFile(anyFile);
}

void ExternImagesDialog::setExternImages(QList<AmosImage *> *originalExternImages)
{
    this->originalExternImages = originalExternImages;
    fillUi();
}

void ExternImagesDialog::setSelectableImages(ReconstructionInputList *selectableImages)
{
    this->selectableImages = selectableImages;
}

bool ExternImagesDialog::dialogActivated(bool activated, QewDialogInactivate from,
                                         bool noSaveChildren)
{
    bool changed = getDataChangedThis();
    bool ret = QewExtensibleDialog::dialogActivated(activated, from, noSaveChildren);
    ret = ret && listDialog->dialogActivated(activated, from, noSaveChildren);

    if(ret && selectableImages && changed) {
        selectableImages->updateList(this, externUuid, externImages);
        for(int i = 0; i < externImages.size(); i++) {
            QStringList uuidList;
            uuidList.append(externImages.at(i)->getImageUuid().toString());
            uuidList.append(externUuid);
            uuidList.append(windowTitle());
            emit emitExternVal("addReconstructionToImage", QVariant::fromValue(uuidList));
        }
    }

    return ret;
}

void ExternImagesDialog::askReconstructionState()
{
    listDialog->askReconstructionState();
}

void ExternImagesDialog::getExternVal(const QString &var_name, const QVariant &var_val)
{
    if(var_name == "addReconstructionToImage") {
        QStringList list = var_val.toStringList();
        if(list.size() != 3)
            return;
        if(externUuid == list.at(1))
            return;

        for(int i = 0; i < originalExternImages->size(); i++) {
            if(originalExternImages->at(i)->getImageUuid().toString() == list.at(0)) {
                if(!QUuid(list.at(1)).isNull()) {
                    originalExternImages->at(i)->addReconstruction(list.at(1), list.at(2));
                    externImages.at(i)->addReconstruction(list.at(1), list.at(2));
                }
            }
        }
    }
    else if(var_name == "removeReconstructionFromImage") {
        QStringList list = var_val.toStringList();
        if(list.size() != 2)
            return;
        for(int i = 0; i < originalExternImages->size(); i++) {
            if(originalExternImages->at(i)->getImageUuid().toString() == list.at(0)) {
                if(!QUuid(list.at(1)).isNull()) {
                    originalExternImages->at(i)->removeReconstruction(list.at(1));
                    externImages.at(i)->removeReconstruction(list.at(1));
                }
            }
        }
    }

    if(this != QVariantPtr<QewExtensibleDialog>::asPtr(var_val))
        return;

    if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Save))
        dialogActivated(false, QewExtensibleDialog::FromSave);
    else if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Restore))
        dialogActivated(false, QewExtensibleDialog::FromRestore);
}

QList<QAction *> ExternImagesDialog::setMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(menuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::Save) {
            it.value()->setText(tr("Update extern images"));
            it.value()->setToolTip(tr("Update extern images list"));
        }
        else if(it.key() == RightCLickMenu::Restore) {
            it.value()->setText(tr("Restore extern images"));
            it.value()->setToolTip(tr("Restore extern images list\nto previously saved"));
        }
        actions.append(it.value());
    }
    return actions;
}

void ExternImagesDialog::setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction *> &menuActions)
{
    this->menuActions = menuActions;
}

bool ExternImagesDialog::getDataChangedThis() const
{
    return listDialog->getDataChangedThis();
}

bool ExternImagesDialog::getSubjectsAsMainDirs()
{
    return subjectsAsMainDirs;
}

void ExternImagesDialog::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
    if(listDialog)
        listDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
}

void ExternImagesDialog::setRootPath(const QString &rootPath)
{
    this->rootPath = rootPath;
    if(listDialog)
        listDialog->setRootPath(rootPath);
}
