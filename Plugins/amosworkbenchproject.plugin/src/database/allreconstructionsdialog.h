#ifndef ALLRECONSTRUCTIONSDIALOG_H
#define ALLRECONSTRUCTIONSDIALOG_H

#include <qewtreeviewdialog.h>
#include "rightclickmenu.h"
#include "reconstructiondialog.h"
#include "newreconstructiondialog.h"

class Reconstruction;
class ReconstructionInputList;

namespace Ui {
class AllReconstructionsDialog;
}

class AllReconstructionsDialog : public QewTreeViewDialog
{
    Q_OBJECT

public:
    explicit AllReconstructionsDialog(QWidget *parent = 0);
    ~AllReconstructionsDialog();
    void addExtensibleChild(QewExtensibleDialog* extensible_child, const QString& caption,
                            QTreeWidgetItem* parentItem = 0, int index = -1);
    bool setUpDialog();

    QList<ReconstructionDialog *> getDialogs() const;
    void removeDialogs();
    void setSelectableImages(ReconstructionInputList* selectableImages);

    void setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction*> &menuActions);
    QList<QAction*> setMenuActionTexts();

    void setChildMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction *> &childMenuActions);

    void setReconstructions(QList<Reconstruction *>* reconstructions);
    void askReconstructionState();

    void setRootOutputPath(const QString &rootOutputPath);
    
    void setMaximumThreads(int maximumThreads);
    
    bool getSubjectsAsMainDirs();
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);
    void setSubjectIds(const QStringList &subjectIds);
    QMap<QString, QPair<QString, QString> > getProgramIdAndInternals();

public slots:
    void getExternVal(const QString& var_name, const QVariant& var_val);
    void changeSubjectIds(const QStringList &subjectIds);
    void removeReconstructionDialog(ReconstructionDialog* dialog);

protected:
    void changeEvent(QEvent *e);
    void newReconstruction();
    virtual Reconstruction* createNewReconstruction(const QString& id, const QString& programID,
    bool internalExec, bool hasPrefixes);
    void addReconstruction(Reconstruction *reconstruction);
    virtual ReconstructionDialog* getDialog(); 
    virtual void setupNewReconstructionDialog(NewReconstructionDialog& newDialog);
    void currentItemTextNotChanged(const QString &text);
    virtual QString getProcessType();
    
    void readProgramsIdAndInternals();

signals:
    void sendIdChanged(QTreeWidgetItem*, QString);
    void sendSubChildRemoved(QTreeWidgetItem*);
    void sendPerformReconstruction(ReconstructionDialog* );

protected:
    QString rootOutputPath;
    QList<Reconstruction*>* reconstructions;
    QStringList subjectIds;
    QList<ReconstructionDialog*> dialogs;
    ReconstructionInputList* selectableImages;
    QMap<RightCLickMenu::ActionTypes, QAction*> menuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> childMenuActions;

    QList<AmosImage::ImageType> typesToRemove;
    
    
    QMap<QString, QPair<QString, QString> > programIdAndInternals;
    int maximumThreads;

    bool subjectsAsMainDirs;
//private:
    Ui::AllReconstructionsDialog *ui;
};

#endif // ALLRECONSTRUCTIONSDIALOG_H
