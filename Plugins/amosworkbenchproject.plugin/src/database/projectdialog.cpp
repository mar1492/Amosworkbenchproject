#include "projectdialog.h"
//#include "ui_projectdialog.h"
#include "newprojectdialog.h"
#include "project.h"
#include "subject.h"
#include "annotatedimagedialog.h"
#include "scansdialog.h"
#include "subjectsdialog.h"
#include "allreconstructionsdialog.h"
#include "reconstructiondialog.h"
#include "segmentationdialog.h"
#include "allamossegmentationsdialog.h"
#include "externimagesdialog.h"
#include "graphvizdialog.h"
#include "messagestouser.h"
#include "rightclickmenu.h"
#include "qvariantptr.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>

#include <berryPlatformUI.h>
#include <internal/berryWorkbench.h>
#include <berryIPreferences.h>
#include <berryIPreferencesService.h>

#include <iostream>
using namespace std;

ProjectDialog::ProjectDialog(QWidget *parent) :
    QewTreeViewDialog(parent), selectableImages(this)/*,
    ui(new Ui::ProjectDialog)*/
{
    setupUi(this);
    
    createMenu();

    setExpandedAtInit(true);
    setAcceptMeansClose(false);

    connect(this, SIGNAL(emitDialogSaved()), this, SLOT(onProjectNotSaved()));
    connect(descriptionTextEdit, SIGNAL(textChanged()),
            this, SLOT(onDescriptionModified()));
    connect(idLineEdit, SIGNAL(editingFinished()), this, SLOT(setId()));
    connect(projectDataPage, SIGNAL(emitActivated(bool, const QString&)),
            this, SLOT(projectDataActivated(bool, const QString&)));
    connect(outputPathLineEdit, SIGNAL(editingFinished()), this, SLOT(setOutputPath()));
    connect(outputPathToolButton, SIGNAL(clicked(bool)), this, SLOT(chooseOutputPath()));

    QewListView->clear();
/*    QTreeWidgetItem* item = new QTreeWidgetItem( QewListView, 0 );
    item->setText( 0, tr( "Project" ) ); */
    projectItem = new QTreeWidgetItem(QewListView, 0);
    projectItem->setText(0, tr( "Project"));
    
    setSaveButtonShowsUpdate(true);

    project = 0;
    settingProject = true;
    projectNotSaved = false;
    closing = false;
    
    berry::IPreferencesService* prefService = berry::Platform::GetPreferencesService();
    berry::IPreferences::Pointer projectPreferences = prefService->GetSystemPreferences()->Node("/amosworkbenchproject.views.amosworkbenchview.database");
    projectFilename = projectPreferences->Get("LastProjectOpenPath", "");      
//     projectFilename = "";
    outputPath = "";
    validOutputPath = false;

    prevCurrentWidget = 0;
    prevCurrentItem = 0;
    scansDialog = 0;
    subjectsDialog = 0;
    annotatedDialog = 0;
    reconstructionsDialog = 0;
    segmentationsDialog = 0;
    externImagesDialog = 0;
    graphVizDialog = 0;
    badCasesBox = new MessagesToUser(this);
    badCasesBox->setVisible(false);
    canShowContextMenu = true;
    maximumThreads = QThread::idealThreadCount();
    maximumThreads = maximumThreads > 0 ? maximumThreads : 1;
    subjectsAsMainDirs = false;

    htmlViewer = new HtmlViewer;
    htmlViewer->hide();
//    connect(htmlViewer, SIGNAL(save()), this, SLOT(saveHtml()));
    connect(htmlViewer, SIGNAL(reloadHtml()), this, SLOT(showHtmlView()));
}

ProjectDialog::~ProjectDialog()
{
//    delete ui;
    if(project) {
    
        berry::IPreferencesService* prefService = berry::Platform::GetPreferencesService();
        berry::IPreferences::Pointer projectPreferences = prefService->GetSystemPreferences()->Node("/amosworkbenchproject.views.amosworkbenchview.database");       
        projectPreferences->Put("LastProjectOpenPath", projectFilename);
        delete project;        
    }
    delete badCasesBox;
    delete htmlViewer;
    reconstructionsDialog->setSelectableImages(0);
    segmentationsDialog->setSelectableImages(0);
    qDebug("Deleting ProjectDialog");
}

void ProjectDialog::createMenu()
{
    menuBar = new QMenuBar();
    menuBarFrame->layout()->setMenuBar(menuBar);
    QMenu* settingsMenu = menuBar->addMenu(tr("Settings"));
    menuBar->addMenu(tr("Remote connections"));
    menuBar->addMenu(tr("Protocols"));
    
    QMenu* changeAPDmenu = settingsMenu->addMenu(tr("Change project settings"));
    changeCustomProgramIdAction = new QAction(tr("Change custom program Id"), this);
//     changeCustomProgramId->setShortcuts(QKeySequence::Change);
    changeCustomProgramIdAction->setStatusTip(tr("Change reconstruction custom program Id to a predefined one"));
    connect(changeCustomProgramIdAction, &QAction::triggered, this, &ProjectDialog::changeCustomProgramId);
    changeAPDmenu->addAction(changeCustomProgramIdAction);
}

