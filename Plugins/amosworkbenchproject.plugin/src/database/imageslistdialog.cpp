#include "imageslistdialog.h"
#include "ui_imageslistdialog.h"
#include "imagedialog.h"
#include "chooseinputimagesdialog.h"
#include "utils.hpp"
#include "xmloperator.h"
#include "rightclickmenu.h"

#include <qewdialogfactory.h>
#include <qewsimpledialog.h>
#include <QMessageBox>

ImagesListDialog::ImagesListDialog(QWidget *parent, bool forInputList, bool individualPaths) :
    QewExtensibleDialog(parent), forInputList(forInputList), individualPaths(individualPaths),
    ui(new Ui::ImagesListDialog)
{
    ui->setupUi(this);

    ui->imagesTableWidget->horizontalHeader()->setStretchLastSection(true);

    if(forInputList) {
        ui->pathWidget->hide();
        ui->imagesTableWidget->setColumnCount(ui->imagesTableWidget->columnCount() + 2);
        // delay is necessary for tables having 0 rows
        // without delay hiding does not work
        QTimer::singleShot(100, this, SLOT(delayedHideColumnsInput()));
        connect(ui->addToolButton, SIGNAL(clicked(bool)), this, SLOT(addInputInfos()));
        connect(ui->removeToolButton, SIGNAL(clicked(bool)), this, SLOT(removeInputInfos()));
    }
    else {
        ui->imagesTableWidget->setColumnCount(ui->imagesTableWidget->columnCount() + 1);
        // delay is necessary for tables having 0 rows
        // without delay hiding does not work
        QTimer::singleShot(100, this, SLOT(delayedHideColumnsOutput()));
        connect(ui->addToolButton, SIGNAL(clicked(bool)), this, SLOT(addImage()));
        connect(ui->removeToolButton, SIGNAL(clicked(bool)), this, SLOT(removeImages()));
        if(!individualPaths) {
            connect(ui->pathLineEdit, SIGNAL(editingFinished()), this, SLOT(setPath()));
            connect(ui->pathToolButton, SIGNAL(clicked(bool)), this, SLOT(choosePath()));
        }
        else
            ui->pathWidget->hide();
        connect(ui->imagesTableWidget, SIGNAL(cellDoubleClicked(int, int)),
                this, SLOT(cellSelected(int, int)));
    }

    selectableImages = 0;
    source = this;
    buttonsDialog = 0;
    imageDialog = 0;
    imagesList = 0;
    anyFile = true;
    imagesPath = "";
    rootPath = "";
    setAcceptMeansClose(false);
}

ImagesListDialog::~ImagesListDialog()
{
    delete ui;
    if(buttonsDialog)
        delete buttonsDialog;
}

bool ImagesListDialog::setUpDialog()
{
    bool ret = QewExtensibleDialog::setUpDialog();
    if(!forInputList)
        createImageDialog();
    return ret;
}

void ImagesListDialog::setSelectableImages(ReconstructionInputList *selectableImages)
{
    this->selectableImages = selectableImages;
}

void ImagesListDialog::fillUi()
{
    ui->titleLabel->setText(windowTitle());
    if(forInputList) {
        int r = ui->imagesTableWidget->rowCount();
        for(int i = r - 1; i >= 0; i--)
            ui->imagesTableWidget->removeRow(i);
        for(int i = 0; i < inputInfoImages->size(); i++)
            fillInputRow(i, inputInfoImages->at(i));
        ui->imagesTableWidget->sortByColumn(1, Qt::AscendingOrder);
        ui->imagesTableWidget->sortByColumn(0, Qt::AscendingOrder);
    }
    else {
        ui->pathLineEdit->setText(imagesPath);
        bool enableEditPath = true;
        for(int i = 0; i < imagesList->size(); i++)
            enableEditPath = enableEditPath && imagesList->at(i)->getEnableEdit();
        ui->pathLineEdit->setEnabled(enableEditPath);
        ui->pathToolButton->setEnabled(enableEditPath);

        int r = ui->imagesTableWidget->rowCount();
        for(int i = r - 1; i >= 0; i--)
            ui->imagesTableWidget->removeRow(i);
        for(int i = 0; i < imagesList->size(); i++)
            fillOutputRow(i, imagesList->at(i));
        ui->imagesTableWidget->sortByColumn(1, Qt::AscendingOrder);
    }
}

