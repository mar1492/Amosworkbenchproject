#ifndef EXTERNIMAGESDIALOG_H
#define EXTERNIMAGESDIALOG_H

#include <qewsimpledialog.h>

#include "reconstructioninputlist.h"
#include "rightclickmenu.h"

class ImagesListDialog;
namespace Ui {
class ExternImagesDialog;
}

class ExternImagesDialog : public QewSimpleDialog
{
    Q_OBJECT

public:
    explicit ExternImagesDialog(QWidget *parent = 0);
    ~ExternImagesDialog();

    bool setUpDialog();
    void setSelectableImages(ReconstructionInputList *selectableImages);
    bool dialogActivated(bool activated, QewDialogInactivate from = FromOutside,
                         bool noSaveChildren = false);
    void askReconstructionState();

    void setExternImages(QList<AmosImage *> *originalExternImages);
    QList<QAction *> setMenuActionTexts();
    void setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction*> &menuActions);
    bool getDataChangedThis() const;

    void setDialogUuid(const QString &externUuid);
    QString getDialogUuid() const;
    void setAnyFile(bool anyFile);
    
    void setRootPath(const QString &rootPath);
    
    bool getSubjectsAsMainDirs();
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);    

public slots:
    void getExternVal(const QString &var_name, const QVariant &var_val);

protected:
    void changeEvent(QEvent *e);
    void fillUi();
    void saveThis();
    void cleanThis();
    void restoreThis();

protected:
    ImagesListDialog* listDialog;
    ReconstructionInputList* selectableImages;
    QList<AmosImage*> externImages;
    QList<AmosImage*>* originalExternImages;
    QMap<RightCLickMenu::ActionTypes, QAction*> menuActions;
    QString externUuid;
    bool subjectsAsMainDirs;
    QString rootPath;
    
private:
    Ui::ExternImagesDialog *ui;
};

#endif // EXTERNIMAGESDIALOG_H