bool ProjectDialog::setUpDialog()
{
    bool ret = QewTreeViewDialog::setUpDialog();
    if(!ret)
        return ret;
    connectDialog(this);

    QString title;
    if(ret) {
        berry::PlatformUI::GetWorkbench()->AddWorkbenchListener(this);
//         projectItem = getTreeView()->topLevelItem(0);
//         projectItem->setText(0, tr( "Project"));
        prevCurrentItem = projectItem;
        dataItem = projectItem->child(0);

        if(!scansDialog) {
            scansDialog = new ScansDialog(this);
            if(scansDialog->setUpDialog()) {
            }
            scansDialog->setSaveButtonShowsUpdate(true);
            scansDialog->setAnyFile(false);
            title = tr("Scans");
            scansDialog->setWindowTitle(title);
            scansDialog->setInputList(&selectableImages);
            connect(scansDialog, SIGNAL(emitDialogSaved()),this, SLOT(onProjectNotSaved()));
            connect(scansDialog, SIGNAL(generateSubjectsAgain()), this, SLOT(generateSubjectsAgain()));

            addExtensibleChild(scansDialog, title);
        }        
        scansItem = scansDialog->getItemTree();

        if(!subjectsDialog) {
            subjectsDialog = new SubjectsDialog(this);
            subjectsDialog->setSaveButtonShowsUpdate(true);
            subjectsDialog->setUpDialog();
            title = tr("Subjects");
            subjectsDialog->setWindowTitle(title);
            subjectsDialog->setScansDialog(scansDialog);

            connect(subjectsDialog, SIGNAL(emitDialogSaved()),
                    this, SLOT(onProjectNotSaved()));
            connect(subjectsDialog, SIGNAL(emitUpdateSubjects()),
                    this, SLOT(updateSubjects()));
            connect(subjectsDialog, SIGNAL(emitRestoreSubjects()),
                    this, SLOT(sendSubjectsMap()));
            addExtensibleChild(subjectsDialog, title);
        }

        subjectsItem = subjectsDialog->getItemTree();

        if(!annotatedDialog) {
            annotatedDialog = new AnnotatedImageDialog(this);
            annotatedDialog->setSaveButtonShowsUpdate(true);
            if(annotatedDialog->setUpDialog()) {
            }
            annotatedDialog->setAnyFile(false);
            title = tr("Annotated images");
            annotatedDialog->setWindowTitle(title);
            annotatedDialog->setInputList(&selectableImages);
            connect(annotatedDialog, SIGNAL(emitDialogSaved()), this, SLOT(onProjectNotSaved()));
            addExtensibleChild(annotatedDialog, title);
        }
        annotatedItem = annotatedDialog->getItemTree();

        if(!reconstructionsDialog) {
            reconstructionsDialog = new AllReconstructionsDialog(this);
            if(reconstructionsDialog->setUpDialog()) {
            }
            title = tr("Reconstructions");
            reconstructionsDialog->setWindowTitle(title);
            reconstructionsDialog->setSelectableImages(&selectableImages);
            reconstructionsDialog->setMaximumThreads(maximumThreads);

            addExtensibleChild(reconstructionsDialog, title);
            connect(reconstructionsDialog, SIGNAL(emitDialogSaved()), this,
                    SLOT(onProjectNotSaved()));
            connect(reconstructionsDialog, SIGNAL(sendIdChanged(QTreeWidgetItem*, QString)),
                    this, SLOT(changeCurrentItemId(QTreeWidgetItem*, QString)));
            connect(reconstructionsDialog, SIGNAL(emitActivated(const QString&)),
                    this, SLOT(childDialogActivated(const QString&)));
            connect(reconstructionsDialog, SIGNAL(sendSubChildRemoved(QTreeWidgetItem*)),
                    this, SLOT(onSubchildRemoved(QTreeWidgetItem*)));
            connect(reconstructionsDialog, SIGNAL(sendPerformReconstruction(ReconstructionDialog* )), this, SIGNAL(sendPerformReconstruction(ReconstructionDialog* )));

            reconstructionsItem = reconstructionsDialog->getItemTree();
        }

        if(!segmentationsDialog) {
            segmentationsDialog = new AllAmosSegmentationsDialog(this);
            if(segmentationsDialog->setUpDialog()) {
            }
            title = tr("Amos segmentations");
            segmentationsDialog->setWindowTitle(title);
            segmentationsDialog->setSelectableImages(&selectableImages);
            segmentationsDialog->setMaximumThreads(maximumThreads);
            
            addExtensibleChild(segmentationsDialog, title);
            connect(segmentationsDialog, SIGNAL(emitDialogSaved()), this,
                    SLOT(onProjectNotSaved()));
            connect(segmentationsDialog, SIGNAL(sendIdChanged(QTreeWidgetItem*, QString)),
                    this, SLOT(changeCurrentItemId(QTreeWidgetItem*, QString)));
            connect(segmentationsDialog, SIGNAL(emitActivated(const QString&)),
                    this, SLOT(childDialogActivated(const QString&)));
            connect(segmentationsDialog, SIGNAL(sendSubChildRemoved(QTreeWidgetItem*)),
                    this, SLOT(onSubchildRemoved(QTreeWidgetItem*)));
            connect(segmentationsDialog, SIGNAL(sendPerformReconstruction(ReconstructionDialog* )), this, SIGNAL(sendPerformReconstruction(ReconstructionDialog* )));            

            segmentationsItem = segmentationsDialog->getItemTree();
        }

        connect(subjectsDialog, SIGNAL(emitSubjectIds(const QStringList &)),
                reconstructionsDialog, SLOT(changeSubjectIds(const QStringList &)));
        connect(subjectsDialog, SIGNAL(emitSubjectIds(const QStringList &)),
                segmentationsDialog, SLOT(changeSubjectIds(const QStringList &)));

        if(!externImagesDialog) {
            externImagesDialog = new ExternImagesDialog(this);
            if(externImagesDialog->setUpDialog()) {
            }
            externImagesDialog->setSaveButtonShowsUpdate(true);
            externImagesDialog->setAnyFile(false);
            title = tr("External images");
            externImagesDialog->setWindowTitle(title);
            externImagesDialog->setSelectableImages(&selectableImages);

            addExtensibleChild(externImagesDialog, title);
            connect(externImagesDialog, SIGNAL(emitDialogSaved()), this,
                    SLOT(onProjectNotSaved()));
            connect(externImagesDialog, SIGNAL(emitActivated(const QString&)),
                    this, SLOT(childDialogActivated(const QString&)));
            externItem = externImagesDialog->getItemTree();
        }

        if(!graphVizDialog) {
            graphVizDialog = new GraphVizDialog(this);
            if(graphVizDialog->setUpDialog()) {
            }
            title = tr("Process graph");
            graphVizDialog->setWindowTitle(title);
            addExtensibleChild(graphVizDialog, title);
            connect(graphVizDialog, SIGNAL(emitActivated(const QString&)),
                    this, SLOT(childDialogActivated(const QString&)));
            graphItem = graphVizDialog->getItemTree();
        }

        prepareMenuActions();
    }

    return ret;
}

void ProjectDialog::enableTree(bool enable)
{
    for(int i = 0; i < projectItem->childCount(); i++)
        projectItem->child(i)->setHidden(!enable);
    projectDataPage->setVisible(enable);
    removeElementAction->setEnabled(enable);
    saveElementAction->setEnabled(enable);
    htmlAction->setEnabled(enable);
}

bool ProjectDialog::dialogActivated(bool activated,
                     QewExtensibleDialog::QewDialogInactivate from, bool noSaveChildren)
{
//    bool outputPathChanged = false;
    if(!activated && from != QewExtensibleDialog::FromRestore && getDataChangedThis()) {
        if(project->getOutputPath().isEmpty() ||
                project->getOutputPath() != outputPathLineEdit->text().trimmed()) {
            if(!isValidOutputPath())
                return false;
            if(project->getReconstructions()->size() > 0 ||
                    project->getSegmentations()->size() > 0) {
                QMessageBox msgBox;
                msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                msgBox.setText(tr("Output path has been changed,") + "\n" +
                               tr("all output image paths in reconstructions and segmentations") + "\n" +
                               tr("will be changed according to new path.")  + "\n" +
                               tr("Do you want to continue?"));
                int ret = msgBox.exec();
                if(ret == QMessageBox::Cancel)
                    return false;
//                outputPathChanged = true;
            }
            from = QewExtensibleDialog::FromSave;
        }
    }
    bool response = QewExtensibleDialog::dialogActivated(activated, from, noSaveChildren);
//    if(response && outputPathChanged) {
//        QStringList list;
//        list << project->getOutputPath() << outputPath;
//        emit emitExternVal("outputPathChanged", QVariant(list));
//    }
    return response;
}