void ImagesListDialog::changeEvent(QEvent *e)
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

void ImagesListDialog::createImageDialog()
{
    QewDialogFactory factory;

    buttonsDialog = factory.createDialog(QewSimpleWidget);
    imageDialog = new ImageDialog(buttonsDialog);
    imageDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
    if(subjectsAsMainDirs)
        imageDialog->setParentPath(rootPath);
    imageDialog->setAcceptMeansClose(true);
    imageDialog->setUpDialog();
    imageDialog->hideTitleLabel();
    if(!individualPaths)
        imageDialog->hidePathSelection();
    imageDialog->showNameToolButton(individualPaths);
    imageDialog->removeTypes(getTypesToRemove());
    buttonsDialog->addExtensibleChild(imageDialog, imageDialogTitle);
    buttonsDialog->hide();
}

void ImagesListDialog::addInputInfos()
{
    if(!selectableImages)
        return;

    ChooseInputImagesDialog dialog;
    QList<ReconstructionInputInfo> list = selectableImages->getList(source);
    for(int i = list.size() - 1; i >= 0; i--) {
        if(inputInfoImages->contains(list.at(i)))
            list.removeAt(i);
    }

    dialog.setSelectableImages(list);
    int ret = dialog.exec();

    if(ret == QDialog::Rejected)
        return;
    QList<ReconstructionInputInfo> dialogList = dialog.getSelectedImages();
    if(dialogList.size() == 0)
        return;

    for(int i = 0; i < dialogList.size(); i++)
        inputInfoImages->append(dialogList.at(i));
    int cnt = ui->imagesTableWidget->rowCount();
    int row;
    for(int i = 0; i < dialogList.size(); i++) {
        row = cnt + i;
        fillInputRow(row, dialogList.at(i));
    }
    ui->imagesTableWidget->sortByColumn(1, Qt::AscendingOrder);
    ui->imagesTableWidget->sortByColumn(0, Qt::AscendingOrder);
    setDataChangedThis(true);
}

void ImagesListDialog::removeInputInfos()
{
    QList<QTableWidgetItem*> items = ui->imagesTableWidget->selectedItems();
    QList<int> rows = Utils::getRowsForItems(items);

    int cnt = rows.size();
    for(int i = cnt - 1; i >= 0; i--) {
        ReconstructionInputInfo inputInfo(ui->imagesTableWidget->item(rows.at(i), 0)->text(),
                           ui->imagesTableWidget->item(rows.at(i), 1)->text(),
                           ui->imagesTableWidget->item(rows.at(i), 2)->text(),
                           ui->imagesTableWidget->item(rows.at(i), 3)->text(),
                           ui->imagesTableWidget->item(rows.at(i), 4)->text());
        emit imageRemoved(inputInfo.getImageUuid(), inputInfo.getImageName());
        inputInfoImages->removeOne(inputInfo);
        ui->imagesTableWidget->removeRow(rows.at(i));
    }
    setDataChangedThis(true);
}

void ImagesListDialog::fillInputRow(int row, const ReconstructionInputInfo &inputInfo)
{
    ui->imagesTableWidget->insertRow(row);
    QString source = inputInfo.getSource();
    QString type = inputInfo.getImageType();
    QString name = inputInfo.getImageName();
    QString imageUuid = inputInfo.getImageUuid();
    QString reconsUuid = inputInfo.getReconstructionUuid();

    QTableWidgetItem* itemSource = new QTableWidgetItem(source);
    QTableWidgetItem* itemType = new QTableWidgetItem(type);
    QTableWidgetItem* itemName = new QTableWidgetItem(name);
    QTableWidgetItem* itemImageUuid = new QTableWidgetItem(imageUuid);
    QTableWidgetItem* itemReconsUuid = new QTableWidgetItem(reconsUuid);

    itemSource->setFlags(itemSource->flags() &  ~Qt::ItemIsEditable);
    itemType->setFlags(itemType->flags() &  ~Qt::ItemIsEditable);
    itemName->setFlags(itemName->flags() &  ~Qt::ItemIsEditable);

    ui->imagesTableWidget->setItem(row, 0, itemSource);
    ui->imagesTableWidget->setItem(row, 1, itemType);
    ui->imagesTableWidget->setItem(row, 2, itemName);
    ui->imagesTableWidget->setItem(row, 3, itemImageUuid);
    ui->imagesTableWidget->setItem(row, 4, itemReconsUuid);
}

