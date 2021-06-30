/*=========================================================================
 * 
 * Program:   Medical Imaging & Interaction Toolkit
 * Language:  C++
 * Date:      $Date$
 * Version:   $Revision$ 
 * 
 * Copyright (c) German Cancer Research Center, Division of Medical and
 * Biological Informatics. All rights reserved.
 * See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the above copyright notices for more information.
 * 
 * =========================================================================*/

// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "QmitkAmosWorkbenchView.h"
#include "mitkPluginActivator.h"

#include <mitkNodePredicateDataType.h>

// Qt
#include <QFileDialog>
#include <QStringList>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QThread>

#include "../database/projectdialog.h"
#include "../database/project.h"
#include "../database/reconstructiondialog.h"
#include "../database/reconstruction.h"

// AmosWorkbenchLib
#include "amosprocessdirectory.h"
#include "amosprocesscase.h"

// ContrastEnhancement
#include "contrastprocessdirectory.h"
#include "contrastprocesscase.h"

// ExternalProcess
#include "resamplingslicerlauncher.h"
#include "labellingfreesurferlauncher.h"
#include <prompt/defs.h>

const std::string QmitkAmosWorkbenchView::VIEW_ID = "amosworkbenchproject.views.amosworkbenchview";

QmitkAmosWorkbenchView::~QmitkAmosWorkbenchView()
{
//     projectDialog->close();
//     projectDialog->deleteLater();
    delete projectDialog;
    delete logSender;
    delete timer;    
}

void QmitkAmosWorkbenchView::CreateQtPartControl( QWidget *parent )
{
    // create GUI widgets from the Qt Designer's .ui file
    this->parent = parent;
//     threshold = 1.0;    
    modeAutomatic = true;
    imagesPreviouslyLoaded = true;
    subjectsAsMainDirs = false;
    amos = 0;
    launcher = 0;
    numValidCases = 0;
    advancedMode = PluginActivator::getContext()->getProperty("AdvancedMode").toBool();
    extractExternalFiles();
    setMode3D(false);
    
/*    svmModelFileName = "svmtest.model";
    svmScaleFileName = "svmtest.scale";*/    
    
    m_Controls.setupUi( parent );
    if(!advancedMode)
        m_Controls.tabWidget->removeTab(1);
    
    projectDialog = new ProjectDialog(parent);
    if(projectDialog->setUpDialog()) {
        projectDialog->setWindowModality(Qt::WindowModal);
        if(!advancedMode)
            m_Controls.tabWidget->insertTab(1, projectDialog, "Database");
        else
            m_Controls.tabWidget->insertTab(2, projectDialog, "Database");
        connect(m_Controls.tabWidget, SIGNAL(currentChanged(int )), this, SLOT(tabChanged(int)));
        connect(projectDialog, SIGNAL(projectLoaded()), this, SLOT(onProjectLoaded()));
        connect(projectDialog, SIGNAL(sendPerformReconstruction(ReconstructionDialog* )), this, SLOT(performReconstruction(ReconstructionDialog* )));         
        projectDialog->enableTree(false);        
    }
    reconstructionDialog = 0;
    reconstruction = 0;
    
//     m_Controls.spinboxThreshold->setValue(threshold);
    
    logSender = new LogSender;
    Utils::setLogSender(logSender);
    ImageFilters::setLogSender(logSender);
//     showFiltersTime(true);
    
    timer = new QTimer(this);
    numMsecsClock = 1000;
    timer->setInterval(numMsecsClock);
    AmosProcessCase::EditableParameters defaultParameters = AmosProcessCase::getDefaultEditableParameters();
    
    NSIGMADESVseed = defaultParameters.NSIGMADESVseed;
    NSIGMADESVseed2 = defaultParameters.NSIGMADESVseed2;
    NSIGMADESV_TH2 = defaultParameters.NSIGMADESV_TH2;
    NSIGMADESV_TH4 = defaultParameters.NSIGMADESV_TH4;
    ERODEWM = defaultParameters.ERODEWM;
    WEIGHT_CLASSIFIER_FL = defaultParameters.WEIGHT_CLASSIFIER_FL;
/*    NBINS = 128;
    NPOINTSGAUSSIANAPROX = 29;        
    MINWMLSIZE = 0;
    MINWMLSIZE = 0;
    EXCLUSIONZONE = 2;
    MIN_R_PERIPHERY = 3;*/ 

    GradientWinRad = 5;
    MinimumLength = 5;
    NmxRank = 0.5;
    NmxConf = 0.5;
    NmxType = CURVE_PARAMS::CURVE_ARC; 
    HystHighRank = 0.9;
    HystHighConf = 0.9;
    HystHighType = CURVE_PARAMS::CURVE_BOX;
    CustCurveHystHigh_00 = 0.4;
    CustCurveHystHigh_01 = 0.7;
    CustCurveHystHigh_10 = 0.6;
    CustCurveHystHigh_11 = 0.3;    
    HystLowRank = 0.8;
    HystLowConf = 0.8;
    HystLowType = CURVE_PARAMS::CURVE_CUSTOM; 
    CustCurveHystLow_00 = 0.4;
    CustCurveHystLow_01 = 0.7;
    CustCurveHystLow_10 = 0.6;
    CustCurveHystLow_11 = 0.3;
    saveEdgesMap = false;
    saveConfidenceMap = false;
    saveGradientMap = false;
        
    numThreads = QThread::idealThreadCount();
//     numThreads = 2;
    numThreads = numThreads > 0 ? numThreads : 1;
    numSubjects = 1;
//     projectDialog->setMaximumThreads(numThreads);
    
    segmentationStopped = false;
    
    connect(m_Controls.flairImageSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(OnFlairComboBoxSelectionChanged(int)));
    connect(m_Controls.t1ImageSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(OnT1ComboBoxSelectionChanged(int)));
    connect(m_Controls.lblImageSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(OnLBComboBoxSelectionChanged(int)));
    connect(m_Controls.maskImageSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(OnMaskComboBoxSelectionChanged(int)));   
    connect(m_Controls.checkBoxEnableEdition, SIGNAL(toggled(bool)), this, SLOT(enableParametersEdition(bool)));
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    connect(this, SIGNAL(sendTime(int)), this, SLOT(showTime(int)));
    
    amosRunning = false;

//     connect( m_Controls.buttonPerformSegmentation, SIGNAL(clicked()), this, SLOT(DoImageProcessing()) );
    connect( m_Controls.buttonPerformSegmentation, SIGNAL(clicked()), this, SLOT(DoInternalProcess()) );
    
    connect( m_Controls.maskCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onMaskChecked(int)) );
    m_Controls.maskImageSelector->setEnabled(false);
    //connect( m_Controls.spinboxThreshold, SIGNAL(valueChanged(double)), this, SLOT(onThresholdSet(double)) );
    
    displayTime("00:00:00:000");
    
    m_Controls.spinBoxSeed->setValue(NSIGMADESVseed);    
    m_Controls.spinBoxSeed2->setValue(NSIGMADESVseed2);
    m_Controls.spinBoxTH2->setValue(NSIGMADESV_TH2);
    m_Controls.spinBoxTH4->setValue(NSIGMADESV_TH4);
    m_Controls.spinBoxErodeWM->setValue(ERODEWM);
    m_Controls.spinBoxWeightClass->setValue(WEIGHT_CLASSIFIER_FL);
    connect( m_Controls.spinBoxSeed, SIGNAL(valueChanged(double)), this, SLOT(onSigmaSeedSet(double)) );
    connect( m_Controls.spinBoxSeed2, SIGNAL(valueChanged(double)), this, SLOT(onSigmaSeed2Set(double)) );
    connect( m_Controls.spinBoxTH2, SIGNAL(valueChanged(double)), this, SLOT(onSigmaTH2Set(double)) );
    connect( m_Controls.spinBoxTH4, SIGNAL(valueChanged(double)), this, SLOT(onSigmaTH4Set(double)) );
    connect( m_Controls.spinBoxErodeWM, SIGNAL(valueChanged(int)), this, SLOT(onErodeWMSet(int)) );
    connect( m_Controls.spinBoxWeightClass, SIGNAL(valueChanged(double)), this, SLOT(onWeightClassSet(double)) );
    
    maskEnabled = false;
    maskEnhancedEnabled = false;
    m_Controls.maskImageSelector->setEnabled(maskEnabled);
    m_Controls.maskImageSelectorEnhance->setEnabled(maskEnhancedEnabled);
    
    initialLableMessage = m_Controls.labelWarning->text();
    m_Controls.buttonPerformSegmentation->setEnabled(false);
    
    connect(m_Controls.flairImageSelectorEnhance, SIGNAL(currentIndexChanged(int)), this, SLOT(OnFlairComboBoxSelectionChanged(int)));
    connect(m_Controls.lblImageSelectorEnhance, SIGNAL(currentIndexChanged(int)), this, SLOT(OnLBComboBoxSelectionChanged(int)));
    connect(m_Controls.maskImageSelectorEnhance, SIGNAL(currentIndexChanged(int)), this, SLOT(OnMaskComboBoxSelectionChanged(int))); 
    connect( m_Controls.maskCheckBoxEnhance, SIGNAL(stateChanged(int)), this, SLOT(onMaskEnhancedChecked(int)) );
    m_Controls.maskImageSelector->setEnabled(false);
    
    connect(m_Controls.checkBoxEnableEditionEnhance, SIGNAL(toggled(bool)), this, SLOT(enableParametersEditionEnhance(bool)));
    
    m_Controls.spinBoxGradientWRad->setValue(GradientWinRad);
    connect( m_Controls.spinBoxGradientWRad, SIGNAL(valueChanged(int)), this, SLOT(onGradientWinRadSet(int)) );
    m_Controls.spinBoxMinimumLength->setValue(MinimumLength);
    connect( m_Controls.spinBoxMinimumLength, SIGNAL(valueChanged(int)), this, SLOT(onMinimumLengthSet(int)) );
    m_Controls.spinBoxNmxRank->setValue(NmxRank);
    connect( m_Controls.spinBoxNmxRank, SIGNAL(valueChanged(double)), this, SLOT(onNmxRankSet(double)) );
    m_Controls.spinBoxNmxConf->setValue(NmxConf);
    connect( m_Controls.spinBoxNmxConf, SIGNAL(valueChanged(double)), this, SLOT(onNmxConfSet(double)) );    
    m_Controls.comboBoxNmxType->setCurrentIndex(NmxType);
    connect( m_Controls.comboBoxNmxType, SIGNAL(currentIndexChanged(int)), this, SLOT(onNmxTypeSet(int)) );
    m_Controls.spinBoxHysterisisHighRank->setValue(HystHighRank);
    connect( m_Controls.spinBoxHysterisisHighRank, SIGNAL(valueChanged(double)), this, SLOT(onHystHighRankSet(double)) );
    m_Controls.spinBoxHysterisisHighConf->setValue(HystHighConf);
    connect( m_Controls.spinBoxHysterisisHighConf, SIGNAL(valueChanged(double)), this, SLOT(onHystHighConfSet(double)) );
    
    m_Controls.comboBoxHysterisisHighType->setCurrentIndex(HystHighType);
    connect( m_Controls.comboBoxHysterisisHighType, SIGNAL(currentIndexChanged(int)), this, SLOT(onHystHighTypeSet(int)) );    
    m_Controls.spinBoxCustomCurveHystHigh00->setValue(CustCurveHystHigh_00);
    connect( m_Controls.spinBoxCustomCurveHystHigh00, SIGNAL(valueChanged(double)), this, SLOT(onCustCurveHystHigh_00Set(double)) );
    m_Controls.spinBoxCustomCurveHystHigh01->setValue(CustCurveHystHigh_01);
    connect( m_Controls.spinBoxCustomCurveHystHigh01, SIGNAL(valueChanged(double)), this, SLOT(onCustCurveHystHigh_01Set(double)) );
    m_Controls.spinBoxCustomCurveHystHigh10->setValue(CustCurveHystHigh_10);
    connect( m_Controls.spinBoxCustomCurveHystHigh10, SIGNAL(valueChanged(double)), this, SLOT(onCustCurveHystHigh_10Set(double)) );
    m_Controls.spinBoxCustomCurveHystHigh11->setValue(CustCurveHystHigh_11);
    connect( m_Controls.spinBoxCustomCurveHystHigh11, SIGNAL(valueChanged(double)), this, SLOT(onCustCurveHystHigh_11Set(double)) );    
    m_Controls.spinBoxHysterisisLowRank->setValue(HystLowRank);
    connect( m_Controls.spinBoxHysterisisLowRank, SIGNAL(valueChanged(double)), this, SLOT(onHystLowRankSet(double)) );
    m_Controls.spinBoxHysterisisLowConf->setValue(HystLowConf);
    connect( m_Controls.spinBoxHysterisisLowConf, SIGNAL(valueChanged(double)), this, SLOT(onHystLowConfSet(double)) );
    m_Controls.comboBoxHysterisisLowType->setCurrentIndex(HystLowType);
    connect( m_Controls.comboBoxHysterisisLowType, SIGNAL(currentIndexChanged(int)), this, SLOT(onHystLowTypeSet(int)) );    
    m_Controls.spinBoxCustomCurveHystLow00->setValue(CustCurveHystLow_00);
    connect( m_Controls.spinBoxCustomCurveHystLow00, SIGNAL(valueChanged(double)), this, SLOT(onCustCurveHystLow_00Set(double)) );
    m_Controls.spinBoxCustomCurveHystLow01->setValue(CustCurveHystLow_01);
    connect( m_Controls.spinBoxCustomCurveHystLow01, SIGNAL(valueChanged(double)), this, SLOT(onCustCurveHystLow_01Set(double)) );
    m_Controls.spinBoxCustomCurveHystLow10->setValue(CustCurveHystLow_10);
    connect( m_Controls.spinBoxCustomCurveHystLow10, SIGNAL(valueChanged(double)), this, SLOT(onCustCurveHystLow_10Set(double)) );
    m_Controls.spinBoxCustomCurveHystLow11->setValue(CustCurveHystLow_11);
    connect( m_Controls.spinBoxCustomCurveHystLow11, SIGNAL(valueChanged(double)), this, SLOT(onCustCurveHystLow_11Set(double)) );
    m_Controls.checkBoxEdgesMap->setChecked(saveEdgesMap);
    connect( m_Controls.checkBoxEdgesMap, SIGNAL(toggled(bool)), this, SLOT(onSaveEdgesMapSet(bool)) );
    m_Controls.checkBoxConfMap->setChecked(saveConfidenceMap);
    connect( m_Controls.checkBoxConfMap, SIGNAL(toggled(bool)), this, SLOT(onSaveConfidenceMapSet(bool)) );
    m_Controls.checkBoxGradMap->setChecked(saveGradientMap);
    connect( m_Controls.checkBoxGradMap, SIGNAL(toggled(bool)), this, SLOT(onSaveGradientMapSet(bool)) );
    
    badCasesBox.setVisible(false);  
    
    // advancedMode
    connect(m_Controls.checkBox3D, SIGNAL(toggled(bool)), this, SLOT(setMode3D(bool)));
    connect(m_Controls.checkBoxFiltersTime, SIGNAL(toggled(bool)), this, SLOT(showFiltersTime(bool)));
    connect(m_Controls.threadsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setNumberOfThreads(int)));
    m_Controls.threadsSpinBox->setMaximum(numThreads);
    m_Controls.threadsSpinBox->setValue(numThreads);    
        
    mitk::NodePredicateDataType::Pointer isDwi = mitk::NodePredicateDataType::New("DiffusionImage");
    mitk::NodePredicateDataType::Pointer isDti = mitk::NodePredicateDataType::New("TensorImage");
    mitk::NodePredicateDataType::Pointer isQbi = mitk::NodePredicateDataType::New("QBallImage");
    mitk::NodePredicateOr::Pointer isDiffusionImage = mitk::NodePredicateOr::New(isDwi, isDti);
    isDiffusionImage = mitk::NodePredicateOr::New(isDiffusionImage, isQbi);
    m_IsOfTypeImagePredicate = mitk::NodePredicateOr::New(isDiffusionImage, mitk::TNodePredicateDataType<mitk::Image>::New());  
    
    mitk::DataStorage::Pointer ds = GetDataStorage();
    mitk::DataStorage::SetOfObjects::ConstPointer images = GetDataStorage()->GetSubset(m_IsOfTypeImagePredicate);
    
    for(mitk::DataStorage::SetOfObjects::const_iterator iter = images->begin(); iter != images->end(); ++iter)
    {
        mitk::DataNode* node = *iter;
        NodeAdded(node);	    
    }    
}