void ProjectDialog::prepareMenuActions()
{
    getTreeView()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(getTreeView(), SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showActionsMenu(const QPoint &)));

    addElementAction = new QAction(tr("Add"), this);
    addElementAction->setToolTip(tr("Add"));
    QPair<RightCLickMenu::ActionTypes, QAction*> addPair(RightCLickMenu::Add, addElementAction);
    RightCLickMenu::addMenuAction(addPair.first, addPair.second);
    connect(addElementAction, SIGNAL(triggered()), this, SLOT(addElement()));

    removeElementAction = new QAction(tr("Remove"), this);
    removeElementAction->setToolTip(tr("Remove"));
    QPair<RightCLickMenu::ActionTypes, QAction*> removePair(RightCLickMenu::Remove, removeElementAction);
    RightCLickMenu::addMenuAction(removePair.first, removePair.second);
    connect(removeElementAction, SIGNAL(triggered()), this, SLOT(removeElement()));

    saveElementAction = new QAction(tr("Save"), this);
    saveElementAction->setToolTip(tr("Save"));
    QPair<RightCLickMenu::ActionTypes, QAction*> savePair(RightCLickMenu::Save, saveElementAction);
    RightCLickMenu::addMenuAction(savePair.first, savePair.second);
    connect(saveElementAction, SIGNAL(triggered()), this, SLOT(saveElement()));

    loadElementAction = new QAction(tr("Load"), this);
    loadElementAction->setToolTip(tr("Load"));
    QPair<RightCLickMenu::ActionTypes, QAction*> loadPair(RightCLickMenu::Load, loadElementAction);
    RightCLickMenu::addMenuAction(loadPair.first, loadPair.second);
    connect(loadElementAction, SIGNAL(triggered()), this, SLOT(loadProject()));

    newElementAction = new QAction(tr("New"), this);
    newElementAction->setToolTip(tr("New"));
    QPair<RightCLickMenu::ActionTypes, QAction*> newPair(RightCLickMenu::New, newElementAction);
    RightCLickMenu::addMenuAction(newPair.first, newPair.second);
    connect(newElementAction, SIGNAL(triggered()), this, SLOT(newProject()));

    restoreElementAction = new QAction(tr("Restore"), this);
    restoreElementAction->setToolTip(tr("Restore"));
    QPair<RightCLickMenu::ActionTypes, QAction*> restorePair(RightCLickMenu::Restore, restoreElementAction);
    RightCLickMenu::addMenuAction(restorePair.first, restorePair.second);
    connect(restoreElementAction, SIGNAL(triggered()), this, SLOT(restoreElement()));

    htmlAction = new QAction(tr("Html view"), this);
    htmlAction->setToolTip(tr("Html view"));
    QPair<RightCLickMenu::ActionTypes, QAction*> htmlPair(RightCLickMenu::Html, htmlAction);
    RightCLickMenu::addMenuAction(htmlPair.first, htmlPair.second);
    connect(htmlAction, SIGNAL(triggered()), this, SLOT(showHtmlView()));

    dataMenuActions.insert(savePair.first, savePair.second);
    dataMenuActions.insert(restorePair.first, restorePair.second);

    projectMenuActions.insert(newPair.first, newPair.second);
    projectMenuActions.insert(loadPair.first, loadPair.second);
    projectMenuActions.insert(savePair.first, savePair.second);
    projectMenuActions.insert(removePair.first, removePair.second);
    projectMenuActions.insert(htmlPair.first, htmlPair.second);

    scansMenuActions.insert(savePair.first, savePair.second);
    scansMenuActions.insert(restorePair.first, restorePair.second);
    scansDialog->setMenuActions(scansMenuActions);

    annotatedMenuActions.insert(savePair.first, savePair.second);
    annotatedMenuActions.insert(restorePair.first, restorePair.second);
    annotatedDialog->setMenuActions(annotatedMenuActions);

    subjectsMenuActions.insert(addPair.first, addPair.second);
    subjectsMenuActions.insert(removePair.first, removePair.second);
    subjectsMenuActions.insert(savePair.first, savePair.second);
    subjectsMenuActions.insert(restorePair.first, restorePair.second);
    subjectsDialog->setMenuActions(subjectsMenuActions);

    allReconstructionsMenuActions.insert(addPair.first, addPair.second);
    reconstructionsDialog->setMenuActions(allReconstructionsMenuActions);

    reconstructionMenuActions.insert(savePair.first, savePair.second);
    reconstructionMenuActions.insert(removePair.first, removePair.second);
    reconstructionMenuActions.insert(restorePair.first, restorePair.second);
    reconstructionsDialog->setChildMenuActions(reconstructionMenuActions);

    allSegmentationsMenuActions.insert(addPair.first, addPair.second);
    segmentationsDialog->setMenuActions(allSegmentationsMenuActions);

    segmentationMenuActions.insert(savePair.first, savePair.second);
    segmentationMenuActions.insert(removePair.first, removePair.second);
    segmentationMenuActions.insert(restorePair.first, restorePair.second);
    segmentationsDialog->setChildMenuActions(segmentationMenuActions);

    externImgMenuActions.insert(savePair.first, savePair.second);
    externImgMenuActions.insert(restorePair.first, restorePair.second);
    externImagesDialog->setMenuActions(externImgMenuActions);

}

