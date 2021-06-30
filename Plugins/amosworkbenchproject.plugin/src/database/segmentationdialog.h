#ifndef SEGMENTATIONDIALOG_H
#define SEGMENTATIONDIALOG_H

#include "reconstructiondialog.h"

class SegmentationDialog : public ReconstructionDialog
{ 
    Q_OBJECT

public:
    explicit SegmentationDialog(QWidget *parent = 0);
    ~SegmentationDialog();

    QList<QAction*> setMenuActionTexts();

protected:
    QString getTitle() const;
    QList<AmosImage::ImageType> getTypesToRemove() const;  

};

#endif // SEGMENTATIONDIALOG_H