void QmitkAmosWorkbenchView::setMode3D(bool mode3D)
{
    this->mode3D = mode3D;
    if(!mode3D) {
        svmModelFileName = "svmtest2D.model";
        svmScaleFileName = "svmtest2D.scale";
    }
    else {
        svmModelFileName = "svmtest3D.model";
        svmScaleFileName = "svmtest3D.scale";
    }
}

void QmitkAmosWorkbenchView::setNumberOfThreads(int numThreads)
{
    this->numThreads = numThreads;
}

void QmitkAmosWorkbenchView::SetFocus()
{
    if(m_Controls.buttonPerformSegmentation->isEnabled())
        m_Controls.buttonPerformSegmentation->setFocus();
}

void QmitkAmosWorkbenchView::OnSelectionChanged( berry::IWorkbenchPart::Pointer /*source*/,
                                                 const QList<mitk::DataNode::Pointer>& /*nodes*/ )
{ 
    // iterate all selected objects, adjust warning visibility
    //   foreach( mitk::DataNode::Pointer node, nodes )
    //   {
    //     if( node.IsNotNull() && dynamic_cast<mitk::Image*>(node->GetData()) )
    //     {
    // 
    //     }
    //   }
    
}

void QmitkAmosWorkbenchView::DoInternalProcess()
{
//     if(m_Controls.tabWidget->currentIndex() == 0) {
    if(m_Controls.tabWidget->tabText(m_Controls.tabWidget->currentIndex()) == "Loaded case") {
        internalProcessType = AMOS;
        DoImageProcessing();
    }
//     else if(m_Controls.tabWidget->currentIndex() == 3) {
    else if(m_Controls.tabWidget->tabText(m_Controls.tabWidget->currentIndex()) == "Contrast enhancement") {
        internalProcessType = CONTRAST;
        DoContrastProcessing();
    }
}

void QmitkAmosWorkbenchView::DoImageProcessing()
{
    if(amosRunning) {
        stopSegmentation();
        return;
    }
    
    imagesPreviouslyLoaded = true;
    numValidCases = 1;
    modeAutomatic = true;    
    subjectsAsMainDirs = false;
    mitk::DataNode::Pointer flairNode = m_Controls.flairImageSelector->GetSelectedNode();
    mitk::DataNode::Pointer t1Node = m_Controls.t1ImageSelector->GetSelectedNode();
    mitk::DataNode::Pointer lblNode = m_Controls.lblImageSelector->GetSelectedNode();
    mitk::DataNode::Pointer maskNode = m_Controls.maskImageSelector->GetSelectedNode();
        
    imagesMITK.clear();
    imagesMITK.append(dynamic_cast<mitk::Image*>(flairNode->GetData()));
    imagesMITK.append(dynamic_cast<mitk::Image*>(t1Node->GetData()));
    imagesMITK.append(dynamic_cast<mitk::Image*>(lblNode->GetData()));
    if(maskEnabled)
        imagesMITK.append(dynamic_cast<mitk::Image*>(maskNode->GetData()));
    else
        imagesMITK.append(0);
    
    ACName = "Amos_AC";
    BCName = "Amos_BC";
    
    QMap<QString, QString> parameters;
    parameters["NSIGMADESVseed"] = QString::number(NSIGMADESVseed);
    parameters["NSIGMADESVseed2"] = QString::number(NSIGMADESVseed2);
    parameters["NSIGMADESV_TH2"] = QString::number(NSIGMADESV_TH2);
    parameters["NSIGMADESV_TH4"] = QString::number(NSIGMADESV_TH4);
    parameters["ERODEWM"] = QString::number(ERODEWM);
    parameters["WEIGHT_CLASSIFIER_FL"] = QString::number(WEIGHT_CLASSIFIER_FL);
    
    QStringList outputNames;
    outputNames << ACName << BCName; 
    
    QStringList externalFiles;
    externalFiles << svmModelFileName << svmScaleFileName;

    QString externalMessage = tr("Missing SVM files");    

    amos = new AmosProcessDirectory();    
//     runSegmentation();
    runInternalProcess(outputNames, parameters, QList<QPair<Utils::ImageTypes, Utils::AmosImageData> >(), externalFiles);
}

/*
void QmitkAmosWorkbenchView::doBatchProcessing(ReconstructionDialog* reconstructionDialog)
{
    this->reconstructionDialog = reconstructionDialog;
    imagesPreviouslyLoaded = false;

    if(amosRunning) {
        stopSegmentation();
        if(segmentationStopped)
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }
    subjectsAsMainDirs = project->getSubjectsAsMainDirs();
    reconstruction = reconstructionDialog->getReconstruction();
    
    QMap<QString, Reconstruction::Parameter> parameters = reconstruction->getParameters();
    // In app version for final users the parameter "automatic" is suppressed from
    // processdata.apd in this case modeAutomatic == true always, they cannot train Amos
    if(parameters.contains("automatic"))
        modeAutomatic = parameters["automatic"].value == "true" ? true : false;
    NSIGMADESVseed = parameters["SIGMADESVseed"].value.toDouble();
    NSIGMADESVseed2 = parameters["SIGMADESVseed2"].value.toDouble();
    NSIGMADESV_TH2 = parameters["SIGMADESV_TH2"].value.toDouble();
    NSIGMADESV_TH4 = parameters["SIGMADESV_TH4"].value.toDouble();
    ERODEWM = parameters["ERODEWM"].value.toInt();
    WEIGHT_CLASSIFIER_FL = parameters["WEIGHT_CLASSIFIER_FL"].value.toDouble();
    
    QList<ReconstructionInputInfo> inputInfoImages = reconstruction->getInputInfoImages();
    AmosImage* flairAmosImage = 0;
    AmosImage* T1AmosImage = 0;
    AmosImage* maskAmosImage = 0;
    AmosImage* labelAmosImage = 0;
    AmosImage* expertAmosImage = 0;
    for(int i = 0; i < inputInfoImages.size(); i++) {
        ReconstructionInputInfo inputInfo = inputInfoImages.at(i);
        if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::EXPERT) {
            expertAmosImage = project->getExpertImage();
        }
        else if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::MASK) {
            maskAmosImage = project->getMaskImage();
        }
        else if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::FLAIR_RECONS ||
                    AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::T1_RECONS ||
               AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::LABEL_RECONS) {        
            QList<Reconstruction*>* reconsList = project->getReconstructions();
            Reconstruction* recon = 0;
            AmosImage* image = 0;
            bool found = false;
            if(inputInfo.getReconstructionUuid() == project->getExternUuid()) {
                QList<AmosImage *>* externImages = project->getExternImages();
                for(int k = 0; k < externImages->size(); k++) {
                    image = externImages->at(k);
                    if(image->getImageUuid() == inputInfo.getImageUuid()) {
                        found = true;
                        break;
                    }
                }
            }
            if(!found) {
                for(int j = 0; j < reconsList->size(); j++) {
                    recon = reconsList->at(j);
                    if(recon == reconstruction)
                        continue;
                    if(recon->getReconstructionUuid() != inputInfo.getReconstructionUuid())
                        continue;
                    QList<AmosImage *> imagesOut = recon->getImagesOut();
                    for(int k = 0; k < imagesOut.size(); k++) {
                        image = imagesOut.at(k); 
                        if(image->getImageUuid() == inputInfo.getImageUuid()) {
                            found = true;
                            break;
                        }
                  
                    }
                }                
            }
            if(found) {
                if(image->getType() == AmosImage::FLAIR_RECONS)
                    flairAmosImage = image;
                else if(image->getType() == AmosImage::T1_RECONS)
                    T1AmosImage = image;
                else if(image->getType() == AmosImage::LABEL_RECONS)
                    labelAmosImage = image;                 
            }
        }        
     
    }
    
    if(!flairAmosImage || !T1AmosImage || !labelAmosImage || (!modeAutomatic && !expertAmosImage)) {
        QString title(tr("Images"));
        QString text(tr("Missing images."));
        QMessageBox::warning(0, title, text);
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }

    outputParentPath = project->getOutputPath();
    outputPath = reconstruction->getOutputPath();
    if(outputParentPath.endsWith(QDir::separator()))
        outputPath.remove(outputParentPath);
    else
        outputPath.remove(outputParentPath + QDir::separator());
    
    flairData.parentPath = flairAmosImage->getParentPath();
    if(subjectsAsMainDirs)
        flairData.path = flairAmosImage->getPath();
    flairData.name = flairAmosImage->getFileName();

    t1Data.parentPath = T1AmosImage->getParentPath();
    if(subjectsAsMainDirs)
        t1Data.path = T1AmosImage->getPath();
//     else
//         t1Data.path = "";
    t1Data.name = T1AmosImage->getFileName();
    
    labelsData.parentPath = labelAmosImage->getParentPath();
    if(subjectsAsMainDirs)
        labelsData.path = labelAmosImage->getPath();
    labelsData.name = labelAmosImage->getFileName();
    
    if(maskAmosImage) {
        maskData.parentPath = maskAmosImage->getParentPath();
        if(subjectsAsMainDirs)
            maskData.path = maskAmosImage->getPath();
        maskData.name = maskAmosImage->getFileName();        
    }    
    
    if(!modeAutomatic) {
        annotData.parentPath = expertAmosImage->getParentPath();
        if(subjectsAsMainDirs)
            annotData.path = expertAmosImage->getPath();
        annotData.name = expertAmosImage->getFileName();        
    }  

    QList<AmosImage *>& outputImages = reconstruction->getImagesOut();
    for(int i = 0; i < outputImages.size(); i++) {
        if(outputImages.at(i)->getType() == AmosImage::AMOS_AC) 
            ACName = outputImages.at(i)->getFileName();
        else if(outputImages.at(i)->getType() == AmosImage::AMOS_BC) 
            BCName = outputImages.at(i)->getFileName();
    }
    
    runSegmentation();    
}
*/