bool ProjectDialog::canCloseProject()
{
    if(!projectNotSaved)
        return true;
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Closing current project"));
    msgBox.setText(tr("Current project has not been saved.\nDo you want to continue?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    int ret = msgBox.exec();
    if(ret == QMessageBox::Cancel)
        return false;
    else {
        projectNotSaved = false;
        return true;
    }
}

void ProjectDialog::closeEvent(QCloseEvent *event)
{

    closingFromEvent = true;
    if(closing) {
        event->accept();
        return;
    }
    closing = true;

    // To know is some QLineEdit is changed
    // and it is necessary to save it
    QWidget* w = QApplication::focusWidget();
    if(w)
        w->clearFocus();

    QewExtensibleDialog* dialog = extensibleParent(getCurrentWidget());
    if(dialog)
        dialog->dialogActivated(false);

    event->ignore();
    QTimer::singleShot(0, this, SLOT(delayedClose()));
}

void ProjectDialog::delayedClose()
{

    if(canCloseProject()) {
        clean();
        if(closingFromEvent)
            close();
        else
            berry::Workbench::GetInstance()->Close();      
    }
    else
        closing = false;
}

void ProjectDialog::releaseContextMenu()
{
    canShowContextMenu = true;
}

void ProjectDialog::delayedShowSubjects()
{
    setCurrentWidget(subjectsItem);
}

void ProjectDialog::onProjectNotSaved()
{
    projectNotSaved = true;
}

Project* ProjectDialog::getProject()
{
    return project;
}

void ProjectDialog::cleanProject()
{
    badCasesBox->clear();
    badCasesBox->hide();
    htmlViewer->hide();

    reconstructionsDialog->removeDialogs();
    for(int i = 0; i < reconstructionsItem->childCount(); i++) {
        bool alsoWidgets = true;
        removeItemFromTree(reconstructionsItem->child(0), alsoWidgets);
    }

    segmentationsDialog->removeDialogs();
    for(int i = 0; i < segmentationsItem->childCount(); i++) {
        bool alsoWidgets = true;
        removeItemFromTree(segmentationsItem->child(0), alsoWidgets);
    }

    graphVizDialog->dialogActivated(false);
    enableTree(false);
}

void ProjectDialog::onCleanProject()
{
    if(!canCloseProject())
        return;
    if(project) {
        delete project;
        project = 0;
    }
    
    cleanProject();
    projectItem->setText(0, tr( "Project"));
    prevCurrentItem = projectItem;
}

void ProjectDialog::setProject(Project *project)
{
    this->project = project;
    settingProject = true;

    cleanProject();

    subjectsAsMainDirs = project->getSubjectsAsMainDirs();
    reconstructionsDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
    segmentationsDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
    subjectsDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
    
    projectId = project->getId();
    outputPath = project->getOutputPath();
    QFileInfo fileInfo(outputPath);
    validOutputPath = fileInfo.exists() && fileInfo.isDir();
    if(validOutputPath) {
        reconstructionsDialog->setRootOutputPath(outputPath);
        segmentationsDialog->setRootOutputPath(outputPath); 
        if(subjectsAsMainDirs)
            subjectsDialog->setRootPath(outputPath);
    }
    if(projectId.isEmpty())
        projectId = tr("New project");
    projectDescription = project->getDescription();
    fillUi();

    setCurrentWidget(dataItem);

    selectableImages.clear(this);

    Scans* scans = project->getScans();
    if(!scans) {
        scans = new Scans();
        project->setScans(scans);
    }

    scansDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
//     if(subjectsAsMainDirs)
        scansDialog->setRootPath(project->getOutputPath());
    scansDialog->setScans(scans);
    scansDialog->setSubjectsGenerated(project->getAllSubjects().size() > 0);
    QString scansUuid = scans->getScansUuid();
    scansDialog->setDialogUuid(scansUuid);

    selectableImages.clear(this);
    selectableImages.addInputInfo(scansDialog->getScans()->getFlair(), scansDialog, scansUuid);
    selectableImages.addInputInfo(scansDialog->getScans()->getT1(), scansDialog, scansUuid);

    sendSubjectsMap();

    annotatedDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
//     if(subjectsAsMainDirs)
        annotatedDialog->setRootPath(project->getOutputPath());      
    annotatedDialog->setFirstImage(project->getExpertImage());
    annotatedDialog->setSecondImage(project->getMaskImage());

    QString annotatedUuid = project->getAnnotatedUuid();
    annotatedDialog->setDialogUuid(annotatedUuid);
    selectableImages.addInputInfo(project->getExpertImage(), annotatedDialog, annotatedUuid);
    selectableImages.addInputInfo(project->getMaskImage(), annotatedDialog, annotatedUuid);

    reconstructionsDialog->setReconstructions(project->getReconstructions());
    segmentationsDialog->setReconstructions(project->getSegmentations());
    QStringList subjectIds;
    QMapIterator<QString, Subject*> it(subjects);
    while(it.hasNext())
        subjectIds.append(it.next().key());
    reconstructionsDialog->setSubjectIds(subjectIds);
    segmentationsDialog->setSubjectIds(subjectIds);

    QString externUuid = project->getExternUuid();
    externImagesDialog->setDialogUuid(externUuid);
    for(int i = 0; i < project->getExternImages()->size(); i++)
       selectableImages.addInputInfo(project->getExternImages()->at(i),
                                     externImagesDialog, externUuid);
    externImagesDialog->setExternImages(project->getExternImages());
    externImagesDialog->setSubjectsAsMainDirs(subjectsAsMainDirs);
//     if(subjectsAsMainDirs)
        externImagesDialog->setRootPath(project->getOutputPath());    
    
    updateGraph();

    setDataChangedThis(false);
    projectNotSaved = false;
    
    enableTree(true);
    
    if(subjectsAsMainDirs) {
        projectTypeLabel->setText(tr("This project has subjects directories structure"));
        labelOutputPath->setText(tr("Subjects path"));
    }
    else {
        projectTypeLabel->setText(tr("This project has procedures directories structure"));
        labelOutputPath->setText(tr("Outputs path"));
    }     

    emit projectLoaded();
    
    QTimer::singleShot(0, this, SLOT(expandAllItems()));

}


void ProjectDialog::updateGraph()
{
    QString scansUuid = scansDialog->getDialogUuid();
    graphVizDialog->setScansNode(NodeGraph(scansUuid, scansDialog->windowTitle(), QColor(Qt::white)));
    AmosImage* image = scansDialog->getFirstImage();
    QStringList list = image->getReconstructionsUsing();
    for(int i = 0; i < list.size(); i++){
        graphVizDialog->addEdge(EdgeGraph(scansUuid, list.at(i), QColor(Qt::black)));
    }
    image = scansDialog->getSecondImage();
    list = image->getReconstructionsUsing();
    for(int i = 0; i < list.size(); i++){
        graphVizDialog->addEdge(EdgeGraph(scansUuid, list.at(i), QColor(Qt::black)));
    }

    QString annotatedUuid = annotatedDialog->getDialogUuid();
    graphVizDialog->setAnnotatedNode(NodeGraph(annotatedUuid, annotatedDialog->windowTitle(), QColor(Qt::white)));
    image = annotatedDialog->getFirstImage();
    list = image->getReconstructionsUsing();
    for(int i = 0; i < list.size(); i++){
        graphVizDialog->addEdge(EdgeGraph(annotatedUuid, list.at(i), QColor(Qt::black)));
    }
    image = annotatedDialog->getSecondImage();
    list = image->getReconstructionsUsing();
    for(int i = 0; i < list.size(); i++){
        graphVizDialog->addEdge(EdgeGraph(annotatedUuid, list.at(i), QColor(Qt::black)));
    }

    QString externUuid = externImagesDialog->getDialogUuid();
    graphVizDialog->setExternNode(NodeGraph(externUuid, externImagesDialog->windowTitle(), QColor(Qt::white)));
    for(int j = 0; j < project->getExternImages()->size(); j++) {
        image = project->getExternImages()->at(j);
        list = image->getReconstructionsUsing();
        for(int i = 0; i < list.size(); i++){
            graphVizDialog->addEdge(EdgeGraph(externUuid, list.at(i), QColor(Qt::black)));
        }
    }

    QList<Reconstruction*>* recons = project->getReconstructions();
    for(int i = 0; i < recons->size();i ++) {
        QString uuid = recons->at(i)->getReconstructionUuid().toString();
        if(recons->at(i)->getExecStatus() == Reconstruction::UNSET)
            graphVizDialog->addNode(NodeGraph(uuid,
                               recons->at(i)->getId(), QColor(Qt::white)));
        else {
            graphVizDialog->addNode(NodeGraph(uuid,
                               recons->at(i)->getId(), QColor(255, 0, 0, 80)));
        }
        QList<AmosImage *>images = recons->at(i)->getImagesOut();
        for(int j = 0; j < images.size(); j++) {
            image = images.at(j);
            list = image->getReconstructionsUsing();
            for(int k = 0; k < list.size(); k++){
                graphVizDialog->addEdge(EdgeGraph(uuid, list.at(k), QColor(Qt::black)));
            }
        }
    }

    QList<Reconstruction*>* segments = project->getSegmentations();
    for(int i = 0; i < segments->size();i ++) {
        QString uuid = segments->at(i)->getReconstructionUuid().toString();
        graphVizDialog->addNode(NodeGraph(uuid,
                                          segments->at(i)->getId(), QColor(Qt::white)));
        QList<AmosImage *>images = segments->at(i)->getImagesOut();
        for(int j = 0; j < images.size(); j++) {
            image = images.at(j);
            list = image->getReconstructionsUsing();
            for(int k = 0; k < list.size(); k++){
                graphVizDialog->addEdge(EdgeGraph(uuid, list.at(k), QColor(Qt::black)));
            }
        }
    }
}

void ProjectDialog::fillUi()
{
    idLineEdit->setText(projectId);
    descriptionTextEdit->setText(projectDescription);
    projectItem->setText(0, projectId);
    outputPathLineEdit->setText(outputPath);
    if(!validOutputPath) {
        reconstructionsItem->setDisabled(true);
        segmentationsItem->setDisabled(true);
    }
    else {
        reconstructionsItem->setDisabled(false);
        segmentationsItem->setDisabled(false);
    }
}


void ProjectDialog::changeEvent(QEvent *e)
{
    QewTreeViewDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void ProjectDialog::saveThis()
{
    qDebug("Saving ProjectDialog data");
    projectId = idLineEdit->text().trimmed();
    project->setId(projectId);
    projectDescription = descriptionTextEdit->toPlainText();
    project->setDescription(projectDescription);
    if(projectFilename.isEmpty()) {
        QFileInfo fileInfo(QDir::home(), projectId + ".amp");
        projectFilename = fileInfo.absoluteFilePath();
    }
    if(outputPath != outputPathLineEdit->text().trimmed()) {            
        outputPath = outputPathLineEdit->text().trimmed();
        reconstructionsDialog->setRootOutputPath(outputPath);
        segmentationsDialog->setRootOutputPath(outputPath); 
//         if(subjectsAsMainDirs) { 
            scansDialog->changeRootPath(outputPath);
            subjectsDialog->setRootPath(outputPath);
            annotatedDialog->changeRootPath(outputPath);
            externImagesDialog->setRootPath(outputPath);
            QList<AmosImage*>* externImages = project->getExternImages();
            for(int i = 0; i < externImages->size(); i++) {
                externImages->at(i)->setParentPath(outputPath);
                if(!subjectsAsMainDirs)
                    externImages->at(i)->setPath(outputPath);
            }
            externImagesDialog->setExternImages(externImages);
                
            
//         }
        project->setOutputPath(outputPath);
    }
}

bool ProjectDialog::validateThis()
{
    qDebug("Validating ProjectDialog");
    return true;
}

void ProjectDialog::cleanThis()
{
    qDebug("Cleaning ProjectDialog");
}

void ProjectDialog::restoreThis()
{
    qDebug("Restoring ProjectDialog");
    fillUi();
    projectItem->setText(0, idLineEdit->text().trimmed());
    setDataChangedThis(false);
}

QStringList ProjectDialog::designedCaptions( void )
{
  QStringList cap_list;
  cap_list << tr("Project data");
  return cap_list;
}

void ProjectDialog::setCurrentWidget(QTreeWidgetItem *itemToShow)
{

    if(itemToShow == prevCurrentItem)
        return;

    if(settingProject){
        settingProject = false;
        prevCurrentWidget = containerWidget();
        getTreeView()->setCurrentItem(itemToShow);
        prevCurrentItem = itemToShow;
        return;
    }


    canShowContextMenu = false;
    QTimer::singleShot(100, this, SLOT(releaseContextMenu()));

    if(prevCurrentWidget) {
        QewExtensibleDialog* oldDialog =
                QewExtensibleDialog::extensibleParent(prevCurrentWidget);
        bool dialogDataChanged = oldDialog->getDataChangedThis();
        if(!oldDialog->dialogActivated(false)) {
            if(dialogDataChanged) {
                canShowContextMenu = false;
                QTimer::singleShot(100, this, SLOT(releaseContextMenu()));
            }
            getTreeView()->setCurrentItem(prevCurrentItem);
            return;
        }
        else if(dialogDataChanged) {
            canShowContextMenu = false;
            QTimer::singleShot(100, this, SLOT(releaseContextMenu()));
        }
    }

    bool showChildWidget = false;
    QTreeWidgetItem *childItem = itemToShow;
    if(itemToShow->childCount() > 0) {        
        showChildWidget = true;
        if(!descendantWidgets.value(itemToShow->child(0))->inherits("QewExtensibleDialog"))
            childItem = itemToShow->child(0);
        else if(descendantWidgets.value(itemToShow)->inherits("QStackedWidget")) {
            QStackedWidget* sw = (QStackedWidget*)(descendantWidgets.value(itemToShow));
            QWidget* w = sw->currentWidget();
            if(w->inherits("QewExtensibleDialog")) {
                childItem = ((QewExtensibleDialog*)w)->getItemTree();
            }
        }
    }

    QWidget* newCurrent = 0;
    if(showChildWidget) {
        newCurrent = descendantWidgets.value(childItem);
    }
    else {
        newCurrent = descendantWidgets.value(itemToShow);
    }

    if(newCurrent) {
        if(showChildWidget) {
            QewTreeViewDialog::setCurrentWidget(childItem);
        }
        else {
            QewTreeViewDialog::setCurrentWidget(itemToShow);
        }
        if(newCurrent->inherits("QewExtensibleDialog")) {
            QewExtensibleDialog* newDialog = (QewExtensibleDialog*)newCurrent;
            newDialog->dialogActivated(true);
        }
    }

    prevCurrentWidget = newCurrent;

    getTreeView()->setCurrentItem(itemToShow);
    prevCurrentItem = itemToShow;
}

void ProjectDialog::currentItemTextNotChanged(const QString &text)
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void ProjectDialog::currentItemTextChanged(const QString &/*text*/)
{

}

void ProjectDialog::setDataChangedThis(bool changed)
{
    QewExtensibleDialog::setDataChangedThis(changed);
    if(changed)
        projectNotSaved = true;
}

QList<QAction *> ProjectDialog::setDataMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(dataMenuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::Save) {
            it.value()->setText(tr("Update project data"));
            it.value()->setToolTip(tr("Update project data information"));
        }
        if(it.key() == RightCLickMenu::Restore) {
            it.value()->setText(tr("Restore project data"));
            it.value()->setToolTip(tr("Restore project data information to previously saved"));
        }
        actions.append(it.value());
    }
    return actions;
}

QList<QAction *> ProjectDialog::setProjectMenuActionTexts()
{
    QList<QAction*> actions;
    QMapIterator<RightCLickMenu::ActionTypes, QAction *> it(projectMenuActions);
    while(it.hasNext()) {
        it.next();
        if(it.key() == RightCLickMenu::New) {
            it.value()->setText(tr("New project"));
            it.value()->setToolTip(tr("Create a new empty project"));
        }
        else if(it.key() == RightCLickMenu::Load) {
            it.value()->setText(tr("Load project"));
            it.value()->setToolTip(tr("Load project from disk"));
        }
        else if(it.key() == RightCLickMenu::Save) {
            it.value()->setText(tr("Save project"));
            it.value()->setToolTip(tr("Save project to disk"));
        }
        else if(it.key() == RightCLickMenu::Remove) {
            it.value()->setText(tr("Close project"));
            it.value()->setToolTip(tr("Close project"));
        }        
        actions.append(it.value());
    }
    return actions;
}

void ProjectDialog::changeCurrentItemId(QTreeWidgetItem* item, QString newId)
{
//    if(getTreeView()->currentItem()->text(0) == oldId)
//        changeCurrentItemText(newId);
    changeItemText(item, newId);
}

//void ProjectDialog::accept()
//{
//    QewExtensibleDialog::accept();
//    project->clearAllSubjects();
//    QMapIterator<QString, Subject*> it(subjects);
//    while(it.hasNext()) {
//        it.next();
//        project->addSubject(it.value());
//    }
//    saveProject();
//}


void ProjectDialog::saveProject()
{
    XMLOperator* xml_operator = XMLOperator::instance();

    updateSubjects();

    QString content = project->saveXML();
    QString sender = tr("Save project");
    QString type_file = "";
    QString extensions = "";
    bool change_name = true;
    xml_operator->saveDocument(projectFilename, content, type_file, extensions, sender,
                               change_name);
    projectNotSaved = false;
    QTimer::singleShot(0, this, SLOT(expandAllItems()));
}

void ProjectDialog::loadProject()
{
    if(!canCloseProject())
        return;

    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename = projectFilename;
    if(filename.isEmpty())
        filename = QDir::homePath();
    QString docName = Project::XMLDocName();
    QString sender = tr("Open project");
    QString type_file = "";
    QString extensions = "";
    bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen, QFileDialog::ExistingFile, type_file, extensions);
    if(!ret)
        return;
    
    QDomDocument doc = xml_operator->loadXML(filename, docName, sender);

//     QDomNode n = doc.firstChild();
//     if(n.isNull())
//         return;
//     QDomElement e_root = n.toElement();
    QDomElement e_root = doc.documentElement();
    if(e_root.isNull())
        return;
    if(e_root.tagName() != "Project")
        return;
    projectFilename = filename;
    if(project)
        delete project;
    project = new Project;
    project->setXMLElement(e_root);        

    setProject(project);
//     emit projectLoaded();
}

