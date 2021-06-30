#ifndef IMAGESLISTDIALOG_H
#define IMAGESLISTDIALOG_H

#include <qewextensibledialog.h>
#include "reconstructioninputlist.h"
#include "amosimage.h"

class ImageDialog;
namespace Ui {
class ImagesListDialog;
}

class ImagesListDialog : public QewExtensibleDialog
{
    Q_OBJECT

public:
    explicit ImagesListDialog(QWidget *parent = 0, bool forInputList = true, bool individualPaths = false);
    ~ImagesListDialog();

    bool setUpDialog();
    void setSelectableImages(ReconstructionInputList* selectableImages);

    QList<AmosImage::ImageType> getTypesToRemove() const;
    void setTypesToRemove(const QList<AmosImage::ImageType> &typesToRemove);

    QString getImageDialogTitle() const;
    void setImageDialogTitle(const QString &imageDialogTitle);

    void setInputInfoImages(QList<ReconstructionInputInfo> *inputInfoImages);
    void setImagesList(QList<AmosImage *> *imagesList);

    QString getImagesPath() const;
    void setImagesPath(const QString &imagesPath);
    void fillUi();

    void setSource(QewExtensibleDialog *source, QString sourceUuid);

    bool dialogActivated(bool activated, QewDialogInactivate from = FromOutside,
                         bool noSaveChildren = false);
    void askReconstructionState();

    void setAnyFile(bool anyFile);

    void setRootPath(const QString &rootPath);
    
    bool getSubjectsAsMainDirs();
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);    

public slots:
    void getExternVal(const QString& var_name, const QVariant& var_val);

protected:
    void changeEvent(QEvent *e);
    void createImageDialog();
    void fillInputRow(int row, const ReconstructionInputInfo& inputInfo);
    void fillOutputRow(int row, AmosImage* image);

protected slots:
    void addInputInfos();
    void removeInputInfos();
    void addImage();
    void removeImages();
    void cellSelected(int row, int col);
    void choosePath();
    void setPath();
    void delayedHideColumnsInput();
    void delayedHideColumnsOutput();
//    void saveThis();

signals:
    void imageNameChanged(QString, QString);
    void imageRemoved(QString, QString);

protected:
    bool subjectsAsMainDirs;
    ReconstructionInputList* selectableImages;
    QList<ReconstructionInputInfo>* inputInfoImages;
    QList<AmosImage*>* imagesList;
    QString imagesPath;
//     QString fullImagesPath;
    QString rootPath;
    bool anyFile;
    bool forInputList;
    bool individualPaths;
    ImageDialog* imageDialog;
    QewExtensibleDialog* buttonsDialog;
    QList<AmosImage::ImageType> typesToRemove;
    QString imageDialogTitle;
    QewExtensibleDialog* source;
    QString sourceUuid;

private:
    Ui::ImagesListDialog *ui;
};

#endif // IMAGESLISTDIALOG_H