void QmitkAmosWorkbenchView::doBatchProcessing(ReconstructionDialog* reconstructionDialog)
{
    internalProcessType = AMOS;
    this->reconstructionDialog = reconstructionDialog;
    imagesPreviouslyLoaded = false;

    if(amosRunning) {
        stopSegmentation();
        if(segmentationStopped)
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }
    subjectsAsMainDirs = project->getSubjectsAsMainDirs();
    reconstruction = reconstructionDialog->getReconstruction();
    
    QMap<QString, Reconstruction::Parameter> recParameters = reconstruction->getParameters();
    // In app version for final users the parameter "automatic" is suppressed from
    // processdata.apd in this case modeAutomatic == true always, they cannot train Amos
    if(recParameters.contains("automatic"))
        modeAutomatic = recParameters["automatic"].value == "true" ? true : false;
//     NSIGMADESVseed = recParameters["SIGMADESVseed"].value.toDouble();
//     NSIGMADESVseed2 = recParameters["SIGMADESVseed2"].value.toDouble();
//     NSIGMADESV_TH2 = recParameters["SIGMADESV_TH2"].value.toDouble();
//     NSIGMADESV_TH4 = recParameters["SIGMADESV_TH4"].value.toDouble();
//     ERODEWM = recParameters["ERODEWM"].value.toInt();
//     WEIGHT_CLASSIFIER_FL = recParameters["WEIGHT_CLASSIFIER_FL"].value.toDouble();
    
    QMap<QString, QString> parameters;
    parameters["NSIGMADESVseed"] = recParameters["SIGMADESVseed"].value;
    parameters["NSIGMADESVseed2"] = recParameters["SIGMADESVseed2"].value;
    parameters["NSIGMADESV_TH2"] = recParameters["SIGMADESV_TH2"].value;
    parameters["NSIGMADESV_TH4"] = recParameters["SIGMADESV_TH4"].value;
    parameters["ERODEWM"] = recParameters["ERODEWM"].value;
    parameters["WEIGHT_CLASSIFIER_FL"] = recParameters["WEIGHT_CLASSIFIER_FL"].value;    
    
    QList<ReconstructionInputInfo> inputInfoImages = reconstruction->getInputInfoImages();
    AmosImage* flairAmosImage = 0;
    AmosImage* T1AmosImage = 0;
    AmosImage* maskAmosImage = 0;
    AmosImage* labelAmosImage = 0;
    AmosImage* expertAmosImage = 0;
    for(int i = 0; i < inputInfoImages.size(); i++) {
        ReconstructionInputInfo inputInfo = inputInfoImages.at(i);
        if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::EXPERT) {
            expertAmosImage = project->getExpertImage();
        }
        else if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::MASK) {
            maskAmosImage = project->getMaskImage();
        }
        else if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::FLAIR_RECONS ||
                    AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::T1_RECONS ||
               AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::LABEL_RECONS) {        
            QList<Reconstruction*>* reconsList = project->getReconstructions();
            Reconstruction* recon = 0;
            AmosImage* image = 0;
            bool found = false;
            if(inputInfo.getReconstructionUuid() == project->getExternUuid()) {
                QList<AmosImage *>* externImages = project->getExternImages();
                for(int k = 0; k < externImages->size(); k++) {
                    image = externImages->at(k);
                    if(image->getImageUuid() == inputInfo.getImageUuid()) {
                        found = true;
                        break;
                    }
                }
            }
            if(!found) {
                for(int j = 0; j < reconsList->size(); j++) {
                    recon = reconsList->at(j);
                    if(recon == reconstruction)
                        continue;
                    if(recon->getReconstructionUuid() != inputInfo.getReconstructionUuid())
                        continue;
                    QList<AmosImage *> imagesOut = recon->getImagesOut();
                    for(int k = 0; k < imagesOut.size(); k++) {
                        image = imagesOut.at(k); 
                        if(image->getImageUuid() == inputInfo.getImageUuid()) {
                            found = true;
                            break;
                        }
                  
                    }
                }                
            }
            if(found) {
                if(image->getType() == AmosImage::FLAIR_RECONS)
                    flairAmosImage = image;
                else if(image->getType() == AmosImage::T1_RECONS)
                    T1AmosImage = image;
                else if(image->getType() == AmosImage::LABEL_RECONS)
                    labelAmosImage = image;                 
            }
        }        
     
    }
    
    if(!flairAmosImage || !T1AmosImage || !labelAmosImage || (!modeAutomatic && !expertAmosImage)) {
        QString title(tr("Images"));
        QString text(tr("Missing images."));
        QMessageBox::warning(0, title, text);
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }


    outputParentPath = project->getOutputPath();
    outputPath = reconstruction->getOutputPath();
    if(outputParentPath.endsWith(QDir::separator()))
        outputPath.remove(outputParentPath);
    else
        outputPath.remove(outputParentPath + QDir::separator());
    
    flairData.parentPath = flairAmosImage->getParentPath();
    if(subjectsAsMainDirs)
        flairData.path = flairAmosImage->getPath();
    flairData.name = flairAmosImage->getFileName();

    t1Data.parentPath = T1AmosImage->getParentPath();
    if(subjectsAsMainDirs)
        t1Data.path = T1AmosImage->getPath();
//     else
//         t1Data.path = "";
    t1Data.name = T1AmosImage->getFileName();
    
    labelsData.parentPath = labelAmosImage->getParentPath();
    if(subjectsAsMainDirs)
        labelsData.path = labelAmosImage->getPath();
    labelsData.name = labelAmosImage->getFileName();
    
    if(maskAmosImage) {
        maskData.parentPath = maskAmosImage->getParentPath();
        if(subjectsAsMainDirs)
            maskData.path = maskAmosImage->getPath();
        maskData.name = maskAmosImage->getFileName();        
    }    
    
    if(!modeAutomatic) {
        annotData.parentPath = expertAmosImage->getParentPath();
        if(subjectsAsMainDirs)
            annotData.path = expertAmosImage->getPath();
        annotData.name = expertAmosImage->getFileName();        
    } 
    
    QFileInfo fileInfo;
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn;
    QPair<Utils::ImageTypes, Utils::AmosImageData> pairF(Utils::FLAIR, flairData);
    fileInfo.setFile(flairData.parentPath);
    if(!fileInfo.exists()) {
        QMessageBox msgBox;
        msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
        msgBox.exec();
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;	
    }        
    amosImagesListIn.append(pairF);   
    QPair<Utils::ImageTypes, Utils::AmosImageData> pairT(Utils::T1, t1Data);   
    amosImagesListIn.append(pairT);  
    QPair<Utils::ImageTypes, Utils::AmosImageData> pairL(Utils::LABEL, labelsData);
    amosImagesListIn.append(pairL);
    
    if(!maskData.parentPath.isEmpty()) {
        fileInfo.setFile(maskData.parentPath);
        if(!fileInfo.exists()) {
            QMessageBox msgBox;
            msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
            msgBox.exec();
            if(!imagesPreviouslyLoaded) {
                reconstructionDialog->processFinished(fullyProcessedSubjects);
            }
            return;	
        } 
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairM(Utils::MASK, maskData);
        amosImagesListIn.append(pairM);            
    }
    
    if(!modeAutomatic) {
        fileInfo.setFile(annotData.parentPath);
        if(!fileInfo.exists()) {
            QMessageBox msgBox;
            msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
            msgBox.exec();
            reconstructionDialog->processFinished(fullyProcessedSubjects);
            return;	
        } 	
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairE(Utils::EXPERT, annotData);
        amosImagesListIn.append(pairE);
    }    

    QList<AmosImage *>& outputImages = reconstruction->getImagesOut();
    for(int i = 0; i < outputImages.size(); i++) {
        if(outputImages.at(i)->getType() == AmosImage::AMOS_AC) 
            ACName = outputImages.at(i)->getFileName();
        else if(outputImages.at(i)->getType() == AmosImage::AMOS_BC) 
            BCName = outputImages.at(i)->getFileName();
    }
    
    QStringList outputNames;
    outputNames << ACName << BCName;
    
    QStringList externalFiles;
    externalFiles << svmModelFileName << svmScaleFileName;
    
    QString externalMessage = tr("Missing SVM files");
    
    amos = new AmosProcessDirectory(); 
    runInternalProcess(outputNames, parameters, amosImagesListIn, externalFiles, externalMessage);    
}

void QmitkAmosWorkbenchView::DoContrastProcessing()
{
    if(amosRunning) {
        stopSegmentation();
        return;
    }
    
    imagesPreviouslyLoaded = true;
    numValidCases = 1;
    modeAutomatic = true;    
    subjectsAsMainDirs = false;
    
    flairContrastNode = m_Controls.flairImageSelectorEnhance->GetSelectedNode();
    mitk::DataNode::Pointer lblNode = m_Controls.lblImageSelectorEnhance->GetSelectedNode();
    mitk::DataNode::Pointer maskNode = m_Controls.maskImageSelectorEnhance->GetSelectedNode();

    imagesMITK.clear();
    imagesMITK.append(dynamic_cast<mitk::Image*>(flairContrastNode->GetData()));
    imagesMITK.append(dynamic_cast<mitk::Image*>(lblNode->GetData())); 
    if(maskEnhancedEnabled)
        imagesMITK.append(dynamic_cast<mitk::Image*>(maskNode->GetData()));
    else
        imagesMITK.append(0);
    
    mitk::PropertyList::Pointer propertyList = flairContrastNode->GetPropertyList();
    bool emptyList = propertyList->IsEmpty();
    QString FLpath;
    if(!emptyList) {
        QString name = QString::fromStdString(propertyList->GetProperty("name")->GetValueAsString());
        if(propertyList->GetMap()->find("path") != propertyList->GetMap()->end()) {
            FLpath = QString::fromStdString(propertyList->GetProperty("path")->GetValueAsString());
            QFileInfo info = QFileInfo(QDir(FLpath), name);
            FLpath = info.filePath();
        }
        else
            FLpath  = name;
    }
    else
        // Image not loaded but generated by another process (e.g. contrast enhancement)
        FLpath = "Flair";
    
    QFileInfo fileF(FLpath);
    QDir dir = fileF.absoluteDir();
    
    QString EnhancedName = QFileInfo(dir, fileF.baseName() + "_Enhanced.nii" + fileF.completeSuffix()).filePath();
    QString gradientName = QFileInfo(dir, "gradientmap.txt").filePath();
    QString edgesName = QFileInfo(dir, "edgesmap.pgm").filePath();
    QString confidenceName = QFileInfo(dir, "confidencemap.txt").filePath();
    
//     QString EnhancedName = "Flair_Enhanced";
//     QString PgmName = "Flair";
//     QString gradientName = "gradientmap.txt";
    
    QMap<QString, QString> parameters;
    parameters["GradientWindowRadius"] = QString::number(GradientWinRad);
    parameters["MinimumLength"] = QString::number(MinimumLength);
    parameters["NmxRank"] = QString::number(NmxRank);
    parameters["NmxConf"] = QString::number(NmxConf);
//     parameters["NmxType"] = QString(CURVETYPE_LIST[NmxType]);
    parameters["NmxType"] = QString::number(NmxType);
    parameters["HysterisisHighRank"] = QString::number(HystHighRank);
    parameters["HysterisisHighConf"] = QString::number(HystHighConf);
//     parameters["HysterisisHighType"] = QString(CURVETYPE_LIST[HystHighType]);
    parameters["HysterisisHighType"] = QString::number(HystHighType);
    parameters["HysterisisLowRank"] = QString::number(HystLowRank);
    parameters["HysterisisLowConf"] = QString::number(HystLowConf);
//     parameters["HysterisisLowType"] = QString(CURVETYPE_LIST[HystLowType]);
    parameters["HysterisisLowType"] = QString::number(HystLowType);
    parameters["CustCurveHysterisisLow_00"] = QString::number(CustCurveHystLow_00);    
    parameters["CustCurveHysterisisLow_01"] = QString::number(CustCurveHystLow_01);
    parameters["CustCurveHysterisisLow_10"] = QString::number(CustCurveHystLow_10);
    parameters["CustCurveHysterisisLow_11"] = QString::number(CustCurveHystLow_11);
    parameters["saveEdgesMap"] = QString::number(saveEdgesMap);
    parameters["saveConfidenceMap"] = QString::number(saveConfidenceMap);
    parameters["saveGradientMap"] = QString::number(saveGradientMap);
    
    
    QStringList outputNames;
    outputNames << EnhancedName << gradientName << edgesName << confidenceName; 
    
    QStringList externalFiles;   
    amos = new ContrastProcessDirectory();
    if(saveEdgesMap)
        amos->increaseNumMITKOutputImages();
    
//     runSegmentation();
    runInternalProcess(outputNames, parameters, QList<QPair<Utils::ImageTypes, Utils::AmosImageData> >(), externalFiles);
}