void ProjectDialog::expandAllItems()
{
    getTreeView()->expandAll();
}

void ProjectDialog::newProject()
{
    if(!canCloseProject())
        return;
    
    NewProjectDialog projectD;
    
    int result = projectD.exec();
    
    if(result == QDialog::Accepted) {
        if(project)
            delete project;
        project = new Project(projectD.getSubjectsAsMainDirs());
        projectFilename = "";
        setProject(project);
        setDataChangedThis(true);
    }
}

void ProjectDialog::getExternVal(const QString &var_name, const QVariant &var_val)
{
    if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::WarningBox))
        badCasesBox->writeMessage(var_val);

    QewExtensibleDialog* dialog = QVariantPtr<QewExtensibleDialog>::asPtr(var_val);
    if(this == dialog) {
        if(getTreeView()->currentItem() == projectItem) {
            if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Save))
                saveProject();
            if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Remove))
                onCleanProject();     
        }
        else if(getTreeView()->currentItem() ==
                projectItem->child(0)) {
            bool noSaveChildren = true;
            if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Save))
                dialogActivated(false, QewExtensibleDialog::FromSave, noSaveChildren);
            else if(var_name == RightCLickMenu::getMenuActionText(RightCLickMenu::Restore))
                dialogActivated(false, QewExtensibleDialog::FromRestore, noSaveChildren);
        }
    }
    if(var_name == "updateGraph") {
        updateGraph();
    }

}

