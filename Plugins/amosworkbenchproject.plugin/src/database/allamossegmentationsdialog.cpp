#include "allamossegmentationsdialog.h"
#include "ui_allreconstructionsdialog.h"
#include "segmentationdialog.h"
#include "reconstruction.h"

AllAmosSegmentationsDialog::AllAmosSegmentationsDialog(QWidget *parent) :
    AllReconstructionsDialog(parent)
{
    typesToRemove.clear();
    typesToRemove << AmosImage::FLAIR << AmosImage::T1 <<
             AmosImage::MASK << AmosImage::EXPERT <<
             AmosImage::LABEL << AmosImage::FLAIR_RECONS <<
             AmosImage::T1_RECONS <<AmosImage::LABEL_RECONS <<
             AmosImage::PROB_MAP;
}

AllAmosSegmentationsDialog::~AllAmosSegmentationsDialog()
{
    qDebug("Deleting AllAmosSegmentationsDialog");
}

ReconstructionDialog *AllAmosSegmentationsDialog::getDialog()
{
    SegmentationDialog* dialog = new SegmentationDialog(this);
    dialog->setSaveButtonShowsUpdate(true);
    return dialog;
}

Reconstruction *AllAmosSegmentationsDialog::createNewReconstruction(const QString& id, const QString& type, bool internalExec, bool hasPrefixes)
{
    Reconstruction* reconstruction = AllReconstructionsDialog::createNewReconstruction(id, type, internalExec, hasPrefixes);
    reconstruction->setProgramId("AmosSegmentation");
//     reconstruction->setInternalExec(true);
    return reconstruction;
}

QString AllAmosSegmentationsDialog::getProcessType()
{
    return "segmentation";
}

QList<QAction *> AllAmosSegmentationsDialog::setMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(menuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::Add) {
            it.value()->setText(tr("Add Amos segmentation"));
            it.value()->setToolTip(tr("Add a new Amos segmentation"));
        }
        actions.append(it.value());
    }
    return actions;
}

void AllAmosSegmentationsDialog::setupNewReconstructionDialog(NewReconstructionDialog& newDialog)
{ 
    AllReconstructionsDialog::setupNewReconstructionDialog(newDialog);
    newDialog.hideCustomType();
}