void QmitkAmosWorkbenchView::doBatchContrastProcessing(ReconstructionDialog* reconstructionDialog)
{
    internalProcessType = AMOS;
    this->reconstructionDialog = reconstructionDialog;
    imagesPreviouslyLoaded = false;

    if(amosRunning) {
        stopSegmentation();
        if(segmentationStopped)
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }
    subjectsAsMainDirs = project->getSubjectsAsMainDirs();
    reconstruction = reconstructionDialog->getReconstruction();
    
    QMap<QString, Reconstruction::Parameter> recParameters = reconstruction->getParameters();
    
    QMap<QString, QString> parameters;
    parameters["GradientWindowRadius"] = recParameters["GradientWindowRadius"].value;
    parameters["MinimumLength"] = recParameters["MinimumLength"].value;
    parameters["NmxRank"] = recParameters["NmxRank"].value;
    parameters["NmxConf"] = recParameters["NmxConf"].value;
//     parameters["NmxType"] = QString(CURVETYPE_LIST[NmxType]);
    parameters["NmxType"] = recParameters["NmxType"].value;
    parameters["HysterisisHighRank"] = recParameters["HysterisisHighRank"].value;
    parameters["HysterisisHighConf"] = recParameters["HysterisisHighConf"].value;
//     parameters["HysterisisHighType"] = QString(CURVETYPE_LIST[HystHighType]);
    parameters["HysterisisHighType"] = recParameters["HysterisisHighType"].value;
    parameters["HysterisisLowRank"] = recParameters["HysterisisLowRank"].value;
    parameters["HysterisisLowConf"] = recParameters["HysterisisLowConf"].value;
//     parameters["HysterisisLowType"] = QString(CURVETYPE_LIST[HystLowType]);
    parameters["HysterisisLowType"] = recParameters["HysterisisLowType"].value;
    parameters["CustCurveHysterisisLow_00"] = recParameters["CustCurveHysterisisLow_00"].value;    
    parameters["CustCurveHysterisisLow_01"] = recParameters["CustCurveHysterisisLow_01"].value;
    parameters["CustCurveHysterisisLow_10"] = recParameters["CustCurveHysterisisLow_10"].value;
    parameters["CustCurveHysterisisLow_11"] = recParameters["CustCurveHysterisisLow_11"].value;
    parameters["saveEdgesMap"] = recParameters["saveEdgesMap"].value;
    parameters["saveConfidenceMap"] = recParameters["saveConfidenceMap"].value;
    parameters["saveGradientMap"] = recParameters["saveGradientMap"].value;   
    
    QList<ReconstructionInputInfo> inputInfoImages = reconstruction->getInputInfoImages();
    AmosImage* flairAmosImage = 0;
    AmosImage* maskAmosImage = 0;
    AmosImage* labelAmosImage = 0;
    for(int i = 0; i < inputInfoImages.size(); i++) {
        ReconstructionInputInfo inputInfo = inputInfoImages.at(i);
        if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::MASK) {
            maskAmosImage = project->getMaskImage();
        }
        else if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::FLAIR_RECONS ||
               AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::LABEL_RECONS) {        
            QList<Reconstruction*>* reconsList = project->getReconstructions();
            Reconstruction* recon = 0;
            AmosImage* image = 0;
            bool found = false;
            if(inputInfo.getReconstructionUuid() == project->getExternUuid()) {
                QList<AmosImage *>* externImages = project->getExternImages();
                for(int k = 0; k < externImages->size(); k++) {
                    image = externImages->at(k);
                    if(image->getImageUuid() == inputInfo.getImageUuid()) {
                        found = true;
                        break;
                    }
                }
            }
            if(!found) {
                for(int j = 0; j < reconsList->size(); j++) {
                    recon = reconsList->at(j);
                    if(recon == reconstruction)
                        continue;
                    if(recon->getReconstructionUuid() != inputInfo.getReconstructionUuid())
                        continue;
                    QList<AmosImage *> imagesOut = recon->getImagesOut();
                    for(int k = 0; k < imagesOut.size(); k++) {
                        image = imagesOut.at(k); 
                        if(image->getImageUuid() == inputInfo.getImageUuid()) {
                            found = true;
                            break;
                        }
                  
                    }
                }                
            }
            if(found) {
                if(image->getType() == AmosImage::FLAIR_RECONS)
                    flairAmosImage = image;
                else if(image->getType() == AmosImage::LABEL_RECONS)
                    labelAmosImage = image;                 
            }
        }        
     
    }
    
    if(!flairAmosImage || !labelAmosImage) {
        QString title(tr("Images"));
        QString text(tr("Missing images."));
        QMessageBox::warning(0, title, text);
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }

    outputParentPath = project->getOutputPath();
    outputPath = reconstruction->getOutputPath();
    if(outputParentPath.endsWith(QDir::separator()))
        outputPath.remove(outputParentPath);
    else
        outputPath.remove(outputParentPath + QDir::separator());
    
    flairData.parentPath = flairAmosImage->getParentPath();
    if(subjectsAsMainDirs)
        flairData.path = flairAmosImage->getPath();
    flairData.name = flairAmosImage->getFileName();
    
    labelsData.parentPath = labelAmosImage->getParentPath();
    if(subjectsAsMainDirs)
        labelsData.path = labelAmosImage->getPath();
    labelsData.name = labelAmosImage->getFileName();
    
    if(maskAmosImage) {
        maskData.parentPath = maskAmosImage->getParentPath();
        if(subjectsAsMainDirs)
            maskData.path = maskAmosImage->getPath();
        maskData.name = maskAmosImage->getFileName();        
    }     
    
    QFileInfo fileInfo;
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn;
    QPair<Utils::ImageTypes, Utils::AmosImageData> pairF(Utils::FLAIR, flairData);
    fileInfo.setFile(flairData.parentPath);
    if(!fileInfo.exists()) {
        QMessageBox msgBox;
        msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
        msgBox.exec();
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;	
    }        
    amosImagesListIn.append(pairF);     
    QPair<Utils::ImageTypes, Utils::AmosImageData> pairL(Utils::LABEL, labelsData);
    amosImagesListIn.append(pairL);
    
    if(!maskData.parentPath.isEmpty()) {
        fileInfo.setFile(maskData.parentPath);
        if(!fileInfo.exists()) {
            QMessageBox msgBox;
            msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
            msgBox.exec();
            if(!imagesPreviouslyLoaded) {
                reconstructionDialog->processFinished(fullyProcessedSubjects);
            }
            return;	
        } 
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairM(Utils::MASK, maskData);
        amosImagesListIn.append(pairM);            
    }

    QList<AmosImage *>& outputImages = reconstruction->getImagesOut();
//     for(int i = 0; i < outputImages.size(); i++) {
//         if(outputImages.at(i)->getType() == AmosImage::FLAIR_RECONS) 
//             EnhancedName = outputImages.at(i)->getFileName();
//     }
    if(outputImages.at(0)->getType() == AmosImage::FLAIR_RECONS) 
        EnhancedName = outputImages.at(0)->getFileName(); 
    QString gradientName = "gradientmap.txt";
    QString edgesName = "edgesmap.pgm";
    QString confidenceName = "confidencemap.txt";    
    
    QStringList outputNames;
    outputNames << EnhancedName << gradientName << edgesName << confidenceName;
    
    QStringList externalFiles;
    
    QString externalMessage = "";
    
    amos = new ContrastProcessDirectory(); 
    runInternalProcess(outputNames, parameters, amosImagesListIn, externalFiles, externalMessage);    
}

void QmitkAmosWorkbenchView::onMaskChecked(int state)
{
    mitk::DataStorage::SetOfObjects::ConstPointer images = GetDataStorage()->GetSubset(m_IsOfTypeImagePredicate);    
    if(state == Qt::Checked) {
        maskEnabled = true;
        
        for(mitk::DataStorage::SetOfObjects::const_iterator iter = images->begin(); iter != images->end(); ++iter)
        {
            mitk::DataNode* node = *iter;
            int index = m_Controls.maskImageSelector->Find(node);
            if(index == -1) {
                m_Controls.maskImageSelector->AddNode(node);
                index = m_Controls.maskImageSelector->Find(node);
            }
            QString name = QString::fromStdString(node->GetName());
            if(name.toUpper().contains("MASK"))
                m_Controls.maskImageSelector->setCurrentIndex(index);	    
        }
    }
    else {
        maskEnabled = false;		
    }
    m_Controls.maskImageSelector->setEnabled(maskEnabled);
    m_Controls.labelWarning->setVisible(!validImages());
}

void QmitkAmosWorkbenchView::onMaskEnhancedChecked(int state)
{
    mitk::DataStorage::SetOfObjects::ConstPointer images = GetDataStorage()->GetSubset(m_IsOfTypeImagePredicate);    
    if(state == Qt::Checked) {
        maskEnhancedEnabled = true;
        
        for(mitk::DataStorage::SetOfObjects::const_iterator iter = images->begin(); iter != images->end(); ++iter)
        {
            mitk::DataNode* node = *iter;
            int index = m_Controls.maskImageSelector->Find(node);
            if(index == -1) {
                m_Controls.maskImageSelector->AddNode(node);
                index = m_Controls.maskImageSelector->Find(node);
            }
            QString name = QString::fromStdString(node->GetName());
            if(name.toUpper().contains("MASK"))
                m_Controls.maskImageSelector->setCurrentIndex(index);	    
        }
    }
    else {
        maskEnhancedEnabled = false;		
    }
    m_Controls.maskImageSelectorEnhance->setEnabled(maskEnhancedEnabled);
    m_Controls.labelWarning->setVisible(!validImagesEnhance());
}

void QmitkAmosWorkbenchView::NodeAdded( const mitk::DataNode* node)
{
    if(node != NULL && dynamic_cast<mitk::Image*>(node->GetData()))
    {
        // 	bool labelVisible = false;
        m_Controls.flairImageSelector->AddNode(node);
        m_Controls.t1ImageSelector->AddNode(node);
        m_Controls.lblImageSelector->AddNode(node);
        
        m_Controls.flairImageSelectorEnhance->AddNode(node);
        m_Controls.lblImageSelectorEnhance->AddNode(node);        
        
        if(maskEnabled) 
            m_Controls.maskImageSelector->AddNode(node);
        if(maskEnhancedEnabled) 
            m_Controls.maskImageSelectorEnhance->AddNode(node);        
        
        QString name = QString::fromStdString(node->GetName());
        int index;	
        if(name.left(2).toUpper() == "FL" || name.toUpper().contains("FLAIR")) {
            index = m_Controls.flairImageSelector->Find(node);
            m_Controls.flairImageSelector->setCurrentIndex(index);
            m_Controls.flairImageSelectorEnhance->setCurrentIndex(index);
        }
        else if(name.toUpper().contains("LB") || name.toUpper().contains("LABEL")) {
            index = m_Controls.lblImageSelector->Find(node);
            m_Controls.lblImageSelector->setCurrentIndex(index);
            m_Controls.lblImageSelectorEnhance->setCurrentIndex(index);
        }
        else if(name.toUpper().contains("T1")) {
            index = m_Controls.t1ImageSelector->Find(node);
            m_Controls.t1ImageSelector->setCurrentIndex(index);
        }
        else if(name.toUpper().contains("MASK") && maskEnabled) {
            index = m_Controls.maskImageSelector->Find(node);
            m_Controls.maskImageSelector->setCurrentIndex(index);
            m_Controls.maskImageSelectorEnhance->setCurrentIndex(index);
        }	
        m_Controls.labelWarning->setVisible(!validImages());
        m_Controls.labelWarningEnhance->setVisible(!validImagesEnhance());
    }    
}

void QmitkAmosWorkbenchView::NodeRemoved( const mitk::DataNode* node)
{
    if(node != NULL && dynamic_cast<mitk::Image*>(node->GetData()))
    {    
        m_Controls.flairImageSelector->RemoveNode(node);
        m_Controls.t1ImageSelector->RemoveNode(node);
        m_Controls.lblImageSelector->RemoveNode(node);
        
        m_Controls.flairImageSelectorEnhance->RemoveNode(node);
        m_Controls.lblImageSelectorEnhance->RemoveNode(node);        
        
        if(m_Controls.maskImageSelector->Find(node) > -1)
            m_Controls.maskImageSelector->RemoveNode(node);
        if(m_Controls.maskImageSelectorEnhance->Find(node) > -1)
            m_Controls.maskImageSelectorEnhance->RemoveNode(node);        
        
        m_Controls.labelWarning->setVisible(!validImages());
        m_Controls.labelWarningEnhance->setVisible(!validImagesEnhance());
    }
}

bool QmitkAmosWorkbenchView::validImages()
{
    mitk::DataNode::Pointer flairNode = m_Controls.flairImageSelector->GetSelectedNode();
    mitk::DataNode::Pointer t1Node = m_Controls.t1ImageSelector->GetSelectedNode();
    mitk::DataNode::Pointer lblNode = m_Controls.lblImageSelector->GetSelectedNode();
    mitk::DataNode::Pointer maskNode = m_Controls.maskImageSelector->GetSelectedNode();
    bool valid = flairNode.IsNotNull() && t1Node.IsNotNull() && lblNode.IsNotNull();
    if(maskEnabled)
        valid = valid && maskNode.IsNotNull();
    if(valid) {
        valid = valid && flairNode != t1Node && flairNode != lblNode && t1Node != lblNode;
        if(maskEnabled)
            valid = valid && maskNode != flairNode && maskNode != t1Node && maskNode != lblNode;	    
    }
    m_Controls.buttonPerformSegmentation->setEnabled(valid);
    return valid;
}

bool QmitkAmosWorkbenchView::validImagesEnhance()
{
    mitk::DataNode::Pointer flairNode = m_Controls.flairImageSelectorEnhance->GetSelectedNode();
    mitk::DataNode::Pointer lblNode = m_Controls.lblImageSelector->GetSelectedNode();
    mitk::DataNode::Pointer maskNode = m_Controls.maskImageSelector->GetSelectedNode();
    bool valid = flairNode.IsNotNull() && lblNode.IsNotNull();
    if(maskEnhancedEnabled)
        valid = valid && maskNode.IsNotNull();    
    m_Controls.buttonPerformSegmentation->setEnabled(valid);
    return valid;
}
// void QmitkAmosWorkbenchView::onThresholdSet(double /*threshold*/)
// {
//     
// }

