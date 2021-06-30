#include "chooseinputimagesdialog.h"
#include "ui_chooseinputimagesdialog.h"
#include "amosimage.h"
#include "utils.hpp"

ChooseInputImagesDialog::ChooseInputImagesDialog(QWidget *parent) :
    QewExtensibleDialog(parent),
    ui(new Ui::ChooseInputImagesDialog)
{
    ui->setupUi(this);
    ui->chooseTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->chooseTableWidget->setSortingEnabled(true);
    ui->chooseTableWidget->setColumnCount(ui->chooseTableWidget->columnCount() + 2);
    ui->chooseTableWidget->setColumnHidden(ui->chooseTableWidget->columnCount() - 2, true);
    ui->chooseTableWidget->setColumnHidden(ui->chooseTableWidget->columnCount() - 1, true);

    setAcceptMeansClose(true);
}

ChooseInputImagesDialog::~ChooseInputImagesDialog()
{
    delete ui;
    qDebug("Deleting ChooseInputImagesDialog");
}

void ChooseInputImagesDialog::setSelectableImages(QList<ReconstructionInputInfo> list)
{
    int row = 0;
    listInputInfo = list;

    for(int i = 0; i < list.size(); i++) {
        ui->chooseTableWidget->insertRow(row);
        QString title = list.at(i).getSource();
        QTableWidgetItem* itemSource = new QTableWidgetItem(title);

        QString type = list.at(i).getImageType();
        QTableWidgetItem* itemType = new QTableWidgetItem(type);

        QString name = list.at(i).getImageName();
        QTableWidgetItem* itemName = new QTableWidgetItem(name);

        QString imageUuid = list.at(i).getImageUuid();
        QTableWidgetItem* itemImageUuid = new QTableWidgetItem(imageUuid);

        QString reconsUuid = list.at(i).getReconstructionUuid();
        QTableWidgetItem* itemReconsUuid = new QTableWidgetItem(reconsUuid);

        bool enable = true;
        if(!list.at(i).isValid())
            enable = false;
        if(enable) {
            itemSource->setFlags(itemSource->flags() &  ~Qt::ItemIsEditable);
            itemType->setFlags(itemType->flags() &  ~Qt::ItemIsEditable);
            itemName->setFlags(itemName->flags() &  ~Qt::ItemIsEditable);
        }
        else {
            itemSource->setFlags(itemSource->flags() &  ~Qt::ItemIsEnabled);
            itemType->setFlags(itemType->flags() &  ~Qt::ItemIsEnabled);
            itemName->setFlags(itemName->flags() &  ~Qt::ItemIsEnabled);
        }

        ui->chooseTableWidget->setItem(row, 0, itemSource);
        ui->chooseTableWidget->setItem(row, 1, itemType);
        ui->chooseTableWidget->setItem(row, 2, itemName);
        ui->chooseTableWidget->setItem(row, 3, itemImageUuid);
        ui->chooseTableWidget->setItem(row, 4, itemReconsUuid);

        row++;
    }
    ui->chooseTableWidget->sortByColumn(1, Qt::AscendingOrder);
    ui->chooseTableWidget->sortByColumn(0, Qt::AscendingOrder);
}

QList<ReconstructionInputInfo> ChooseInputImagesDialog::getSelectedImages()
{
   QList<ReconstructionInputInfo> list;
   QList<QTableWidgetItem*> items = ui->chooseTableWidget->selectedItems();
   QList<int> rows = Utils::getRowsForItems(items);

   int cnt = rows.size();
   for(int i = 0; i < cnt; i++) {
       list.append(ReconstructionInputInfo(ui->chooseTableWidget->item(rows.at(i), 0)->text(),
                               ui->chooseTableWidget->item(rows.at(i), 1)->text(),
                               ui->chooseTableWidget->item(rows.at(i), 2)->text(),
                               ui->chooseTableWidget->item(rows.at(i), 3)->text(),
                               ui->chooseTableWidget->item(rows.at(i), 4)->text()));
   }
   return list;
}

void ChooseInputImagesDialog::hideSource()
{
    ui->chooseTableWidget->hideColumn(0);
}

void ChooseInputImagesDialog::changeEvent(QEvent *e)
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
