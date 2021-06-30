/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision$ 
 
Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef QmitkAmosWorkbenchView_h
#define QmitkAmosWorkbenchView_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include <mitkNodePredicateOr.h>
#include <berryIShellListener.h>
#include <berryISaveablePart.h>

// AmosWorkbenchLib
// #include "amosprocessdirectory.h"
#include "imagefilters.hpp"

#include <QTime>
#include <QTimer>

#include "messagestouser.h"
#include "ui_QmitkAmosWorkbenchViewControls.h"

// class AmosProcessDirectory;
class InternalProcessDirectory;
class LogSender;
class ProjectDialog;
class Project;
class ReconstructionDialog;
class Reconstruction;
class ExternalProcessLauncher;

/*!
  \brief QmitkAmosWorkbenchView

  \warning  This application module is not yet documented. Use "svn blame/praise/annotate" and ask the author to provide basic documentation.

  \sa QmitkFunctionality
  \ingroup Functionalities
*/
class QmitkAmosWorkbenchView : public QmitkAbstractView, public berry::ISaveablePart/*berry::IShellListener*/
{  
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT
  
  public:  

    static const std::string VIEW_ID;
    
    enum DirTypes {SCANSDIR, MASKDIR, EXPERTDIR, OUTPUTDIR};
    enum ImageTypes {FLAIR, T1, LABEL, MASK, EXPERT};

    virtual ~QmitkAmosWorkbenchView();

    virtual void CreateQtPartControl(QWidget *parent);
    void NodeAdded( const mitk::DataNode* node);
    void NodeRemoved( const mitk::DataNode* node);
    
    void DoSave() {}
    void DoSaveAs() {}
    bool IsDirty() const {return true;}
    bool IsSaveAsAllowed() const {return true; }
    bool IsSaveOnCloseNeeded() const  {return true; }    
     
  protected slots:          
      
    void setMode3D(bool mode3D); 
    void setNumberOfThreads(int numThreads);
    void appendTextLog(QString log);
    void segmentationFinished();
    void getProgressRange(int numSlices);
    void getProgress(int progress);
    void getProgressType(QString progressType);
    void getCaseName(QString caseName);
    void getCaseProgressRange(int numCases);
    void getCaseProgress(int caseNum);    
    void showTime(int mSecs);    
    void setSegmentationResults(QList< QPair<Image::Pointer, QString> > segmentationResults);      
  
    /// \brief Called when the user clicks the GUI button
    void DoInternalProcess();
//     void DoImageProcessing();
//     void DoContrastProcessing();
//     void onThresholdSet(double threshold);

//     void runSegmentation();
    void stopSegmentation();
    void runInternalProcess(QStringList outputNames, QMap<QString, QString> parameters, QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn = QList< QPair<Utils::ImageTypes, Utils::AmosImageData> >(), QStringList externalFiles = QStringList(), QString externalMessage = "");
/*    void runContrastEnhancement();
    void stopContrastEnhancement();*/    
    void runWMHStatistics();
    void stopWMHStatistics();
    void calculateWMHStatistics(ReconstructionDialog* reconstructionDialog);
    
    void showFiltersTime(bool show);
    void showLogWindow(bool show);
    
//     void setNumThreads(int numThreads);
    void showTime(); 
        
    void OnFlairComboBoxSelectionChanged(int /*index*/);
    void OnT1ComboBoxSelectionChanged(int /*index*/);
    void OnLBComboBoxSelectionChanged(int /*index*/);
    void OnMaskComboBoxSelectionChanged(int /*index*/);
    void onMaskChecked(int state);
    
    void OnFlairEnhancedComboBoxSelectionChanged(int /*index*/);
    void OnLBEnhancedComboBoxSelectionChanged(int /*index*/);
    void OnMaskEnhancedComboBoxSelectionChanged(int /*index*/);
    void onMaskEnhancedChecked(int state);    
    
    void onMessageToUser(QString message);
    void onProjectLoaded();
    
    void performReconstruction(ReconstructionDialog* reconstructionDialog);
    
    void amosResampling(ReconstructionDialog* reconstructionDialog);    
    void runAmosResampling();
    void stopAmosResampling();
    void amosResamplingFinished();

    void freesurferLabelling(ReconstructionDialog* reconstructionDialog);    
    void runFreesurferLabelling();    
    void stopFreesurferLabelling();    
    void freesurferLabellingFinished();
    
    void setFullyProcessedSubjects(QStringList fullyProcessedSubjects);
    
    void enableParametersEdition(bool enable);
    
    void onSigmaSeedSet(double val);
    void onSigmaSeed2Set(double val);
    void onSigmaTH2Set(double val);
    void onSigmaTH4Set(double val);
    void onErodeWMSet(int val);
    void onWeightClassSet(double val);
    
    void enableParametersEditionEnhance(bool enable);
    