/*
void QmitkAmosWorkbenchView::runSegmentation()
{
    QFileInfo fileInfo;
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn;
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListOut;
    
    segmentationStopped = false;

    if(imagesPreviouslyLoaded) {
        beginLoadedCase = chrono::high_resolution_clock::now();
        m_Controls.processProgressBar->reset();
    }
    
    QFileInfo svmModel(svmModelFileName);
    QFileInfo svmScale(svmScaleFileName);
    if(!svmModel.exists() || !svmScale.exists()) {
        QMessageBox msgBox;
        msgBox.setText(tr("SVM files does not exist"));
        msgBox.exec();
        if(!imagesPreviouslyLoaded) {
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        }
        return;
    }
    
    if(!imagesPreviouslyLoaded) {
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairF(Utils::FLAIR, flairData);
        fileInfo.setFile(flairData.parentPath);
        if(!fileInfo.exists()) {
            QMessageBox msgBox;
            msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
            msgBox.exec();
//             if(!imagesPreviouslyLoaded) {
            reconstructionDialog->processFinished(fullyProcessedSubjects);
//             }
            return;	
        }        
        amosImagesListIn.append(pairF);   
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairT(Utils::T1, t1Data);   
        amosImagesListIn.append(pairT);  
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairL(Utils::LABEL, labelsData);
        amosImagesListIn.append(pairL);
        
        if(!maskData.parentPath.isEmpty()) {
            fileInfo.setFile(maskData.parentPath);
            if(!fileInfo.exists()) {
                QMessageBox msgBox;
                msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
                msgBox.exec();
                if(!imagesPreviouslyLoaded) {
                    reconstructionDialog->processFinished(fullyProcessedSubjects);
                }
                return;	
            } 
            QPair<Utils::ImageTypes, Utils::AmosImageData> pairM(Utils::MASK, maskData);
            amosImagesListIn.append(pairM);            
        }
        
        if(!modeAutomatic) {
            fileInfo.setFile(annotData.parentPath);
            if(!fileInfo.exists()) {
                QMessageBox msgBox;
                msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
                msgBox.exec();
                if(!imagesPreviouslyLoaded) {
                    reconstructionDialog->processFinished(fullyProcessedSubjects);
                }
                return;	
            } 	
            QPair<Utils::ImageTypes, Utils::AmosImageData> pairE(Utils::EXPERT, annotData);
            amosImagesListIn.append(pairE);
        }
    }
    
    QThread* segmentationThread = new QThread;
    amos = new AmosProcessDirectory();        
    amos->moveToThread(segmentationThread);
    amos->setSubjectsAsMainDirs(subjectsAsMainDirs);
    amos->setImagesPreviouslyLoaded(imagesPreviouslyLoaded);

    QMap<QString, QString> parameters;
    parameters["NSIGMADESVseed"] = QString::number(NSIGMADESVseed);
    parameters["NSIGMADESVseed2"] = QString::number(NSIGMADESVseed2);
    parameters["NSIGMADESV_TH2"] = QString::number(NSIGMADESV_TH2);
    parameters["NSIGMADESV_TH4"] = QString::number(NSIGMADESV_TH4);
    parameters["ERODEWM"] = QString::number(ERODEWM);
    parameters["WEIGHT_CLASSIFIER_FL"] = QString::number(WEIGHT_CLASSIFIER_FL);
    amos->setParameters(parameters);
    amos->setModeAutomatic(modeAutomatic);
    amos->setMode3D(mode3D);
    
    QStringList selectedSubjects;
    if(!imagesPreviouslyLoaded) {
        selectedSubjects = reconstructionDialog->getSelectedSubjects();
        numThreads = reconstructionDialog->getNumberOfThreads();
        numSubjects = min(selectedSubjects.size(), reconstructionDialog->getNumberOfSubjects());
    }
    amos->setNumThreads(numThreads);
    amos->setNumSimultaneousSubjects(numSubjects);
    QStringList externalFiles;
    externalFiles << svmModelFileName << svmScaleFileName;
    amos->setExternalFiles(externalFiles);
    
    connect(amos, SIGNAL(sendLog(QString)), this, SLOT(appendTextLog(QString)));
    
    connect(amos, SIGNAL(sendProgressRange(int)), this, SLOT(getProgressRange(int)));
    connect(amos, SIGNAL(sendProgress(int)), this, SLOT(getProgress(int)));
    connect(amos, SIGNAL(sendProgressType(QString)), this, SLOT(getProgressType(QString)));
    
    connect(amos, SIGNAL(sendCaseName(QString)), this, SLOT(getCaseName(QString)));
    connect(amos, SIGNAL(sendCaseProgressRange(int)), this, SLOT(getCaseProgressRange(int)));
    connect(amos, SIGNAL(sendCaseProgress(int)), this, SLOT(getCaseProgress(int)));
    connect(amos, SIGNAL(sendTime(int)), this, SLOT(showTime(int)));
    connect(amos, SIGNAL(sendMessageToUser(QString)), this, SLOT(onMessageToUser(QString)));
    
    connect(segmentationThread, SIGNAL (started()), amos, SLOT (processCases()));
    connect(amos, SIGNAL (processFinished()), segmentationThread, SLOT (quit()));
    connect(amos, SIGNAL (processFinished()), amos, SLOT (deleteLater()));
    connect(amos, SIGNAL (processFinished()), this, SLOT (segmentationFinished()));
    connect(amos, SIGNAL (processedSubject(QStringList)), this, SLOT (setFullyProcessedSubjects(QStringList)));
    connect(segmentationThread, SIGNAL (finished()), segmentationThread, SLOT (deleteLater()));
    
    qRegisterMetaType<QList< QPair<Image::Pointer, QString> > >("QList< QPair<Image::Pointer, QString> >");
    connect(amos, SIGNAL (sendSegmentationResults(QList< QPair<Image::Pointer, QString> > )), this, SLOT (setSegmentationResults(QList< QPair<Image::Pointer, QString> > )));
    
    segmentationResults.clear();
    proccessTime.setHMS(0, 0, 0, 0);
    showTime();
    
    bool ret;
    if(!imagesPreviouslyLoaded) {          
        amos->setSubjects(selectedSubjects);
        QStringList outNames;
        outNames << ACName << BCName;
        ret = amos->setAmosImagesData(amosImagesListIn, outputParentPath, outputPath, outNames);        
    }
    else {
        QStringList outNames;
        outNames << ACName << BCName;
        amos->setOutNames(outNames);
        ret = amos->setSubjectName(tr("Subject"));
    }

    if(ret) {
        if(imagesPreviouslyLoaded) {
            amos->setImagesList(imagesMITK);
            m_Controls.buttonPerformSegmentation->setText(tr("Stop"));
        }
        timer->start();		
        segmentationThread->start();
        amosRunning = true;
        projectDialog->conditionalEnableRun(false);        
    }
    else {
        QMessageBox msgBox;
        msgBox.setText(tr("Images problems"));
        msgBox.exec();
        if(!imagesPreviouslyLoaded) {
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        }
        return;	
    }   
}
*/

void QmitkAmosWorkbenchView::runInternalProcess(QStringList outputNames, QMap< QString, QString > parameters, QList< QPair< Utils::ImageTypes,  Utils::AmosImageData > > amosImagesListIn, QStringList externalFiles, QString externalMessage)
{
    
    segmentationStopped = false;

    if(imagesPreviouslyLoaded) {
        beginLoadedCase = chrono::high_resolution_clock::now();
        m_Controls.processProgressBar->reset();
    }
    
    bool retExt = true;
    QFileInfo fileInfo;
    for(int i = 0; i < externalFiles.size(); i++) {
        fileInfo.setFile(externalFiles.at(i));
        retExt = retExt && fileInfo.exists();
    }
    if(!retExt) {
        QMessageBox msgBox;
        msgBox.setText(externalMessage);
        msgBox.exec();
        if(!imagesPreviouslyLoaded) {
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        }
        return;
    }
    
    QThread* segmentationThread = new QThread;
    /*amos = new AmosProcessDirectory(); */       
    amos->moveToThread(segmentationThread);
    amos->setSubjectsAsMainDirs(subjectsAsMainDirs);
    amos->setImagesPreviouslyLoaded(imagesPreviouslyLoaded);

//     QMap<QString, QString> parameters;
//     parameters["NSIGMADESVseed"] = QString::number(NSIGMADESVseed);
//     parameters["NSIGMADESVseed2"] = QString::number(NSIGMADESVseed2);
//     parameters["NSIGMADESV_TH2"] = QString::number(NSIGMADESV_TH2);
//     parameters["NSIGMADESV_TH4"] = QString::number(NSIGMADESV_TH4);
//     parameters["ERODEWM"] = QString::number(ERODEWM);
//     parameters["WEIGHT_CLASSIFIER_FL"] = QString::number(WEIGHT_CLASSIFIER_FL);
    amos->setParameters(parameters);
    amos->setModeAutomatic(modeAutomatic);
    amos->setMode3D(mode3D);
    
    QStringList selectedSubjects;
    if(!imagesPreviouslyLoaded) {
        selectedSubjects = reconstructionDialog->getSelectedSubjects();
        numThreads = reconstructionDialog->getNumberOfThreads();
        numSubjects = min(selectedSubjects.size(), reconstructionDialog->getNumberOfSubjects());
    }
    amos->setNumThreads(numThreads);
    amos->setNumSimultaneousSubjects(numSubjects);
//     QStringList externalFiles;
//     externalFiles << svmModelFileName << svmScaleFileName;
    amos->setExternalFiles(externalFiles);
    
    connect(amos, SIGNAL(sendLog(QString)), this, SLOT(appendTextLog(QString)));
    
    connect(amos, SIGNAL(sendProgressRange(int)), this, SLOT(getProgressRange(int)));
    connect(amos, SIGNAL(sendProgress(int)), this, SLOT(getProgress(int)));
    connect(amos, SIGNAL(sendProgressType(QString)), this, SLOT(getProgressType(QString)));
    
    connect(amos, SIGNAL(sendCaseName(QString)), this, SLOT(getCaseName(QString)));
    connect(amos, SIGNAL(sendCaseProgressRange(int)), this, SLOT(getCaseProgressRange(int)));
    connect(amos, SIGNAL(sendCaseProgress(int)), this, SLOT(getCaseProgress(int)));
    connect(amos, SIGNAL(sendTime(int)), this, SLOT(showTime(int)));
    connect(amos, SIGNAL(sendMessageToUser(QString)), this, SLOT(onMessageToUser(QString)));
    
    connect(segmentationThread, SIGNAL (started()), amos, SLOT (processCases()));
    connect(amos, SIGNAL (processFinished()), segmentationThread, SLOT (quit()));
    connect(amos, SIGNAL (processFinished()), amos, SLOT (deleteLater()));
    connect(amos, SIGNAL (processFinished()), this, SLOT (segmentationFinished()));
    connect(amos, SIGNAL (processedSubject(QStringList)), this, SLOT (setFullyProcessedSubjects(QStringList)));
    connect(segmentationThread, SIGNAL (finished()), segmentationThread, SLOT (deleteLater()));
    
    qRegisterMetaType<QList< QPair<Image::Pointer, QString> > >("QList< QPair<Image::Pointer, QString> >");
    connect(amos, SIGNAL (sendSegmentationResults(QList< QPair<Image::Pointer, QString> > )), this, SLOT (setSegmentationResults(QList< QPair<Image::Pointer, QString> > )));
    
    segmentationResults.clear();
    proccessTime.setHMS(0, 0, 0, 0);
    showTime();
    
    bool ret;
    if(!imagesPreviouslyLoaded) {          
        amos->setSubjects(selectedSubjects);
//         QStringList outNames;
//         outNames << ACName << BCName;
        ret = amos->setAmosImagesData(amosImagesListIn, outputParentPath, outputPath, outputNames);        
    }
    else {
//         QStringList outNames;
//         outNames << ACName << BCName;
        amos->setOutNames(outputNames);
        ret = amos->setSubjectName(tr("Subject"));
    }

    if(ret) {
        if(imagesPreviouslyLoaded) {
            amos->setImagesList(imagesMITK);
            m_Controls.buttonPerformSegmentation->setText(tr("Stop"));
        }
        timer->start();		
        segmentationThread->start();
        amosRunning = true;
        projectDialog->conditionalEnableRun(false);        
    }
    else {
        QMessageBox msgBox;
        msgBox.setText(tr("Images problems"));
        msgBox.exec();
        if(!imagesPreviouslyLoaded) {
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        }
        return;	
    }   
}

void QmitkAmosWorkbenchView::segmentationFinished()
{
    if(amos /*&& amosRunning*/) {
        timer->stop();
        amos = 0;
        amosRunning = false;
        projectDialog->conditionalEnableRun(true);
        
        if(imagesPreviouslyLoaded) {
            if(!segmentationStopped) {
                mitk::DataStorage::Pointer ds = GetDataStorage();
                mitk::DataStorage::SetOfObjects::ConstPointer images = GetDataStorage()->GetSubset(m_IsOfTypeImagePredicate);
                if(internalProcessType == AMOS) {
                    for(mitk::DataStorage::SetOfObjects::const_iterator iter = images->begin(); iter != images->end(); ++iter)
                    {
                        mitk::DataNode* node = *iter;
                        ds->Remove(node);	    
                    }
                }
            }
            auto end = chrono::high_resolution_clock::now();
            int mSecs = chrono::duration_cast<chrono::nanoseconds>(end - beginLoadedCase).count()/1000000.0;
            // emit sendLog("Total time " + QString::number(mSecs) + "ms."); 
            emit sendTime(mSecs);
            m_Controls.progressLabel->setText(tr("Procedure"));
        }
        else
            reconstructionDialog->setCaseName(tr("Case"));

        QMessageBox msgBox;
        msgBox.setText(tr("Segmentation has finished"));
        msgBox.exec();
        
        if(imagesPreviouslyLoaded) {
            if(!segmentationStopped) {
                mitk::DataStorage::Pointer ds = GetDataStorage();
                if(internalProcessType == AMOS) {
                    for(int i = 0; i < segmentationResults.size(); i++) {
                        mitk::DataNode::Pointer imageNode = mitk::DataNode::New();
                        imageNode->SetData(segmentationResults.at(i).first);
                        imageNode->SetName(segmentationResults.at(i).second.toStdString());
                        ds->Add(imageNode);
                        mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(ds);
                    }
                }
                else if(internalProcessType == CONTRAST) {
//                     flairContrastNode->SetData(segmentationResults.at(0).first);
//                     flairContrastNode->SetName(segmentationResults.at(0).second.toStdString());                     
//                     mitk::DataNode::Pointer imageNode = mitk::DataNode::New();
//                     imageNode->SetData(segmentationResults.at(1).first);
//                     imageNode->SetName(segmentationResults.at(1).second.toStdString());                                        
//                     ds->Add(imageNode, flairContrastNode);
//                     mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(ds);                     
                    mitk::DataNode::Pointer imageNode = mitk::DataNode::New();
                    imageNode->SetData(segmentationResults.at(0).first);
                    imageNode->SetName(segmentationResults.at(0).second.toStdString());                                        
                    ds->Add(imageNode, flairContrastNode);
                    if(saveEdgesMap) {
                        mitk::DataNode::Pointer edgeNode = mitk::DataNode::New();
                        edgeNode->SetData(segmentationResults.at(1).first);
                        edgeNode->SetName(segmentationResults.at(1).second.toStdString());                                        
                        ds->Add(edgeNode, flairContrastNode);  
                        mitk::RenderingManager::GetInstance()->InitializeViews(segmentationResults.at(1).first->GetTimeGeometry(), mitk::RenderingManager::REQUEST_UPDATE_ALL, true);
                        /*mitk::TimeGeometry::ConstPointer geo = ds->ComputeBoundingGeometry3D(ds->GetAll());
                        mitk::RenderingManager::GetInstance()->InitializeViews( geo ); */                       
                    }
                    //mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(ds);
                }
            }	    
            segmentationResults.clear();
            m_Controls.processProgressBar->reset();
            m_Controls.buttonPerformSegmentation->setText(tr("Run"));
        }
        else {
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        }
        segmentationStopped = false;
        return;	
    }
}