void ProjectDialog::onSubchildRemoved(QTreeWidgetItem *itemToShow)
{
    prevCurrentWidget = 0;
    setCurrentWidget(itemToShow);
}

void ProjectDialog::updateSubjects()
{
    project->clearAllSubjects();
    QMapIterator<QString, Subject*> it(subjects);
    while(it.hasNext()) {
        it.next();
        project->addSubject(it.value());
    }
}

void ProjectDialog::generateSubjectsAgain()
{
    project->clearAllSubjects();
    sendSubjectsMap();
    bool askToUser = false;
    subjectsDialog->generateSubjects(askToUser);
    QTimer::singleShot(100, this, SLOT(delayedShowSubjects()));
}

void ProjectDialog::sendSubjectsMap()
{
    subjects.clear();
    QList<Subject*> lSubjects = project->getAllSubjects();
    for(int i = 0; i < lSubjects.size(); i++)
        subjects.insert(lSubjects.at(i)->getId(), lSubjects.at(i));
    subjectsDialog->setSubjects(&subjects);
}

void ProjectDialog::showActionsMenu(const QPoint &pos)
{
    if(!canShowContextMenu)
        return;
    QTreeWidgetItem *item = getTreeView()->itemAt(pos);

    QList<QAction*> actions;
    if(!item)
        return;    
    else if(item == dataItem)
        actions = this->setDataMenuActionTexts();
    else if(item == projectItem)
        actions = this->setProjectMenuActionTexts();
    else if(item == subjectsItem)
        actions = subjectsDialog->setMenuActionTexts();
    else if(item == scansItem)
        actions = scansDialog->setMenuActionTexts();
    else if(item == annotatedItem)
        actions = annotatedDialog->setMenuActionTexts();
    else if(item == reconstructionsItem)
        actions = reconstructionsDialog->setMenuActionTexts();
    else if(item == segmentationsItem)
        actions = segmentationsDialog->setMenuActionTexts();
    else if(item->parent() == reconstructionsItem) {
        QewExtensibleDialog* d = QewExtensibleDialog::extensibleParent(getCurrentWidget());
        if(d && d->inherits("ReconstructionDialog")) {
            ReconstructionDialog* reconstructionDialog = (ReconstructionDialog*)d;
            actions = reconstructionDialog->setMenuActionTexts();
        }
    }
    else if(item->parent() == segmentationsItem) {
        QewExtensibleDialog* d = QewExtensibleDialog::extensibleParent(getCurrentWidget());
        if(d && d->inherits("SegmentationDialog")) {
            SegmentationDialog* segmentationDialog = (SegmentationDialog*)d;
            actions = segmentationDialog->setMenuActionTexts();
        }
    }
    else if(item == externItem)
        actions = externImagesDialog->setMenuActionTexts();

    if(actions.size() > 0) {
        RightCLickMenu menu;
        menu.setActions(actions);
        menu.exec(getTreeView()->mapToGlobal(pos));
    }
}

