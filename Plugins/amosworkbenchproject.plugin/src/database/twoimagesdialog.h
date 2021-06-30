#ifndef TWOIMAGESDIALOG_H
#define TWOIMAGESDIALOG_H

#include <qewsimpledialog.h>
#include "rightclickmenu.h"

class ImageDialog;
class AmosImage;
class ReconstructionInputList;

namespace Ui {
class TwoImagesDialog;
}

class TwoImagesDialog : public QewSimpleDialog
{
    Q_OBJECT

public:
    explicit TwoImagesDialog(QWidget *parent = 0);
    virtual ~TwoImagesDialog();

    void showPathWidget(bool show);
    bool setUpDialog();
    void setFirstImage(AmosImage *image);
    void setSecondImage(AmosImage *image);
    AmosImage *getFirstImage() const;
    AmosImage *getSecondImage() const;

    bool getDataChangedThis() const;

    void setInputList(ReconstructionInputList *list);
    bool dialogActivated(bool activated, QewDialogInactivate from = FromOutside,
                         bool noSaveChildren = false);
    void askReconstructionState();

    void setFirstTitle(const QString &firstTitle);
    void setSecondTitle(const QString &secondTitle);
    void setSaveActionText(const QString &saveActionText);
    void setSaveActionToolTip(const QString &saveActionToolTip);
    void setRestoreActionText(const QString &restoreActionText);
    void setRestoreActionToolTip(const QString &restoreActionToolTip);

    void setFirstPath(const QString &path);
    void setSecondPath(const QString &path);

    void setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction*> &menuActions);
    QList<QAction*> setMenuActionTexts();

    virtual void fillUi();
    QString getPath() const;

    void setDialogUuid(const QString &dialogUuid);
    QString getDialogUuid() const;

    void setAnyFile(bool anyFile);
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);
    void setRootPath(QString rootPath);
    void changeRootPath(QString rootPath);
    
    void  setOneValidSubjectId(QString oneValidSubjectId) const;

public slots:
    void getExternVal(const QString& var_name, const QVariant& var_val);

protected:
    void changeEvent(QEvent *e);
    void saveThis( void );
    bool validateThis( void );
    void cleanThis( void );
    void restoreThis();
    virtual void preDialogActivated(bool activated, QewDialogInactivate* from);
    virtual void postDialogActivated(bool response);

protected slots:
    void choosePath();
    void setPath();

protected:
    bool showPath;
    QString rootPath;
    bool subjectsAsMainDirs;
    ImageDialog* firstDialog;
    ImageDialog* secondDialog;
    bool anyFile;
    bool invalidFile;
    QString path;
//     QString pathWithSubject;
    QString firstTitle;
    QString secondTitle;
    QString saveActionText;
    QString saveActionToolTip;
    QString restoreActionText;
    QString restoreActionToolTip;
    AmosImage *firstImage;
    AmosImage *secondImage;
    ReconstructionInputList* inputList;
    QMap<RightCLickMenu::ActionTypes, QAction*> menuActions;
    Ui::TwoImagesDialog *ui;
    QString dialogUuid;
    bool changedOtherThanDescription;
};

#endif // TWOIMAGESDIALOG_H