void QmitkAmosWorkbenchView::stopSegmentation()
{
    if(!amos || !amosRunning) 
        return;
    
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to stop the segmentation proccess?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int result = msgBox.exec();
    if(result == QMessageBox::Yes) {
        amos->stopProcess();
        segmentationStopped = true;
    }
    
    return;	
}

void QmitkAmosWorkbenchView::appendTextLog(QString log)
{
    MITK_INFO << log.toStdString();
}

void QmitkAmosWorkbenchView::showFiltersTime(bool show)
{
    Utils::showTime = show;
    if(show)
        connect(logSender, SIGNAL(sendText(QString)), this, SLOT(appendTextLog(QString)));
    else
        disconnect(logSender, SIGNAL(sendText(QString)), this, SLOT(appendTextLog(QString)));
}

void QmitkAmosWorkbenchView::showLogWindow(bool show)
{
    showLog = show;
    if(!show) {
        if(Utils::showTime) {
            Utils::showTime = false;
            disconnect(logSender, SIGNAL(sendText(QString)), this, SLOT(appendTextLog(QString)));
        }
    }
}

void QmitkAmosWorkbenchView::getProgressRange(int numSlices)
{
    if(numSlices < 0) {
        if(imagesPreviouslyLoaded) {
            m_Controls.progressLabel->setVisible(false);
            m_Controls.processProgressBar->setVisible(false);
        }
        else 
            reconstructionDialog->setProgressBarVisible(false);
    }
    else {
        if(imagesPreviouslyLoaded) {
            m_Controls.processProgressBar->setVisible(true);
            m_Controls.processProgressBar->setMaximum(numSlices);
            m_Controls.processProgressBar->reset();
            m_Controls.progressLabel->setVisible(true);
        }
        else 
            reconstructionDialog->setMaximumBatchProgressBar(numSlices);      
    }
}

void QmitkAmosWorkbenchView::getProgress(int progress)
{
    if(imagesPreviouslyLoaded) 
        m_Controls.processProgressBar->setValue(progress);
    else
        reconstructionDialog->setProcessProgressBarValue(progress);
}

void QmitkAmosWorkbenchView::getProgressType(QString progressType)
{
    if(imagesPreviouslyLoaded) 
        m_Controls.progressLabel->setText(progressType);
    else
        reconstructionDialog->setProgressProcessType(progressType);
}

void QmitkAmosWorkbenchView::getCaseName(QString caseName)
{
    if(!imagesPreviouslyLoaded) {
        reconstructionDialog->setCaseName(caseName);
    }	
}

void QmitkAmosWorkbenchView::getCaseProgressRange(int numCases)
{
    numValidCases = numCases;
    if(!imagesPreviouslyLoaded)
        reconstructionDialog->setCaseProgressRange(numCases);
}

void QmitkAmosWorkbenchView::getCaseProgress(int caseNum)
{
    if(!imagesPreviouslyLoaded) 
        reconstructionDialog->setCaseProgress(caseNum);
}

void QmitkAmosWorkbenchView::showTime()
{     
    QString text = proccessTime.toString("hh:mm:ss:zzz");
    if(!imagesPreviouslyLoaded)         
        reconstructionDialog->displayTime(text);
    else
        displayTime(text);

    proccessTime = proccessTime.addMSecs(numMsecsClock);
    emit sendChronoInterval(numMsecsClock);
}

void QmitkAmosWorkbenchView::showTime(int mSecs)
{    
    proccessTime.setHMS(0, 0, 0, 0);
    proccessTime = proccessTime.addMSecs(mSecs);
    QString text = proccessTime.toString("hh:mm:ss:zzz");
    if(!imagesPreviouslyLoaded)         
        reconstructionDialog->displayTime(text);
    else
        displayTime(text);
}

void QmitkAmosWorkbenchView::setSegmentationResults(QList< QPair<Image::Pointer, QString> > segmentationResults)
{
    this->segmentationResults = segmentationResults;    
}

// TODO
void QmitkAmosWorkbenchView::OnSelectionChanged(mitk::DataNode* /*node*/)
{
    
}

void QmitkAmosWorkbenchView::OnFlairComboBoxSelectionChanged(int /*index*/)
{
    m_Controls.labelWarning->setVisible(!validImages());
}

void QmitkAmosWorkbenchView::OnT1ComboBoxSelectionChanged(int /*index*/)
{
    m_Controls.labelWarning->setVisible(!validImages());
}

void QmitkAmosWorkbenchView::OnLBComboBoxSelectionChanged(int /*index*/)
{
    m_Controls.labelWarning->setVisible(!validImages());
}

void QmitkAmosWorkbenchView::OnMaskComboBoxSelectionChanged(int /*index*/)
{
    m_Controls.labelWarning->setVisible(!validImages());
}

void QmitkAmosWorkbenchView::OnFlairEnhancedComboBoxSelectionChanged(int /*index*/)
{
    m_Controls.labelWarningEnhance->setVisible(!validImagesEnhance());
}

void QmitkAmosWorkbenchView::OnLBEnhancedComboBoxSelectionChanged(int /*index*/)
{
    m_Controls.labelWarningEnhance->setVisible(!validImagesEnhance());
}

void QmitkAmosWorkbenchView::OnMaskEnhancedComboBoxSelectionChanged(int /*index*/)
{
    m_Controls.labelWarningEnhance->setVisible(!validImagesEnhance());
}

void QmitkAmosWorkbenchView::enableParametersEdition(bool enable)
{
    m_Controls.spinBoxSeed->setEnabled(enable);
    m_Controls.spinBoxSeed2->setEnabled(enable);
    m_Controls.spinBoxTH2->setEnabled(enable);
    m_Controls.spinBoxTH4->setEnabled(enable);
    m_Controls.spinBoxErodeWM->setEnabled(enable);
    m_Controls.spinBoxWeightClass->setEnabled(enable);
}

void QmitkAmosWorkbenchView::enableParametersEditionEnhance(bool enable)
{
    m_Controls.spinBoxGradientWRad->setEnabled(enable);
    m_Controls.spinBoxMinimumLength->setEnabled(enable);
    m_Controls.spinBoxNmxRank->setEnabled(enable);
    m_Controls.spinBoxNmxConf->setEnabled(enable);
    m_Controls.comboBoxNmxType->setEnabled(enable);
    m_Controls.spinBoxHysterisisHighRank->setEnabled(enable);
    m_Controls.spinBoxHysterisisHighConf->setEnabled(enable);
    m_Controls.comboBoxHysterisisHighType->setEnabled(enable);
    m_Controls.widgetCustCurveHystHigh->setEnabled(enable && HystHighType == CURVE_PARAMS::CURVE_CUSTOM);   
    m_Controls.spinBoxHysterisisLowRank->setEnabled(enable);
    m_Controls.spinBoxHysterisisLowConf->setEnabled(enable);
    m_Controls.comboBoxHysterisisLowType->setEnabled(enable);
    m_Controls.widgetCustCurveHystLow->setEnabled(enable && HystLowType == CURVE_PARAMS::CURVE_CUSTOM);
    m_Controls.checkBoxEdgesMap->setEnabled(enable);
    m_Controls.checkBoxConfMap->setEnabled(enable);
    m_Controls.checkBoxGradMap->setEnabled(enable);
}

void QmitkAmosWorkbenchView::onMessageToUser(QString message)
{
    badCasesBox.show();
    badCasesBox.appendText(message);    
}

void QmitkAmosWorkbenchView::onProjectLoaded()
{
    project = projectDialog->getProject();
}

void QmitkAmosWorkbenchView::performReconstruction(ReconstructionDialog* reconstructionDialog)
{
    qDebug() << reconstructionDialog->getReconstruction()->getProgramId();
    
    if(reconstructionDialog->getReconstruction()->getProgramId() == "AmosSegmentation") 
        doBatchProcessing(reconstructionDialog);
    else if(reconstructionDialog->getReconstruction()->getProgramId() == "WMHStatistics") 
        calculateWMHStatistics(reconstructionDialog);
    else if(reconstructionDialog->getReconstruction()->getProgramId() == "AmosResampling") 
        amosResampling(reconstructionDialog);
    else if(reconstructionDialog->getReconstruction()->getProgramId() == "FreeSurferLabelling") 
        freesurferLabelling(reconstructionDialog);
    else if(reconstructionDialog->getReconstruction()->getProgramId() == "ContrastEnhancement") 
        doBatchContrastProcessing(reconstructionDialog);     
        
}

void QmitkAmosWorkbenchView::calculateWMHStatistics(ReconstructionDialog* /*reconstructionDialog*/)
{
    
}

void QmitkAmosWorkbenchView::runWMHStatistics()
{
    
}

void QmitkAmosWorkbenchView::stopWMHStatistics()
{
    
}

void QmitkAmosWorkbenchView::amosResampling(ReconstructionDialog* reconstructionDialog)
{    
    cout << "Executing AmosResampling" << endl;
    imagesPreviouslyLoaded = false;
    this->reconstructionDialog = reconstructionDialog;
    if(amosRunning) {
        stopAmosResampling();
        if(segmentationStopped)
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }

    subjectsAsMainDirs = project->getSubjectsAsMainDirs();
    reconstruction = reconstructionDialog->getReconstruction();
    
    slicer3DPath = reconstruction->getProgramPath();
        
    if(slicer3DPath.isEmpty()) {                
        QMap<QString, Reconstruction::Parameter> parameters = reconstruction->getParameters();
        if(parameters.contains("Slicer3D Path")) // for supporting old projects file
            slicer3DPath = parameters["Slicer3D Path"].value;    
        else if(parameters.contains("3DSlicer Path"))
            slicer3DPath = parameters["3DSlicer Path"].value;        
    }

    QFileInfo slicerExe(slicer3DPath, "Slicer");
    if(!slicerExe.exists()) {
        QString title(tr("Slicer"));
        QString text(tr("Slicer program not found"));
        QMessageBox::warning(0, title, text);
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }
    
    QMap<QString, Reconstruction::Parameter> reconsParameters = reconstruction->getParameters();
    doBiasCorrection = true;
    if(reconsParameters.contains("do bias correction")) 
        doBiasCorrection = reconsParameters["do bias correction"].value == "true" ? true : false;    
    LBResampling = true;
    if(reconsParameters.contains("LB resampling")) 
        LBResampling = reconsParameters["LB resampling"].value == "true" ? true : false;   
    
    QList<ReconstructionInputInfo> inputInfoImages = reconstruction->getInputInfoImages();
    AmosImage* flairAmosImage = 0;
    AmosImage* T1AmosImage = 0;
    AmosImage* labelAmosImage = 0;
    bool found;
    for(int i = 0; i < inputInfoImages.size(); i++) {
        found = false;
        ReconstructionInputInfo inputInfo = inputInfoImages.at(i);
        if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::FLAIR ||
                    AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::T1 ||
               AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::LABEL) {        
            QList<Reconstruction*>* reconsList = project->getReconstructions();
            Reconstruction* recon = 0;
            AmosImage* image = 0;
//             bool found = false;
            if(inputInfo.getReconstructionUuid() == project->getScansUuid()) {
                QList<AmosImage *> scansImages;
                scansImages.append(project->getScans()->getFlair());
                scansImages.append(project->getScans()->getT1());
                for(int k = 0; k < scansImages.size(); k++) {
                    image = scansImages.at(k);
                    if(image->getImageUuid() == inputInfo.getImageUuid()) {
                        found = true;
                        break;
                    }
                }
            }        
            if(inputInfo.getReconstructionUuid() == project->getExternUuid()) {
                QList<AmosImage *>* externImages = project->getExternImages();
                for(int k = 0; k < externImages->size(); k++) {
                    image = externImages->at(k);
                    if(image->getImageUuid() == inputInfo.getImageUuid()) {
                        found = true;
                        break;
                    }
                }
            }
            if(!found) {
                for(int j = 0; j < reconsList->size(); j++) {
                    recon = reconsList->at(j);
                    if(recon == reconstruction)
                        continue;
                    if(recon->getReconstructionUuid() != inputInfo.getReconstructionUuid())
                        continue;
                    QList<AmosImage *> imagesOut = recon->getImagesOut();
                    for(int k = 0; k < imagesOut.size(); k++) {
                        image = imagesOut.at(k); 
                        if(image->getImageUuid() == inputInfo.getImageUuid()) {
                            found = true;
                            break;
                        }
                  
                    }
                }                
            }
            if(found) {
                if(image->getType() == AmosImage::FLAIR)
                    flairAmosImage = image;
                else if(image->getType() == AmosImage::T1)
                    T1AmosImage = image;
                else if(image->getType() == AmosImage::LABEL)
                    labelAmosImage = image;                 
            }
        }        
     
    }
    
    found = LBResampling ? flairAmosImage && T1AmosImage && labelAmosImage : flairAmosImage && T1AmosImage;