void ProjectDialog::childDialogActivated(const QString &caption)
{
    QTreeWidgetItem* item = getTreeView()->currentItem();
    int index = childExists(caption, item);
    if(index > -1) {
        getTreeView()->setCurrentItem(item->child(index));
        prevCurrentItem = item->child(index);
    }
}

void ProjectDialog::setId()
{
    if(projectId != idLineEdit->text().trimmed()) {
        projectId = idLineEdit->text().trimmed();
        projectItem->setText(0, projectId);
        setDataChangedThis(true);
    }
}

void ProjectDialog::onDescriptionModified()
{
    setDataChangedThis(true);
}

void ProjectDialog::setOutputPath()
{
    disconnect(outputPathLineEdit, SIGNAL(editingFinished()), this, SLOT(setOutputPath()));
    if(outputPath != outputPathLineEdit->text().trimmed()) {
        validOutputPath = isValidOutputPath();
        if(validOutputPath) {
            reconstructionsItem->setDisabled(false);
            segmentationsItem->setDisabled(false);
            setDataChangedThis(true);
        }
        else {
            reconstructionsItem->setDisabled(true);
            segmentationsItem->setDisabled(true);
            outputPathLineEdit->setText(outputPath);
        }
    }
    connect(outputPathLineEdit, SIGNAL(editingFinished()), this, SLOT(setOutputPath()));
}

bool ProjectDialog::isValidOutputPath()
{
    QFileInfo fileInfo(outputPathLineEdit->text().trimmed());
    if(!fileInfo.exists()) {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        QString text = "";
        bool emptyPath = false;
        if(outputPathLineEdit->text().trimmed().isEmpty()) {
            text = tr("Empty path.")+ "\n";
            emptyPath = true;
        }
        else
            text = outputPathLineEdit->text().trimmed() + "\n" +
                        tr("does not exist.") + "\n";
        if(emptyPath) {
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Close);
            msgBox.setText(text +
                        tr("Without a valid output path you cannot add") + "\n" +
                        tr("reconstructions or segmentations.") + "\n" +
                        tr("You have to define a valid path.") + "\n" +
                        tr("Changing to previuos path"));
        }
        else {
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Close);
            msgBox.setText(text +
                        tr("Without a valid output path you cannot add") + "\n" +
                        tr("reconstructions or segmentations.") + "\n" +
                        tr("Do you want to create the output path?"));
        }
        int ret = msgBox.exec();
        
        if(ret == QMessageBox::Close) {
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setText(tr("Do you want to close project?"));
            ret = msgBox.exec();
            if(ret == QMessageBox::Ok)
                QTimer::singleShot(100, this, SLOT(onCleanProject()));
            return false;
        }        
        
        if(emptyPath)
            return false;

        if(ret == QMessageBox::Cancel) {
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setText(tr("Changing to previuos path"));            
            return false;
        }
        bool created = QDir::root().mkpath(fileInfo.absoluteFilePath());
        if(!created) {
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setText(tr("Cannot create") + "\n" +
                           fileInfo.absoluteFilePath() + "\n" +
                           tr("Changing to previuos path"));
            msgBox.exec();
            return false;
        }
    }
    return true;
}

void ProjectDialog::chooseOutputPath()
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString filename = QDir::homePath();
    if(!outputPathLineEdit->text().trimmed().isEmpty())
        filename = outputPathLineEdit->text().trimmed();
    QString sender = tr("Main output directory");
    QString type_file = "";
    QString extensions = "";
//     bool anyFile = true;
//    QFileDialog::FileMode filemode = anyFile ? QFileDialog::AnyFile : QFileDialog::ExistingFile;
    QFileDialog::FileMode filemode = QFileDialog::Directory;

    bool ret = xml_operator->getFileDialog(filename, sender, QFileDialog::AcceptOpen,
                                           filemode, type_file, extensions, QFileDialog::ShowDirsOnly);
    if(!ret)
        return;
    if(outputPath != filename.trimmed()) {
        setDataChangedThis(true);
    }
    outputPathLineEdit->setText(filename.trimmed());
    validOutputPath = isValidOutputPath();
    if(validOutputPath) {
        reconstructionsItem->setDisabled(false);
        segmentationsItem->setDisabled(false);
    }
    else {
        reconstructionsItem->setDisabled(true);
        segmentationsItem->setDisabled(true);
    }
}

