#include "segmentationdialog.h"
#include "ui_reconstructiondialog.h"

SegmentationDialog::SegmentationDialog(QWidget* parent) : ReconstructionDialog(parent)
{
    ui->idLabel->setText(tr("Segmentation ID"));
//     ui->programLabel->setText(tr("Amos"));
//     ui->internalExecutionWidget->hide();
    ui->internalExecutionCheckBox->setChecked(true);
    ui->internalExecutionCheckBox->setEnabled(false);
    // TODO
//     ui->internalExecutionComboBox
    ui->programPathWidget->hide();
    ui->addParameterToolButton->setDisabled(true);
    ui->removeParameterToolButton->setDisabled(true);
    showPrefixParamColumn(false);
}

SegmentationDialog::~SegmentationDialog()
{
    qDebug("Deleting SegmentationDialog");
}

QString SegmentationDialog::getTitle() const
{
    QString title = tr("Segmentation output image");
    return title;
}

QList<AmosImage::ImageType> SegmentationDialog::getTypesToRemove() const
{
    QList<AmosImage::ImageType> types;
    types << AmosImage::FLAIR << AmosImage::T1 <<
             AmosImage::MASK << AmosImage::EXPERT <<
             AmosImage::LABEL << AmosImage::FLAIR_RECONS <<
             AmosImage::T1_RECONS <<AmosImage::LABEL_RECONS <<
             AmosImage::PROB_MAP;
    return types;
}

QList<QAction *> SegmentationDialog::setMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(menuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::Save) {
            it.value()->setText(tr("Update segmentation"));
            it.value()->setToolTip(tr("Update selected segmentation"));
        }
        else if(it.key() == RightCLickMenu::Remove) {
            it.value()->setText(tr("Remove segmentation"));
            it.value()->setToolTip(tr("Remove selected segmentation"));
        }
        else if(it.key() == RightCLickMenu::Restore) {
            it.value()->setText(tr("Restore segmentation"));
            it.value()->setToolTip(tr("Restore segmentation information\nto previously saved"));
        }
        actions.append(it.value());
    }
    return actions;
}