void ImagesListDialog::addImage()
{
    if(ui->pathLineEdit->text().isEmpty() && !individualPaths) {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("Output images path is empty.\n Choose a path before"));
        msgBox.exec();
        return;
    }
    AmosImage* image = new AmosImage;
    if(!individualPaths) {
        if(subjectsAsMainDirs) {
            image->setPath(imagesPath);
            image->setParentPath(rootPath);            
        }
        else {
            image->setPath(imagesPath);
            image->setParentPath(imagesPath);
        }
    }
    imageDialog->setImage(image);
    imageDialog->setAnyFile(anyFile);
    int ret = buttonsDialog->exec();

    if(ret == QDialog::Accepted) {
        imagesList->append(image);

        int row = ui->imagesTableWidget->rowCount();
        fillOutputRow(row, image);

        ui->imagesTableWidget->sortByColumn(1, Qt::AscendingOrder);
        setDataChangedThis(true);
    }
    else {
        delete image;
        imageDialog->setImage(0);
    }
}

void ImagesListDialog::removeImages()
{
    QList<QTableWidgetItem*> items = ui->imagesTableWidget->selectedItems();
    QList<int> rows = Utils::getRowsForItems(items);

    int cnt = rows.size();
    for(int i = cnt - 1; i >= 0; i--) {
        for(int j = 0; j < imagesList->size(); j++) {
            if(imagesList->at(j)->getImageUuid().toString() ==
                    ui->imagesTableWidget->item(rows.at(i), 3)->text()) {
                emit imageRemoved(imagesList->at(j)->getImageUuid().toString(),
                                  imagesList->at(j)->getFileName());
                imagesList->removeAt(j);

                break;
            }
        }
        ui->imagesTableWidget->removeRow(rows.at(i));
    }
    setDataChangedThis(true);
}

void ImagesListDialog::cellSelected(int row, int /*col*/)
{
    QString uuid = ui->imagesTableWidget->item(row, 3)->text();
    AmosImage* image = 0;
    for(int i = 0; i < imagesList->size(); i++) {
        image = imagesList->at(i);
        if(image->getImageUuid().toString() == uuid)
            break;
    }
    if(image) {
        QString oldName = image->getFileName();
        imageDialog->setImage(image);
        imageDialog->setAnyFile(anyFile);
        int ret = buttonsDialog->exec();
        if(ret == QDialog::Accepted) {
            ui->imagesTableWidget->item(row, 1)->setText(image->getTypeName());
            ui->imagesTableWidget->item(row, 2)->setText(image->getFileName());

            ui->imagesTableWidget->sortByColumn(1, Qt::AscendingOrder);
            setDataChangedThis(true); // imageDialog->saveThis() put dataChanged=false
            emit imageNameChanged(image->getImageUuid().toString(), oldName);
        }
        imageDialog->setImage(0);
    }
}

void ImagesListDialog::fillOutputRow(int row, AmosImage *image)
{
    ui->imagesTableWidget->insertRow(row);
    QString type = image->getTypeName();
    QString name = image->getFileName();
    QString uuid = image->getImageUuid().toString();
    QTableWidgetItem* itemSource = new QTableWidgetItem(source->windowTitle());
    QTableWidgetItem* itemType = new QTableWidgetItem(type);
    QTableWidgetItem* itemName = new QTableWidgetItem(name);
    QTableWidgetItem* itemUuid = new QTableWidgetItem(uuid);

    itemType->setFlags(itemType->flags() &  ~Qt::ItemIsEditable);
    itemName->setFlags(itemName->flags() &  ~Qt::ItemIsEditable);

    ui->imagesTableWidget->setItem(row, 0, itemSource);
    ui->imagesTableWidget->setItem(row, 1, itemType);
    ui->imagesTableWidget->setItem(row, 2, itemName);
    ui->imagesTableWidget->setItem(row, 3, itemUuid);
}

void ImagesListDialog::choosePath()
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename;
    if(rootPath.isEmpty())
        filename = QDir::homePath();
    else
        filename = rootPath;
    if(!ui->pathLineEdit->text().trimmed().isEmpty())
        filename = ui->pathLineEdit->text().trimmed();
    QString sender = tr("Output images path");
    QString type_file = "";
    QString extensions = "";
