#ifndef PROJECTDIALOG_H
#define PROJECTDIALOG_H

#include <qewtreeviewdialog.h>
#include "ui_projectdialog.h"
#include "reconstructioninputlist.h"
#include "rightclickmenu.h"
#include "htmlviewer.h"

#include <QDir>
#include <QMenuBar>

#include <berryIWorkbenchListener.h>

class Project;
class Subject;
class Reconstruction;
class ReconstructionDialog;
//class ProjectTreePage;
class ScansDialog;
class SubjectsDialog;
class AnnotatedImageDialog;
class MessagesToUser;
class AllReconstructionsDialog;
class AllAmosSegmentationsDialog;
class ExternImagesDialog;
class AmosImage;
class GraphVizDialog;

//namespace Ui {
//    class ProjectDialog;
//}

class ProjectDialog : public QewTreeViewDialog, public berry::IWorkbenchListener, private Ui::ProjectDialog
{
    Q_OBJECT

public:
    explicit ProjectDialog(QWidget *parent = 0);
    ~ProjectDialog();

    void setProject(Project* project);
    Project* getProject();
    bool setUpDialog();

    bool dialogActivated(bool activated, QewDialogInactivate from = FromOutside,
                         bool noSaveChildren = false);
    
//     void setMaximumThreads(int maximumThreads);
    
    bool PreShutdown(berry::IWorkbench*  /*workbench*/, bool  /*forced*/);
    void enableTree(bool enable);
    void conditionalEnableRun(bool enable);

public slots:
//    void accept();
    void newProject();    
    void getExternVal(const QString& var_name, const QVariant& var_val);
    void onSubchildRemoved(QTreeWidgetItem* itemToShow);

protected slots:
    void setCurrentWidget(QTreeWidgetItem* itemToShow);
    void changeCurrentItemId(QTreeWidgetItem *item, QString newId);
    void saveProject();
    void loadProject();
    void expandAllItems();
    void onProjectNotSaved();
    void onCleanProject();

    void updateSubjects();
    void generateSubjectsAgain();
    void sendSubjectsMap();

    void showActionsMenu(const QPoint &pos);
    void childDialogActivated(const QString& caption);

    void setId();
    void onDescriptionModified();
    void setOutputPath();
    void chooseOutputPath();

    void projectDataActivated(bool active, const QString& caption);

    void saveElement();
    void addElement();
    void removeElement();
    void restoreElement();
    void showHtmlView();
    void delayedShowHtmlView();
//    void saveHtml();
//    void delayedSaveHtml();
    void delayedClose();

    void releaseContextMenu();
    void delayedShowSubjects();
    
    void changeCustomProgramId();
    
signals:
    void projectLoaded();
    void sendPerformReconstruction(ReconstructionDialog* );

protected:
    void createMenu();
    void changeEvent(QEvent *e);
    void saveThis( void );
    bool validateThis( void );
    void cleanThis( void );
    void restoreThis();
    void cleanProject();
    void fillUi();
    QStringList designedCaptions( void );
    void currentItemTextNotChanged(const QString &text);
    void currentItemTextChanged(const QString &text);
    void setDataChangedThis(bool changed);

    QList<QAction*> setDataMenuActionTexts();
    QList<QAction*> setProjectMenuActionTexts();

    void prepareMenuActions();
    bool canCloseProject();

    void closeEvent(QCloseEvent * event);

    void askReconstructionState();
    bool isValidOutputPath();

    void updateGraph();        

protected:
    QMap<QString, Subject*> subjects;
    Project *project;
    QString projectId;
    QString projectDescription;
    QString outputPath;
    bool subjectsAsMainDirs;
    bool validOutputPath;

    bool projectNotSaved;
    bool closing;

    ScansDialog* scansDialog;
    SubjectsDialog* subjectsDialog;
    AnnotatedImageDialog* annotatedDialog;
    AllReconstructionsDialog* reconstructionsDialog;
    AllAmosSegmentationsDialog* segmentationsDialog;
    ExternImagesDialog* externImagesDialog;
    GraphVizDialog* graphVizDialog;

    MessagesToUser* badCasesBox;
    HtmlViewer* htmlViewer;

    QString projectFilename;
    QTreeWidgetItem* projectItem;
    QTreeWidgetItem* subjectsItem;
    QTreeWidgetItem* scansItem;
    QTreeWidgetItem* annotatedItem;
    QTreeWidgetItem* dataItem;
    QTreeWidgetItem* reconstructionsItem;
    QTreeWidgetItem* segmentationsItem;
    QTreeWidgetItem* externItem;
    QTreeWidgetItem* graphItem;
    QTreeWidgetItem* prevCurrentItem;

    QWidget* prevCurrentWidget;
    bool settingProject;

    QAction* addElementAction;
    QAction* removeElementAction;
    QAction* saveElementAction;
    QAction* loadElementAction;
    QAction* newElementAction;
    QAction* restoreElementAction;
    QAction* htmlAction;

    QMap<RightCLickMenu::ActionTypes, QAction*> projectMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> dataMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> scansMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> annotatedMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> subjectsMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> allReconstructionsMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> reconstructionMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> allSegmentationsMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> segmentationMenuActions;
    QMap<RightCLickMenu::ActionTypes, QAction*> externImgMenuActions;

    bool canShowContextMenu;

    ReconstructionInputList selectableImages;

    QMenuBar* menuBar;
    QAction* changeCustomProgramIdAction;
    
    bool closingFromEvent;
    int maximumThreads;

//private:
//    Ui::ProjectDialog *ui;
};

#endif // PROJECTDIALOG_H