    void onGradientWinRadSet(int val);
    void onMinimumLengthSet(int val);
    void onNmxRankSet(double val);
    void onNmxConfSet(double val);
    void onNmxTypeSet(int val);	
    void onHystHighRankSet(double val);
    void onHystHighConfSet(double val);
    void onHystHighTypeSet(int val);
    void onHystLowRankSet(double val);
    void onHystLowConfSet(double val);
    void onHystLowTypeSet(int val);	
    void onCustCurveHystHigh_00Set(double val);
    void onCustCurveHystHigh_01Set(double val);
    void onCustCurveHystHigh_10Set(double val);
    void onCustCurveHystHigh_11Set(double val);    
    void onCustCurveHystLow_00Set(double val);
    void onCustCurveHystLow_01Set(double val);
    void onCustCurveHystLow_10Set(double val);
    void onCustCurveHystLow_11Set(double val);
    void onSaveEdgesMapSet(bool val);
    void onSaveConfidenceMapSet(bool val); 
    void onSaveGradientMapSet(bool val);
    
    void tabChanged(int index);
    
signals:
    void sendChronoInterval(uint);
    void sendTime(int mSecs);

protected:
    enum InternalProcessType {AMOS, CONTRAST};
    
    virtual void SetFocus();

    /// \brief called by QmitkAbstractView when DataManager's selection has changed
    virtual void OnSelectionChanged( berry::IWorkbenchPart::Pointer source,
                                     const QList<mitk::DataNode::Pointer>& nodes );
    virtual void OnSelectionChanged(mitk::DataNode* node);
    void displayTime(QString time)    ;
    bool validImages();
    bool validImagesEnhance();
    
    void DoImageProcessing();
    void DoContrastProcessing();
    void doBatchProcessing(ReconstructionDialog* reconstructionDialog);
    void doBatchContrastProcessing(ReconstructionDialog* reconstructionDialog);
    
    void extractExternalFiles();
    bool copyFromResources(QString fileName);

protected:
      
    InternalProcessType internalProcessType;
    QWidget *parent;
    Ui::QmitkAmosWorkbenchViewControls m_Controls;
    mitk::NodePredicateOr::Pointer m_IsOfTypeImagePredicate;
    bool advancedMode;
    bool mode3D;
//     double threshold;
    std::chrono::time_point<std::chrono::high_resolution_clock> beginLoadedCase;
    bool maskEnabled;
    bool maskEnhancedEnabled;
    QString initialLableMessage;
    
    bool modeAutomatic;
    
    QString outputParentPath;
    QString outputPath;
    bool dirSelectionOn;
    bool subjectsAsMainDirs;
    
    QString subjectName;
    QString flairName;
    QString t1Name;
    QString labelsName;
    QString maskName;
    QString annotName;
    QString ACName;
    QString BCName;
    QString EnhancedName;
    Utils::AmosImageData flairData;
    Utils::AmosImageData t1Data;
    Utils::AmosImageData labelsData;
    Utils::AmosImageData maskData;
    Utils::AmosImageData annotData;    
    bool imageSelectionOn;
    
    QString svmModelFileName;
    QString svmScaleFileName;
    
//     AmosProcessDirectory *amos;
    InternalProcessDirectory *amos;
    bool amosRunning;
    int numThreads;
    int numSubjects;
    
    LogSender *logSender;
    bool showLog;

    double NSIGMADESVseed;
    double NSIGMADESVseed2;
    double NSIGMADESV_TH2;
    double NSIGMADESV_TH4;
/*    int NBINS;
    int NPOINTSGAUSSIANAPROX; */   
    int ERODEWM;
//     int MINWMLSIZE;    
//     int EXCLUSIONZONE;
//     int MIN_R_PERIPHERY;
    double WEIGHT_CLASSIFIER_FL;
    
    int GradientWinRad;
    int MinimumLength;
    double NmxRank;
    double NmxConf;
    int NmxType; 
    QString SNmxType;
    double HystHighRank;
    double HystHighConf;
    int HystHighType;
    double CustCurveHystHigh_00;
    double CustCurveHystHigh_10;
    double CustCurveHystHigh_01;
    double CustCurveHystHigh_11;    
    double HystLowRank;
    double HystLowConf;
    int HystLowType; 
    double CustCurveHystLow_00;
    double CustCurveHystLow_10;
    double CustCurveHystLow_01;
    double CustCurveHystLow_11;
    bool saveEdgesMap;
    bool saveConfidenceMap;
    bool saveGradientMap;
    
    QTimer *timer;
    QTime proccessTime;
    int numMsecsClock;
    bool imagesPreviouslyLoaded;
    QList<Image::Pointer> imagesMITK;
    QList< QPair<Image::Pointer, QString> > segmentationResults;
    
    MessagesToUser badCasesBox;
    int numValidCases;
    ProjectDialog* projectDialog;
    Project* project;
    ReconstructionDialog* reconstructionDialog;
    Reconstruction* reconstruction;
    QHash<ReconstructionDialog*, bool> runButtonsState;
    
    bool segmentationStopped;
    
    ExternalProcessLauncher* launcher;
    QString slicer3DPath;
    bool doBiasCorrection;
    bool LBResampling;
    QString freesurferPath;
    QString currentPath;
    QStringList fullyProcessedSubjects;
    
    mitk::DataNode::Pointer flairContrastNode;
};


#endif // QmitkAmosWorkbenchView_h