void ProjectDialog::projectDataActivated(bool active, const QString &/*caption*/)
{
     if(active && getTreeView()->currentItem() != dataItem) {
         prevCurrentItem = dataItem;
         getTreeView()->setCurrentItem(dataItem);
     }
}

void ProjectDialog::saveElement()
{
    QWidget * widget = getCurrentWidget();
    if(widget) {
        QewExtensibleDialog * dialog = QewExtensibleDialog::extensibleParent(widget);
        if(dialog)
            emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::Save),
                               QVariantPtr<QewExtensibleDialog>::asQVariant(dialog));
    }
}

void ProjectDialog::addElement()
{
    QWidget * widget = getCurrentWidget();
    if(widget) {
        QewExtensibleDialog * dialog = QewExtensibleDialog::extensibleParent(widget);
        if(dialog)
            emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::Add),
                               QVariantPtr<QewExtensibleDialog>::asQVariant(dialog));
    }
}

void ProjectDialog::removeElement()
{
    QWidget * widget = getCurrentWidget();
    if(widget) {
        QewExtensibleDialog * dialog = QewExtensibleDialog::extensibleParent(widget);
        if(dialog) {
            emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::Remove),
                               QVariantPtr<QewExtensibleDialog>::asQVariant(dialog));
        }
    }
}

void ProjectDialog::restoreElement()
{
    QWidget * widget = getCurrentWidget();
    if(widget) {
        QewExtensibleDialog * dialog = QewExtensibleDialog::extensibleParent(widget);
        if(dialog)
            emit emitExternVal(RightCLickMenu::getMenuActionText(RightCLickMenu::Restore),
                               QVariantPtr<QewExtensibleDialog>::asQVariant(dialog));
    }
}

void ProjectDialog::showHtmlView()
{
    askReconstructionState();
    if(graphVizDialog)
        graphVizDialog->dialogActivated(true);
    QTimer::singleShot(200, this, SLOT(delayedShowHtmlView()));
}

void ProjectDialog::delayedShowHtmlView()
{
    if(project) {
        if(graphVizDialog)
            project->setSvgContents(graphVizDialog->toSvg());
        QFileInfo filename(QDir::currentPath(), "temp.html");
        QFile file(filename.absoluteFilePath());
        QTextStream ts;
        if(file.open(QIODevice::WriteOnly)) {
            ts.setDevice(&file);
            ts << project->generateHtml(1);
            file.close();
            htmlViewer->setHtml(filename.absoluteFilePath());
            htmlViewer->show();
            QTimer::singleShot(0, this, SLOT(expandAllItems()));
        }
        else {
            QMessageBox::critical(0, tr("Generating html"),
                tr("Couldn't generate html file"), tr("&Cancel"));
        }
        if(graphVizDialog)
            graphVizDialog->dialogActivated(false);
    }
}

//void ProjectDialog::saveHtml()
//{
//    askReconstructionState();
//    QTimer::singleShot(100, this, SLOT(delayedSaveHtml()));
//}

//void ProjectDialog::delayedSaveHtml()
//{
//    if(project)
//        project->saveHtml();
//}

void ProjectDialog::askReconstructionState()
{
    scansDialog->askReconstructionState();
    annotatedDialog->askReconstructionState();
    externImagesDialog->askReconstructionState();
    reconstructionsDialog->askReconstructionState();
    segmentationsDialog->askReconstructionState();

    save();
}

// void ProjectDialog::setMaximumThreads(int maximumThreads)
// {
//     this->maximumThreads = maximumThreads;
// }

bool ProjectDialog::PreShutdown(berry::IWorkbench*  /*workbench*/, bool  /*forced*/)
{
    closingFromEvent = false;
    if(closing) {
        //event->accept();
        return true;
    }
    closing = true;

    // To know is some QLineEdit is changed
    // and it is necessary to save it
    QWidget* w = QApplication::focusWidget();
    if(w)
        w->clearFocus();

    QewExtensibleDialog* dialog = extensibleParent(getCurrentWidget());
    if(dialog)
        dialog->dialogActivated(false);

    //event->ignore();
    QTimer::singleShot(0, this, SLOT(delayedClose()));
    return false;    
}

void ProjectDialog::conditionalEnableRun(bool enable)
{
    QList<ReconstructionDialog *> dialogsR = reconstructionsDialog->getDialogs();
    for(int i = 0; i < dialogsR.size(); i++)
        dialogsR.at(i)->conditionalEnableRun(enable);
    QList<ReconstructionDialog *> dialogsS = segmentationsDialog->getDialogs();
    for(int i = 0; i < dialogsS.size(); i++)
        dialogsS.at(i)->conditionalEnableRun(enable);    
}

void ProjectDialog::changeCustomProgramId()
{
    if(project == 0) {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("There is no open project"));
        msgBox.exec();

        return;      
    }
    
    if(prevCurrentItem == 0 || prevCurrentItem->parent() != reconstructionsItem) {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("Selected item is not a reconstruction"));
        msgBox.exec();

        return;       
    }
    
    QList<ReconstructionDialog *> dialogs = reconstructionsDialog->getDialogs();
    Reconstruction* recons = 0;
    ReconstructionDialog* dialog = 0;
    int i = 0;
    while(i < dialogs.size()) {
        dialog = dialogs.at(i);
        recons = dialog->getReconstruction();
        if(recons->getId() == prevCurrentItem->text(0))
            break;
        i++;
    }
    
    if(recons->getProgramId() != "Custom") {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("Selected reconstruction has not Custom program Id"));
        msgBox.exec();
        return;        
    }
    
    QMap<QString, QPair<QString, QString> > programsIdAndInternals = reconstructionsDialog->getProgramIdAndInternals();
    QStringList programsId;    
    QMapIterator<QString, QPair<QString, QString> > it(programsIdAndInternals);
    while (it.hasNext()) {
        it.next();
        programsId.append(it.key());
    }    
    
    bool ok;
    QString programID = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                         tr("Program ID:"), programsId, 0, false, &ok);
    if (ok && !programID.isEmpty()) {
        recons->setProgramId(programID); 
        if(programsIdAndInternals.value(programID).first.toLower() == "true")
            recons->setInternalExec(true);
        else if(programsIdAndInternals.value(programID).first.toLower() == "false")
            recons->setInternalExec(false);
        if(programsIdAndInternals.value(programID).second.toLower() == "true")
            recons->setHasPrefixes(true);
        else if(programsIdAndInternals.value(programID).second.toLower() == "false")
            recons->setHasPrefixes(false);
        
        dialog->programDataChangedFromMenu();
    }
}
