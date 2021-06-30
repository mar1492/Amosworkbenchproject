#ifndef ALLAMOSSEGMENTATIONSDIALOG_H
#define ALLAMOSSEGMENTATIONSDIALOG_H

#include "allreconstructionsdialog.h"

class ReconstructionInputList;
//class SegmentationDialog;

//namespace Ui {
//class AllReconstructionsDialog;
//}

class AllAmosSegmentationsDialog : public AllReconstructionsDialog
{
    Q_OBJECT

public:
    explicit AllAmosSegmentationsDialog(QWidget *parent = 0);
    ~AllAmosSegmentationsDialog();
    QList<QAction*> setMenuActionTexts();

protected:
    ReconstructionDialog* getDialog();
    Reconstruction* createNewReconstruction(const QString& id, const QString& programID,
    bool internalExec, bool hasPrefixes);
    void setupNewReconstructionDialog(NewReconstructionDialog& newDialog);
    QString getProcessType();
    //void readProgramsIdAndInternals() {}
};

#endif // ALLAMOSSEGMENTATIONSDIALOG_H