//     QFileDialog::FileMode filemode = anyFile ? QFileDialog::AnyFile : QFileDialog::ExistingFile;
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
    if(imagesPath != newPath) {
//        if(!rootPath.isEmpty() && filename.trimmed().left(rootPath.size()) != rootPath) {
//            QMessageBox msgBox;
//            msgBox.setStandardButtons(QMessageBox::Ok);
//            msgBox.setText(filename.trimmed() + "\n" +
//                           tr("has to be a subdirectory of") + "\n" + rootPath + "\n" +
//                           tr("changing to previous."));
//            msgBox.exec();
//            ui->pathLineEdit->setText(imagesPath);
//            ui->pathLineEdit->setFocus();
//            return;
//        }
        
//         if(subjectsAsMainDirs) {
//             QFileInfo fileInfo(filename.trimmed());
//             imagesPath = fileInfo.fileName();
//         }
//         else
        imagesPath = newPath;
        for(int i = 0; i < imagesList->size(); i++) {
            imagesList->at(i)->setPath(ui->pathLineEdit->text().trimmed());
            if(subjectsAsMainDirs)
                imagesList->at(i)->setParentPath(rootPath);
            else
                imagesList->at(i)->setParentPath(ui->pathLineEdit->text().trimmed());
        }
        setDataChangedThis(true);
    }
//     if(subjectsAsMainDirs) {
//         QFileInfo fileInfo(filename.trimmed());
//         ui->pathLineEdit->setText(fileInfo.fileName());
//     }
//     else
    ui->pathLineEdit->setText(newPath);
}

void ImagesListDialog::setPath()
{
//     if(!rootPath.isEmpty() &&
//             ui->pathLineEdit->text().trimmed().left(rootPath.size()) != rootPath) {
    if(!anyFile) {
        if(!rootPath.isEmpty()) {
            QDir rootDir(rootPath);
            bool existsDir = false;
            if(subjectsAsMainDirs) {
                QFileInfo fileInfo;            
                QFileInfoList subdirs = rootDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
//                 subdirs.removeFirst(); // .
//                 subdirs.removeFirst(); // ..
                for(int i = 0; i < subdirs.size(); i++) {
                    fileInfo.setFile(subdirs.at(i).absoluteFilePath(), ui->pathLineEdit->text().trimmed());
                    existsDir = fileInfo.exists();
                    if(existsDir)
                        break;
                }          
            }
            else
                existsDir = rootDir.exists(ui->pathLineEdit->text().trimmed());
            if(!existsDir) {
                QMessageBox msgBox;
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setText(ui->pathLineEdit->text().trimmed() + "\n" +
                            tr("has to be a subdirectory of") + "\n" + rootPath + "\n" +
                            tr("changing to previous."));
                msgBox.exec();
                ui->pathLineEdit->setText(imagesPath);
                ui->pathLineEdit->setFocus();
                return;
            }
        }
        else {
            QFileInfo fileInfo(ui->pathLineEdit->text().trimmed());
            if(!fileInfo.exists()) {
                QMessageBox msgBox;
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setText(ui->pathLineEdit->text().trimmed() + "\n" +
                            tr("Does not exist, changing to previous."));
                msgBox.exec();
                ui->pathLineEdit->setText(imagesPath);
                ui->pathLineEdit->setFocus();
    //            changedOtherThanDescription = false;
                return;
            }
        }
    }
    if(imagesPath != ui->pathLineEdit->text().trimmed()) {
        imagesPath = ui->pathLineEdit->text().trimmed();
        for(int i = 0; i < imagesList->size(); i++) {
            imagesList->at(i)->setPath(ui->pathLineEdit->text().trimmed());
            if(subjectsAsMainDirs)
                imagesList->at(i)->setParentPath(rootPath);
            else
                imagesList->at(i)->setParentPath(ui->pathLineEdit->text().trimmed());
        }
        setDataChangedThis(true);
    }
}

void ImagesListDialog::setRootPath(const QString &rootPath)
{
    this->rootPath = rootPath;
    if(imageDialog)
        imageDialog->setParentPath(rootPath);
}

void ImagesListDialog::setAnyFile(bool anyFile)
{
    this->anyFile = anyFile;
}

//void ImagesListDialog::saveThis()
//{
//    imagesPath = ui->pathLineEdit->text().trimmed();
//}