//     if(!flairAmosImage || !T1AmosImage || !labelAmosImage) {
    if(!found) {
        QString title(tr("Images"));
        QString text(tr("Missing images."));
        QMessageBox::warning(0, title, text);
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }

    outputParentPath = project->getOutputPath();
    outputPath = reconstruction->getOutputPath();
    if(outputParentPath.endsWith(QDir::separator()))
        outputPath.remove(outputParentPath);
    else
        outputPath.remove(outputParentPath + QDir::separator());
    
    flairData.parentPath = flairAmosImage->getParentPath();
    if(subjectsAsMainDirs)
        flairData.path = flairAmosImage->getPath();
    flairData.name = flairAmosImage->getFileName();

    t1Data.parentPath = T1AmosImage->getParentPath();
    if(subjectsAsMainDirs)
        t1Data.path = T1AmosImage->getPath();
    t1Data.name = T1AmosImage->getFileName();
    
    if(LBResampling) {
        labelsData.parentPath = labelAmosImage->getParentPath();
        if(subjectsAsMainDirs)
            labelsData.path = labelAmosImage->getPath();
        labelsData.name = labelAmosImage->getFileName();
    }
   
    runAmosResampling();

}

void QmitkAmosWorkbenchView::runAmosResampling()
{
    cout << "AmosResampling started" << endl;
    
    QFileInfo fileInfo;
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn;
    segmentationStopped = false;
    
    QMap<QString, QString> parameters;
    parameters["do bias correction"] = doBiasCorrection ? "true" : "false";
    parameters["LB resampling"] = LBResampling ? "true" : "false";    
//     QMap<QString, Reconstruction::Parameter> reconsParameters = reconstruction->getParameters();
//     
//     bool doBiasCorrection = true;
//     if(reconsParameters.contains("do bias correction")) { 
//         doBiasCorrection = reconsParameters["do bias correction"].value == "true" ? true : false ;
//         parameters["do bias correction"] = reconsParameters["do bias correction"].value;        
//     }
//     
//     bool LBResampling = true;
//     if(reconsParameters.contains("LB resampling")) { 
//         LBResampling = reconsParameters["LB resampling"].value == "true" ? true : false ;
//         parameters["LB resampling"] = reconsParameters["LB resampling"].value;
//     }    
    
    QPair<Utils::ImageTypes, Utils::AmosImageData> pairF(Utils::FLAIR, flairData);
    fileInfo.setFile(flairData.parentPath);
    if(!fileInfo.exists()) {
        QMessageBox msgBox;
        msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
        msgBox.exec();
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;	
    }                  
    amosImagesListIn.append(pairF);    

    QPair<Utils::ImageTypes, Utils::AmosImageData> pairT(Utils::T1, t1Data);
    fileInfo.setFile(t1Data.parentPath);
    if(!fileInfo.exists()) {
        QMessageBox msgBox;
        msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
        msgBox.exec();
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;	
    }                  
    amosImagesListIn.append(pairT);
    
    if(LBResampling) {
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairL(Utils::LABEL, labelsData);
        fileInfo.setFile(labelsData.parentPath);
        if(!fileInfo.exists()) {
            QMessageBox msgBox;
            msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
            msgBox.exec();
            reconstructionDialog->processFinished(fullyProcessedSubjects);
            return;	
        }    
        amosImagesListIn.append(pairL);
    }
    
    Utils::AmosImageData FOutData;
    Utils::AmosImageData TOutData;
    Utils::AmosImageData LOutData;
    QList<AmosImage *> outputImages = reconstruction->getImagesOut();
    
    for(int i = 0; i < outputImages.size(); i++) {
        if(outputImages.at(i)->getType() == AmosImage::FLAIR_RECONS) 
            FOutData.name = outputImages.at(i)->getFileName();
        else if(outputImages.at(i)->getType() == AmosImage::T1_RECONS) 
            TOutData.name = outputImages.at(i)->getFileName();
        else if(outputImages.at(i)->getType() == AmosImage::LABEL_RECONS) 
            LOutData.name = outputImages.at(i)->getFileName();        
    }
    
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListOut;
    if(doBiasCorrection) {
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairFOut(Utils::FLAIR, FOutData);
        amosImagesListOut.append(pairFOut);
    }
    QPair<Utils::ImageTypes, Utils::AmosImageData> pairTOut(Utils::T1, TOutData);
    amosImagesListOut.append(pairTOut);
    if(LBResampling) {
        QPair<Utils::ImageTypes, Utils::AmosImageData> pairLOut(Utils::LABEL, LOutData);    
        amosImagesListOut.append(pairLOut);
    }

    QThread* segmentationThread = new QThread;
    launcher = new ResamplingSlicerLauncher();        
    launcher->moveToThread(segmentationThread);
    
    QStringList selectedSubjects = reconstructionDialog->getSelectedSubjects();
    numThreads = reconstructionDialog->getNumberOfThreads();
    numSubjects = min(selectedSubjects.size(), reconstructionDialog->getNumberOfSubjects());    
//     numSubjects = reconstructionDialog->getNumberOfSubjects();
    launcher->setNumThreads(numThreads);
    launcher->setNumSimultaneousSubjects(numSubjects);
    launcher->setSubjectsAsMainDirs(subjectsAsMainDirs);
    launcher->setProgram(slicer3DPath);
    launcher->setParameters(parameters);  
    
    connect(launcher, SIGNAL(sendLog(QString)), this, SLOT(appendTextLog(QString)));
    
    connect(launcher, SIGNAL(sendProgressRange(int)), this, SLOT(getProgressRange(int)));
    connect(launcher, SIGNAL(sendProgress(int)), this, SLOT(getProgress(int)));
    connect(launcher, SIGNAL(sendProgressType(QString)), this, SLOT(getProgressType(QString)));
    
    connect(launcher, SIGNAL(sendCaseName(QString)), this, SLOT(getCaseName(QString)));
    connect(launcher, SIGNAL(sendCaseProgressRange(int)), this, SLOT(getCaseProgressRange(int)));
    connect(launcher, SIGNAL(sendCaseProgress(int)), this, SLOT(getCaseProgress(int)));
    connect(launcher, SIGNAL(sendTime(int)), this, SLOT(showTime(int)));
    connect(launcher, SIGNAL(sendMessageToUser(QString)), this, SLOT(onMessageToUser(QString)));
    
    connect(segmentationThread, SIGNAL (started()), launcher, SLOT (processCases()));
    connect(launcher, SIGNAL (processFinished()), segmentationThread, SLOT (quit()));
    connect(launcher, SIGNAL (processFinished()), launcher, SLOT (deleteLater()));
    connect(launcher, SIGNAL (processFinished()), this, SLOT (amosResamplingFinished()));
    connect(launcher, SIGNAL (processedSubject(QStringList)), this, SLOT (setFullyProcessedSubjects(QStringList)));
    connect(this, SIGNAL (sendChronoInterval(uint)), launcher, SLOT (addTimeToChrono(uint)));

    connect(segmentationThread, SIGNAL (finished()), segmentationThread, SLOT (deleteLater()));
    
    proccessTime.setHMS(0, 0, 0, 0);
    showTime();
    
//     QStringList selectedSubjects = reconstructionDialog->getSelectedSubjects();
//     QStringList selectedSubjects;
//     QMapIterator<QString, bool> it(reconstruction->getSubjects());
//     while(it.hasNext()) {
//         it.next();
//         if(it.value())
//             selectedSubjects.append(it.key());
//     }
    launcher->setSubjects(selectedSubjects);
    bool ret = launcher->setAmosImagesData(amosImagesListIn, outputParentPath, outputPath, amosImagesListOut);
    
    if(ret) {
        timer->start();		
        segmentationThread->start();
        amosRunning = true;
        projectDialog->conditionalEnableRun(false);  
//         if(selectedSubjects.size() == 1)
//             reconstructionDialog->setProgressBarVisible(false);
    }
    else {
        QMessageBox msgBox;
        msgBox.setText(tr("Images problems"));
        msgBox.exec();
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;	
    }      
}

void QmitkAmosWorkbenchView::amosResamplingFinished()
{
    if(launcher /*&& amosRunning*/) {
        timer->stop();
        launcher = 0;
        amosRunning = false;
        projectDialog->conditionalEnableRun(true);
        reconstructionDialog->setCaseName(tr("Case"));
        reconstructionDialog->setProgressProcessType(tr("Finished"));
        reconstructionDialog->setMaximumBatchProgressBar(1);
        reconstructionDialog->setProcessProgressBarValue(1); // 100%

        QMessageBox msgBox;
        msgBox.setText(tr("Resampling has finished"));
        msgBox.exec();

        reconstructionDialog->processFinished(fullyProcessedSubjects);
        reconstructionDialog->setProgressBarVisible(true);
        segmentationStopped = false;
        return;	
    }
}    

void QmitkAmosWorkbenchView::stopAmosResampling()
{
    cout << "AmosResampling finished" << endl;
    
    if(!launcher || !amosRunning) 
        return;
    
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to stop the resampling proccess?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int result = msgBox.exec();
    if(result == QMessageBox::Yes) {
        launcher->stopProcess();
        segmentationStopped = true;
    }
    
    return;    
}

void QmitkAmosWorkbenchView::freesurferLabelling(ReconstructionDialog* reconstructionDialog)
{    
    cout << "Executing FreeSurferLabelling" << endl;
    imagesPreviouslyLoaded = false;
    this->reconstructionDialog = reconstructionDialog;
    if(amosRunning) {
        stopFreesurferLabelling();
        if(segmentationStopped)
            reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }

    subjectsAsMainDirs = project->getSubjectsAsMainDirs();
    reconstruction = reconstructionDialog->getReconstruction();
    freesurferPath = reconstruction->getProgramPath();
    QFileInfo reconAllExe(freesurferPath, "bin/recon-all");
    if(!reconAllExe.exists()) {
        QString title(tr("FreeSurfer"));
        QString text(tr("FreeSurfer recon-all command not found"));
        QMessageBox::warning(0, title, text);
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }    
    
    QList<ReconstructionInputInfo> inputInfoImages = reconstruction->getInputInfoImages();
    AmosImage* T1AmosImage = 0;
    for(int i = 0; i < inputInfoImages.size(); i++) {
        ReconstructionInputInfo inputInfo = inputInfoImages.at(i);
        if(AmosImage::getTypeFromName(inputInfo.getImageType()) == AmosImage::T1) {        
            QList<Reconstruction*>* reconsList = project->getReconstructions();
            Reconstruction* recon = 0;
            AmosImage* image = 0;
            bool found = false;
            if(inputInfo.getReconstructionUuid() == project->getScansUuid()) {
                QList<AmosImage *> scansImages;
                scansImages.append(project->getScans()->getT1());
                for(int k = 0; k < scansImages.size(); k++) {
                    image = scansImages.at(k);
                    if(image->getImageUuid() == inputInfo.getImageUuid()) {
                        found = true;
                        break;
                    }
                }
            }        
//             if(inputInfo.getReconstructionUuid() == project->getExternUuid()) {
//                 QList<AmosImage *>* externImages = project->getExternImages();
//                 for(int k = 0; k < externImages->size(); k++) {
//                     image = externImages->at(k);
//                     if(image->getImageUuid() == inputInfo.getImageUuid()) {
//                         found = true;
//                         break;
//                     }
//                 }
//             }
            if(!found) {
                for(int j = 0; j < reconsList->size(); j++) {
                    recon = reconsList->at(j);
                    if(recon == reconstruction)
                        continue;
                    if(recon->getReconstructionUuid() != inputInfo.getReconstructionUuid())
                        continue;
                    QList<AmosImage *> imagesOut = recon->getImagesOut();
                    for(int k = 0; k < imagesOut.size(); k++) {
                        image = imagesOut.at(k); 
                        if(image->getImageUuid() == inputInfo.getImageUuid()) {
                            found = true;
                            break;
                        }
                  
                    }
                }                
            }
            if(found) {
                if(image->getType() == AmosImage::T1)
                    T1AmosImage = image;                
            }
        }        
     
    }
    
    if(!T1AmosImage) {
        QString title(tr("Images"));
        QString text(tr("Missing images."));
        QMessageBox::warning(0, title, text);
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;
    }

    outputParentPath = project->getOutputPath();
    outputPath = reconstruction->getOutputPath();
    if(outputParentPath.endsWith(QDir::separator()))
        outputPath.remove(outputParentPath);
    else
        outputPath.remove(outputParentPath + QDir::separator());
    
//     flairData.parentPath = flairAmosImage->getParentPath();
//     if(subjectsAsMainDirs)
//         flairData.path = flairAmosImage->getPath();
//     flairData.name = flairAmosImage->getFileName();

    t1Data.parentPath = T1AmosImage->getParentPath();
    if(subjectsAsMainDirs)
        t1Data.path = T1AmosImage->getPath();
    t1Data.name = T1AmosImage->getFileName();
    
//     labelsData.parentPath = labelAmosImage->getParentPath();
//     if(subjectsAsMainDirs)
//         labelsData.path = labelAmosImage->getPath();
//     labelsData.name = labelAmosImage->getFileName();   
   
    runFreesurferLabelling();

}

void QmitkAmosWorkbenchView::runFreesurferLabelling()
{
    cout << "FreeSurferLabelling started" << endl;
    
    QFileInfo fileInfo;
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn;
    segmentationStopped = false;
    
//     QPair<Utils::ImageTypes, Utils::AmosImageData> pairF(Utils::FLAIR, flairData);
//     fileInfo.setFile(flairData.parentPath);
//     if(!fileInfo.exists()) {
//         QMessageBox msgBox;
//         msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
//         msgBox.exec();
//         reconstructionDialog->processFinished(fullyProcessedSubjects);
//         return;	
//     }                  
//     amosImagesListIn.append(pairF);    

    QPair<Utils::ImageTypes, Utils::AmosImageData> pairT(Utils::T1, t1Data);
    fileInfo.setFile(t1Data.parentPath);
    if(!fileInfo.exists()) {
        QMessageBox msgBox;
        msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
        msgBox.exec();
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;	
    }                  
    amosImagesListIn.append(pairT);
    
//     QPair<Utils::ImageTypes, Utils::AmosImageData> pairL(Utils::LABEL, labelsData);
//     fileInfo.setFile(labelsData.parentPath);
//     if(!fileInfo.exists()) {
//         QMessageBox msgBox;
//         msgBox.setText(fileInfo.absoluteFilePath() + " " + tr("does not exist"));
//         msgBox.exec();
//         reconstructionDialog->processFinished(fullyProcessedSubjects);
//         return;	
//     }    
//     amosImagesListIn.append(pairL);
    
//     Utils::AmosImageData FOutData;
//     Utils::AmosImageData TOutData;
    Utils::AmosImageData LOutData;
    QList<AmosImage *> outputImages = reconstruction->getImagesOut();
    
    for(int i = 0; i < outputImages.size(); i++) {
/*        if(outputImages.at(i)->getType() == AmosImage::FLAIR_RECONS) 
            FOutData.name = outputImages.at(i)->getFileName();
        else if(outputImages.at(i)->getType() == AmosImage::T1_RECONS) 
            TOutData.name = outputImages.at(i)->getFileName();
        else if(outputImages.at(i)->getType() == AmosImage::LABEL_RECONS) 
            LOutData.name = outputImages.at(i)->getFileName();*/
        if(outputImages.at(i)->getType() == AmosImage::LABEL) 
            LOutData.name = outputImages.at(i)->getFileName();
    }
    
//     QPair<Utils::ImageTypes, Utils::AmosImageData> pairFOut(Utils::FLAIR, FOutData);
//     QPair<Utils::ImageTypes, Utils::AmosImageData> pairTOut(Utils::T1, TOutData);
    QPair<Utils::ImageTypes, Utils::AmosImageData> pairLOut(Utils::LABEL, LOutData);
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListOut;
//     amosImagesListOut.append(pairFOut);
//     amosImagesListOut.append(pairTOut);
    amosImagesListOut.append(pairLOut);

    QThread* segmentationThread = new QThread;
    launcher = new LabellingFreeSurferLauncher();
    numThreads = reconstructionDialog->getNumberOfThreads();
    QStringList selectedSubjects;
    selectedSubjects = reconstructionDialog->getSelectedSubjects();
    numThreads = reconstructionDialog->getNumberOfThreads();
    numSubjects = min(selectedSubjects.size(), reconstructionDialog->getNumberOfSubjects());    
//     numSubjects = reconstructionDialog->getNumberOfSubjects();
    launcher->setNumThreads(numThreads);
    launcher->setNumSimultaneousSubjects(numSubjects);    
    launcher->moveToThread(segmentationThread);
    launcher->setSubjectsAsMainDirs(subjectsAsMainDirs);
    launcher->setProgram(reconstruction->getProgramPath());
    
    QMap<QString, QString> parameters;
    parameters["FSPath"] = freesurferPath;
    QMap<QString, Reconstruction::Parameter> reconsParameters = reconstruction->getParameters();
    if(reconsParameters.contains("delay")) {
        uint delay = 0;
        Reconstruction::Parameter param = reconsParameters.value("delay");
        if(param.prefix == "second")
            delay = param.value.toInt();
        else if(param.prefix == "minute")
            delay = param.value.toInt() * 60;
        parameters["delay"] = QString::number(delay);
    }     
    
    parameters["path4FSOutput"] = project->getOutputPath();

    if(reconsParameters.contains("only convert"))
        parameters["only convert"] = reconsParameters["only convert"].value;
    
    if(reconsParameters.contains("FS out inside subjects"))
        parameters["FS out inside subjects"] = reconsParameters["FS out inside subjects"].value;     
    
    launcher->setParameters(parameters);  
    
    connect(launcher, SIGNAL(sendLog(QString)), this, SLOT(appendTextLog(QString)));
    
    connect(launcher, SIGNAL(sendProgressRange(int)), this, SLOT(getProgressRange(int)));
    connect(launcher, SIGNAL(sendProgress(int)), this, SLOT(getProgress(int)));
    connect(launcher, SIGNAL(sendProgressType(QString)), this, SLOT(getProgressType(QString)));
    
    connect(launcher, SIGNAL(sendCaseName(QString)), this, SLOT(getCaseName(QString)));
    connect(launcher, SIGNAL(sendCaseProgressRange(int)), this, SLOT(getCaseProgressRange(int)));
    connect(launcher, SIGNAL(sendCaseProgress(int)), this, SLOT(getCaseProgress(int)));
    connect(launcher, SIGNAL(sendTime(int)), this, SLOT(showTime(int)));
    connect(launcher, SIGNAL(sendMessageToUser(QString)), this, SLOT(onMessageToUser(QString)));
    
    connect(segmentationThread, SIGNAL (started()), launcher, SLOT (processCases()));
    connect(launcher, SIGNAL (processFinished()), segmentationThread, SLOT (quit()));
    connect(launcher, SIGNAL (processFinished()), launcher, SLOT (deleteLater()));
    connect(launcher, SIGNAL (processFinished()), this, SLOT (freesurferLabellingFinished()));
    connect(launcher, SIGNAL (processedSubject(QStringList)), this, SLOT (setFullyProcessedSubjects(QStringList)));
    connect(this, SIGNAL (sendChronoInterval(uint)), launcher, SLOT (addTimeToChrono(uint))); 

    connect(segmentationThread, SIGNAL (finished()), segmentationThread, SLOT (deleteLater()));
    
    proccessTime.setHMS(0, 0, 0, 0);
    showTime();
    
//     QStringList selectedSubjects = reconstructionDialog->getSelectedSubjects();
//     QStringList selectedSubjects;
//     QMapIterator<QString, bool> it(reconstruction->getSubjects());
//     while(it.hasNext()) {
//         it.next();
//         if(it.value())
//             selectedSubjects.append(it.key());
//     }
    launcher->setSubjects(selectedSubjects);
    bool ret = launcher->setAmosImagesData(amosImagesListIn, outputParentPath, outputPath, amosImagesListOut);
    currentPath = QDir::currentPath();
    
    if(ret) {
        timer->start();		
        segmentationThread->start();
        amosRunning = true;
        projectDialog->conditionalEnableRun(false);  
//         if(selectedSubjects.size() == 1)
//             reconstructionDialog->setProgressBarVisible(false);
    }
    else {
        QMessageBox msgBox;
        msgBox.setText(tr("Images problems"));
        msgBox.exec();
        reconstructionDialog->processFinished(fullyProcessedSubjects);
        return;	
    }      
}

void QmitkAmosWorkbenchView::freesurferLabellingFinished()
{
    if(launcher /*&& amosRunning*/) {
        timer->stop();
        launcher = 0;
        amosRunning = false;
        projectDialog->conditionalEnableRun(true);
        reconstructionDialog->setCaseName(tr("Case"));

        QMessageBox msgBox;
        msgBox.setText(tr("Labelling has finished"));
        msgBox.exec();

        reconstructionDialog->processFinished(fullyProcessedSubjects);
        reconstructionDialog->setProgressBarVisible(true);
        segmentationStopped = false;
        QDir::setCurrent(currentPath);
        return;	
    }
}    

void QmitkAmosWorkbenchView::stopFreesurferLabelling()
{
    cout << "FreeSurferLabelling finished" << endl;
    
    if(!launcher || !amosRunning) 
        return;
    
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to stop the labelling proccess?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int result = msgBox.exec();
    if(result == QMessageBox::Yes) {
        launcher->stopProcess();
        segmentationStopped = true;
    }
    
    return;    
}

void QmitkAmosWorkbenchView::setFullyProcessedSubjects(QStringList fullyProcessedSubjects)
{
    this->fullyProcessedSubjects = fullyProcessedSubjects;
}

void QmitkAmosWorkbenchView::displayTime(QString time)
{
    m_Controls.clockLcdNumber->display(time);
}

void QmitkAmosWorkbenchView::onSigmaSeedSet(double val)
{
    NSIGMADESVseed = val;
}

void QmitkAmosWorkbenchView::onSigmaSeed2Set(double val)
{
    NSIGMADESVseed2 = val;
}

void QmitkAmosWorkbenchView::onSigmaTH2Set(double val)
{
    NSIGMADESV_TH2 = val;
}

void QmitkAmosWorkbenchView::onSigmaTH4Set(double val)
{
    NSIGMADESV_TH4 = val;
}

void QmitkAmosWorkbenchView::onErodeWMSet(int val)
{
    ERODEWM = val;
}

void QmitkAmosWorkbenchView::onWeightClassSet(double val)
{
    WEIGHT_CLASSIFIER_FL = val;
}

void QmitkAmosWorkbenchView::onGradientWinRadSet(int val)
{
    GradientWinRad = val;
}

void QmitkAmosWorkbenchView::onMinimumLengthSet(int val)
{
    MinimumLength = val;
}

void QmitkAmosWorkbenchView::onNmxRankSet(double val)
{
    NmxRank = val;
}

void QmitkAmosWorkbenchView::onNmxConfSet(double val)
{
    NmxConf = val;
}

void QmitkAmosWorkbenchView::onNmxTypeSet(int val)
{
    NmxType = val;
}
	
void QmitkAmosWorkbenchView::onHystHighRankSet(double val)
{
    HystHighRank = val;
}

void QmitkAmosWorkbenchView::onHystHighConfSet(double val)
{
    HystHighConf = val;
}

void QmitkAmosWorkbenchView::onHystHighTypeSet(int val)
{
    HystHighType = val;
    m_Controls.widgetCustCurveHystHigh->setEnabled(val == CURVE_PARAMS::CURVE_CUSTOM);
}

void QmitkAmosWorkbenchView::onCustCurveHystHigh_00Set(double val)
{
    CustCurveHystHigh_00 = val;
}

void QmitkAmosWorkbenchView::onCustCurveHystHigh_01Set(double val)
{
    CustCurveHystHigh_01 = val;
}

void QmitkAmosWorkbenchView::onCustCurveHystHigh_10Set(double val)
{
    CustCurveHystHigh_10 = val;
}

void QmitkAmosWorkbenchView::onCustCurveHystHigh_11Set(double val)
{
    CustCurveHystHigh_11 = val;
}


void QmitkAmosWorkbenchView::onHystLowRankSet(double val)
{
    HystLowRank = val;
}

void QmitkAmosWorkbenchView::onHystLowConfSet(double val)
{
    HystLowConf = val;
}

void QmitkAmosWorkbenchView::onHystLowTypeSet(int val)
{
    HystLowType = val;
    m_Controls.widgetCustCurveHystLow->setEnabled(val == CURVE_PARAMS::CURVE_CUSTOM);
}
	
void QmitkAmosWorkbenchView::onCustCurveHystLow_00Set(double val)
{
    CustCurveHystLow_00 = val;
}

void QmitkAmosWorkbenchView::onCustCurveHystLow_01Set(double val)
{
    CustCurveHystLow_01 = val;
}

void QmitkAmosWorkbenchView::onCustCurveHystLow_10Set(double val)
{
    CustCurveHystLow_10 = val;
}

void QmitkAmosWorkbenchView::onCustCurveHystLow_11Set(double val)
{
    CustCurveHystLow_11 = val;
}

void QmitkAmosWorkbenchView::onSaveEdgesMapSet(bool val)
{
    saveEdgesMap = val;
}

void QmitkAmosWorkbenchView::onSaveConfidenceMapSet(bool val)
{
    saveConfidenceMap = val;
}

void QmitkAmosWorkbenchView::onSaveGradientMapSet(bool val)
{
    saveGradientMap = val;
}

void QmitkAmosWorkbenchView::tabChanged(int index)
{
//     if(index == 0 || index == 3)
//         m_Controls.runGroupBox->setVisible(true);
//     else
//         m_Controls.runGroupBox->setVisible(false);
    if(m_Controls.tabWidget->tabText(index) == "Advanced" || m_Controls.tabWidget->tabText(index) == "Database")
        m_Controls.runGroupBox->setVisible(false);
    else
        m_Controls.runGroupBox->setVisible(true);
}

void QmitkAmosWorkbenchView::extractExternalFiles()
{
    // Copy external files from resources to app path
    //In amosprocess.cpp classifier->setModel() calls svm_load_model() that uses fopen and not manage Qt resource files
   copyFromResources(":/segmentation/svmtest2D.model");
   copyFromResources(":/segmentation/svmtest2D.scale");
   copyFromResources(":/segmentation/svmtest3D.model");
   copyFromResources(":/segmentation/svmtest3D.scale");   
}

bool QmitkAmosWorkbenchView::copyFromResources(QString fileName)
{
    QFileInfo fileInfo(fileName);
    if(!fileInfo.exists()) {
        QString title(tr("External files"));
        QString text(tr("Missing ") + fileInfo.fileName());
        QMessageBox::warning(0, title, text);
        return false;
    }
    QFileInfo fileInfo2(fileInfo.fileName());
    bool copyFile = true;
    if(fileInfo2.exists()) {
        copyFile = fileInfo.lastModified() > fileInfo2.lastModified();
        if(copyFile)
            QFile::remove(fileInfo.fileName());
    }
    if(copyFile)
        QFile::copy(fileName, fileInfo.fileName());
    fileName = fileInfo.fileName();
    
    return true;
}