void ImagesListDialog::setSource(QewExtensibleDialog *source, QString sourceUuid)
{
    this->source = source;
    this->sourceUuid = sourceUuid;
}

bool ImagesListDialog::dialogActivated(bool activated, QewDialogInactivate from,
                                       bool noSaveChildren)
{
    if(activated && imagesList) {
        askReconstructionState();
    }

    bool ret = QewExtensibleDialog::dialogActivated(activated, from, noSaveChildren);
    return ret;
}

void ImagesListDialog::askReconstructionState()
{
    for(int i = 0; i < imagesList->size(); i++) {
        AmosImage* image = imagesList->at(i);
        image->getBlockedReconstructionsId().clear();
        image->setCntReconstructionsState(0);
        if(image->getReconstructionsUsing().size() == 0) {
            image->setEnableEdit(true);
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
}

void ImagesListDialog::getExternVal(const QString &var_name, const QVariant &var_val)
{
    if(var_name == "returnReconstructionState") {
        if(!imagesList)
            return;
        for(int i = 0; i < imagesList->size(); i++)
            imagesList->at(i)->setEnableEdit(true);

        QStringList response = var_val.toStringList();
        if(response.size() != 3)
            return;
        QString imageUuid = response.at(2);

        int idx;
        for(idx = 0; idx < imagesList->size(); idx++) {
            if(imageUuid == imagesList->at(idx)->getImageUuid().toString())
                break;
        }
        if(idx >= imagesList->size())
            return;

        AmosImage* image = imagesList->at(idx);

        bool blocked = response.at(1) == "true" ? true : false;
        QString reconsUuid = response.at(0);
        int index = image->getReconstructionsUsing().indexOf(reconsUuid);
        if(index > -1) {
            image->setCntReconstructionsState(image->getCntReconstructionsState() + 1);
            if(blocked) {
                QString id = image->getReconstructionsUsingIds().at(index);
                if(!image->getBlockedReconstructionsId().contains(id)) {
                    image->getBlockedReconstructionsId().append(id);
                }
            }
        }
        if(image->getCntReconstructionsState() == image->getReconstructionsUsing().size()) {
            image->setEnableEdit(image->getBlockedReconstructionsId().size() == 0);
            if(!image->getEnableEdit()) {
                QStringList text = image->getBlockedReconstructionsId();
                text.prepend(tr("Image ") + image->getFileName() + tr(" is blocked by:"));
                emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::WarningBox), QVariant(text));
            }
        }
    }

}

QString ImagesListDialog::getImagesPath() const
{
    return imagesPath;
}

void ImagesListDialog::setImagesPath(const QString &imagesPath)
{
    this->imagesPath = imagesPath;
}

void ImagesListDialog::setImagesList(QList<AmosImage *> *imagesList)
{
    this->imagesList = imagesList;
}

void ImagesListDialog::setInputInfoImages(QList<ReconstructionInputInfo> *inputInfoImages)
{
    this->inputInfoImages = inputInfoImages;
}

QString ImagesListDialog::getImageDialogTitle() const
{
    return imageDialogTitle;
}

void ImagesListDialog::setImageDialogTitle(const QString &imageDialogTitle)
{
    this->imageDialogTitle = imageDialogTitle;
}

QList<AmosImage::ImageType> ImagesListDialog::getTypesToRemove() const
{
    return typesToRemove;
}

void ImagesListDialog::setTypesToRemove(const QList<AmosImage::ImageType> &typesToRemove)
{
    this->typesToRemove = typesToRemove;
}

bool ImagesListDialog::getSubjectsAsMainDirs()
{
    return subjectsAsMainDirs;
}

void ImagesListDialog::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
    if(imageDialog) {
        imageDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
        if(subjectsAsMainDirs)
            imageDialog->setParentPath(rootPath);
    }
}

void ImagesListDialog::delayedHideColumnsInput()
{
    ui->imagesTableWidget->setColumnHidden(ui->imagesTableWidget->columnCount() - 2, true);
    ui->imagesTableWidget->setColumnHidden(ui->imagesTableWidget->columnCount() - 1, true);        
}

void ImagesListDialog::delayedHideColumnsOutput()
{
    ui->imagesTableWidget->setColumnHidden(ui->imagesTableWidget->columnCount() - 1, true);
    ui->imagesTableWidget->setColumnHidden(0, true);       
}
