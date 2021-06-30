#include "amosprocesscase.h"
// #include "amosniftiimageio.h" // this include before #include "utils.hpp" in order to avoid conflict between mat44 in armadillo and nifti_io
#include "utils.hpp"
#include "levenbergmarquardt.hpp"
#include "regionproperties.h"
#include "svmclassifier.h"
#include "amositkimageio.h"

#include "tests.hpp"

#include <QTextStream>
#include <QTimer>
#include <QDateTime>
#include <mitkProperties.h>

// us
#include <usModule.h>
#include <usModuleResource.h>
#include <usGetModuleContext.h>
#include <usModuleContext.h>

/**
 * @brief Constructor, initializes pointers to 0
 * 
 */

AmosProcessCase::EditableParameters defaultParameters = {
    4.0,    // NSIGMADESVseed
    2.40,   // NSIGMADESVseed2
    1.80,   // NSIGMADESV_TH2
    1.20,   // NSIGMADESV_TH4
    0,      // ERODEWM
    1.0     // WEIGHT_CLASSIFIER_FL
};

AmosProcessCase::AmosProcessCase(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D) : InternalProcessCase(modeAutomatic, imagesPreviouslyLoaded, mode3D)
{
   
    FL3D = 0;
    orgFL3D = 0;
    orgT13D = 0;    
    orgLB3D = 0;
    orgANOT3D = 0;
    orgMASK3D = 0;
    
    LB3DConverted = false;
    
    NBINS = 128;
    NPOINTSGAUSSIANAPROX = 29;
    NSIGMADESVseed = defaultParameters.NSIGMADESVseed;
    NSIGMADESVseed2 = defaultParameters.NSIGMADESVseed2;
    NSIGMADESVseed2Max = 100;
    NSIGMADESV_TH2 = defaultParameters.NSIGMADESV_TH2;
    NSIGMADESV_TH4 = defaultParameters.NSIGMADESV_TH4;
    ERODEWM = defaultParameters.ERODEWM;
    WEIGHT_CLASSIFIER_FL = defaultParameters.WEIGHT_CLASSIFIER_FL;
    WMDisk10Diameterin = 5;
    LBWMDiskDiameterin = 2;    
    MINWMLSIZE = 0;    
    EXCLUSIONZONEin = 1;
    R_PERIPHERYin = 2;
    
    numSlicesWithLBWM3DMinSize = 0;
    initialLBWM3DMinSize = 15000;
    LBWM3DMinSize = initialLBWM3DMinSize;
    
    // this number was the xy-spacing in the first images set
    LBWM3DMinSizeFactor = 0.4492;
    K_noiseLevelFindLimit = 1.2;
    K_noiseLevelDefault = 2.7;
    TH13D = 0;
    
    histoCenters = linspace<fvec>(0.0, 1.0, NBINS);
    posH = linspace<fvec>(0.5/NBINS, 1.0-0.5/NBINS,NBINS);
    
    featuresNumber = 0;
    
    geometryFL = 0;
    orgGeometryFL = 0;
    geometryFLChanged = false;
   
    LBWM3DMinSize = (uint) round(initialLBWM3DMinSize * LBWM3DMinSizeFactor * LBWM3DMinSizeFactor/(outSpacing3D[0] * outSpacing3D[1]));

    // cout << QString::number(numCase).toStdString()  + " AmosProcessCase created" << endl;
}

/**
 * @brief Destructor, frees memory
 * 
 */
AmosProcessCase::~AmosProcessCase()
{
    for(int i = 0; i < regionProps.size(); i++)
        delete regionProps[i];
    regionProps.clear();
    delete classifier;
    
    cout << QString::number(numCase).toStdString()  + " AmosProcessCase deleted" << endl;
}

void AmosProcessCase::clearMemory() {
    InternalProcessCase::clearMemory();
    
    FL3Dcube.reset();
    FL3DROIcube.reset();
    T13Dcube.reset();   
    LB3Dcube.reset();
    ANOT3Dcube.reset();
    
    WMMask3DModelCube.reset();
    WMMask3DROICube.reset();
    Imateria.reset();
    Iseg3D_BC.reset();
    newIseg3D_BC.reset();
    Iseg3D_AC.reset();
    newIseg3D_AC.reset();
    
    Iseg3D_AC_blobs.reset();
    Iseg3D_BC_blobs.reset();
    DistToCHULL3D.reset();
    //DistToBorder3D.reset();   //mari
    DistToWMBorder3D.reset(); 
    DistToWMCM.reset();
    Iroi3D.reset();
    Ich3D.reset(); 
    WMmask3DROIBinCube.reset();
    WMmask3DModelBinCube.reset();    
    
    
    // NUEVOS
    WMMask3DModelCube.reset();
    WMMask3DROICube.reset(); 
    
    WMCM.reset();
    
    Imateria.reset();
    Iseg3D_BC.reset();
    newIseg3D_BC.reset();
    Iseg3D_AC.reset();
    newIseg3D_AC.reset();
        
    //fin nuevos
    
    orgFL3D = NULL;
    orgT13D = NULL;    
    orgLB3D = NULL;
    orgANOT3D = NULL;
    orgMASK3D = NULL;    
    
    FL3D = NULL;
    
}

void AmosProcessCase::initHeap() // call always after constructor for avoiding create objects in it due to threads
{

    for(int i = 0; i < numThreads; i++)
        regionProps.append(new RegionProperties());

    classifier = new SVMClassifier;
}

void AmosProcessCase::setEditableParameters(EditableParameters parameters)
{
    this->NSIGMADESVseed = parameters.NSIGMADESVseed;
    this->NSIGMADESVseed2 = parameters.NSIGMADESVseed2;
    this->NSIGMADESV_TH2 = parameters.NSIGMADESV_TH2;
    this->NSIGMADESV_TH4 = parameters.NSIGMADESV_TH4;
    this->ERODEWM = parameters.ERODEWM;
    this->WEIGHT_CLASSIFIER_FL = parameters.WEIGHT_CLASSIFIER_FL;    
}

AmosProcessCase::EditableParameters AmosProcessCase::getDefaultEditableParameters()
{
    return defaultParameters;
}

// void AmosProcessCase::setNumThreads(int numThreads)
// {
//     this->numThreads = numThreads;
// }

void AmosProcessCase::setSVMFiles(QString modelFile, QString scaleFile, int featuresNumber)
{
    if(!modelFile.isEmpty()) { 
        classifier->setModel(modelFile.toLatin1().constData());
        SVMScaleFile= scaleFile;
        readScaleFile();
        this->featuresNumber = minmaxFeatures.size();
    }
    else {
        this->featuresNumber = featuresNumber;
        featuresClasses.append(1);
    }
}

bool AmosProcessCase::setOutputDirectories(QString parentDirOut, QString pathOut)
{
    if(!InternalProcessCase::setOutputDirectories(parentDirOut, pathOut))
        return false;
    
    if(modeAutomatic)
        return true;
    
    objectsFeaturesDir.setFile(parentDirOut, QString("Objects_features"));

    if(!objectsFeaturesDir.exists()) {
        QDir dir;
        if(!dir.mkdir(objectsFeaturesDir.absoluteFilePath())) {
            emit sendLog("Can not create results directory " + objectsFeaturesDir.absoluteFilePath());
            return false;
        }
    }
    
    return true;
}

bool AmosProcessCase::loadImages(QList<Image::Pointer> imagesMITK)
{
    orgFL3D = imagesMITK.at(0);
    orgT13D = imagesMITK.at(1);
    orgLB3D = imagesMITK.at(2);
    orgMASK3D = imagesMITK.at(3);
    orgANOT3D = 0;
    if(!modeAutomatic) {
        orgANOT3D = imagesMITK.at(4);
        if(!orgANOT3D) {
            return false;
        }
    }
    if(orgMASK3D)
        convertProgressRange++;
        // MASK not included in listTypes because it is not processed in convertImages()
        // it is processed directly in convertImageMaskAndLB()
    if(orgANOT3D) {
        convertProgressRange++;
        listTypes << Utils::EXPERT;
    }
    return true;
}

/**
 * @brief Get the 3Dimages associated to the case and creates the cubes associated to their data
 * 
 * @param FL3D Flair image
 * @param T13D T1 image
 * @param LB3D Labels image
 * @param ANOT3D Expert annotated image
 * @param SLICE_INI_LOOP optional, starting slice
 * @param SLICE_END_LOOP optional, ending slice
 * @return bool, true if everything could be created, false otherwise
 */

bool AmosProcessCase::verifySizes()
{
    
    unsigned int dimFL = orgFL3D->GetDimension();
    unsigned int dimT1 = orgT13D->GetDimension();
    unsigned int dimLB = orgLB3D->GetDimension();
    unsigned int dimANOT = orgANOT3D ? orgANOT3D->GetDimension() : 3;
    unsigned int dimMASK = orgMASK3D ? orgMASK3D->GetDimension() : 3;
    if(dimFL != 3 || dimT1 != 3 ||  dimLB != 3 || dimANOT != 3 || dimMASK != 3) {
        emit sendLog(QString::fromLatin1("Images are not 3D"));
        //         cout  << "Images are not 3D" << endl;
        return false;
    }
    
    unsigned int sizesFL[3];
    unsigned int sizesT1[3];
    unsigned int sizesLB[3];
    unsigned int sizesANOT[3];
    unsigned int sizesMASK[3];
    dimImg3D = (unsigned int*) malloc(sizeof(unsigned int) * 3);
    
    for(int i = 0; i < 3; i++) {
        sizesFL[i] = orgFL3D->GetDimension(i);
        sizesT1[i] = orgT13D->GetDimension(i);
        sizesLB[i] = orgLB3D->GetDimension(i);
        sizesANOT[i] = orgANOT3D ? orgANOT3D->GetDimension(i) : orgFL3D->GetDimension(i);
        sizesMASK[i] = orgMASK3D ? orgMASK3D->GetDimension(i) : orgFL3D->GetDimension(i);
        if(sizesFL[i] != sizesT1[i] || sizesFL[i] != sizesLB[i] || sizesFL[i] != sizesANOT[i] || sizesFL[i] != sizesMASK[i]) {
            emit sendLog(QString::fromLatin1("Images have not the same sizes"));
            //             cout  << "Images have not the same sizes" << endl;
            return false;
        }
        dimImg3D[i] = sizesFL[i];
    }
    
    return true;
    
}

bool AmosProcessCase::calculateOrientation()
{
    geometryFL = orgFL3D->GetGeometry();
    if(modeAutomatic && !imagesPreviouslyLoaded) {
        Amos_AC_Image = Image::New();
        Amos_BC_Image = Image::New();
        Amos_AC_Image->Initialize(mitk::MakeScalarPixelType<uchar>(), *geometryFL);
        Amos_BC_Image->Initialize(mitk::MakeScalarPixelType<uchar>(), *geometryFL);
    }
    AffineTransform3D* transform = geometryFL->GetIndexToWorldTransform();
    itk::Matrix<ScalarType> qformFL = transform->GetMatrix();
    
    BaseGeometry* geometry = orgT13D->GetGeometry();
    transform = geometry->GetIndexToWorldTransform();
    itk::Matrix<ScalarType> qformT1 = transform->GetMatrix();
    
    geometry = orgLB3D->GetGeometry();
    transform = geometry->GetIndexToWorldTransform();
    itk::Matrix<ScalarType> qformLB = transform->GetMatrix();
    
    itk::Matrix<ScalarType> qformMASK;
    if(orgMASK3D) {
        geometry = orgMASK3D->GetGeometry();
        transform = geometry->GetIndexToWorldTransform();
        qformMASK = transform->GetMatrix();
    }
    else
        qformMASK = qformFL;
    
    itk::Matrix<ScalarType> qformANOT;
    if(orgANOT3D) {
        geometry = orgANOT3D->GetGeometry();
        transform = geometry->GetIndexToWorldTransform();
        qformANOT = transform->GetMatrix();
    }
    else
        qformANOT = qformFL;    
    // if(!(qformFL == qformT1 && qformFL == qformLB && qformFL == qformMASK && qformFL == qformANOT))
    if(!(Utils::aproxEqualITKMatrix(qformFL, qformT1, Utils::epsQform) && Utils::aproxEqualITKMatrix(qformFL, qformLB, Utils::epsQform) && Utils::aproxEqualITKMatrix(qformFL, qformMASK, Utils::epsQform) && Utils::aproxEqualITKMatrix(qformFL, qformANOT, Utils::epsQform))) {
        // 	// emit sendLog(subjectName + " images qforms are different");
        // emit sendLog(subjectName + " " + tr("images qforms are different, skipping"));	
        emit sendDifferentQforms(subjectName);			
        return false;
    }
  
    Utils::changeGeometry(geometryFL, &icod, &jcod, &kcod, &reorderI, &reorderJ, &reorderK);
    geometryFLChanged = true;
    BoundingBox::BoundsArrayType bounds = geometryFL->GetBounds();
    for(int i = 0; i < 3; i++) 
        dimImg3D[i] = bounds[2*i + 1] - bounds[2*i];
    
    if(modeAutomatic && imagesPreviouslyLoaded) { // after geometry has changed
        Amos_AC_Image = Image::New();
        Amos_BC_Image = Image::New();
        Amos_AC_Image->Initialize(mitk::MakeScalarPixelType<uchar>(), *geometryFL);
        Amos_BC_Image->Initialize(mitk::MakeScalarPixelType<uchar>(), *geometryFL);
    }    
    
    /*    // emit sendLog(QString::fromLatin1(Utils::nifti_orientation_string(icod)) + " " + QString::fromLatin1(Utils::nifti_orientation_string(jcod)) + " " + QString::fromLatin1(Utils::nifti_orientation_string(kcod)));
     *    cout << Utils::nifti_orientation_string(icod) << " " << Utils::nifti_orientation_string(jcod) << " " << Utils::nifti_orientation_string(kcod) << endl;*/	
    
    applyRescale = calculateRescaling();
    if(applyRescale)
        LBWM3DMinSize = (uint) round(initialLBWM3DMinSize * LBWM3DMinSizeFactor * LBWM3DMinSizeFactor/(outSpacing3D[0] * outSpacing3D[1]));
   
    return true;
}

void AmosProcessCase::convertImageMaskAndLB()
{
//     LB3DConverted = false;
//     
//     if(orgMASK3D) {
//         
//         MASK3Dcube = Utils::createCubeFromMITKImage<uchar>(orgMASK3D, false);
//         MASK3Dcube = Utils::inverseArma(MASK3Dcube); // it is possible to define ROIs and masks         
//         hasMask = true;
//         emit sendProgress(++convertCounter);
//     }    
//     
//     convertImages(Utils::LABEL);
    InternalProcessCase::convertImageMaskAndLB();
    generateLBWM3D();     
}

void AmosProcessCase::convertImages(Utils::ImageTypes type)
{
    InternalProcessCase::convertImages(type);
    
    if(convertCounter == convertProgressRange) { // for AmosProcessParallel not repeating this
        Iseg3D_BC = zeros<Cube<unsigned char>>(dimImg3D[0], dimImg3D[1], dimImg3D[2]);
        Iseg3D_AC = zeros<Cube<unsigned char>>(dimImg3D[0], dimImg3D[1], dimImg3D[2]);  
        
        // 	if(modeAutomatic)
        // emit sendLog(QString::number(FL3Dcube.n_elem) + " "  + QString::number(T13Dcube.n_elem) + " "  + QString::number(WMMask3DROICube.n_elem));
        // 	else
        // emit sendLog(QString::number(FL3Dcube.n_elem) + " "  + QString::number(T13Dcube.n_elem) + " "  + QString::number(WMMask3DROICube.n_elem) + QString::number(ANOT3Dcube.n_elem));	    
    }   
}

bool AmosProcessCase::calculateRescaling()
{
    bool apply = InternalProcessCase::calculateRescaling();
    
    //mari
    WMDisk10Diameter[0] = max(1,(int)round(WMDisk10Diameterin/outSpacing3D[0]));
    WMDisk10Diameter[1] = max(1,(int)round(WMDisk10Diameterin/outSpacing3D[1]));
    WMDisk10Diameter[2] = max(1,(int)round(WMDisk10Diameterin/outSpacing3D[2]));

    LBWMDiskDiameter[0] = max(1,(int)round(LBWMDiskDiameterin/outSpacing3D[0]));
    LBWMDiskDiameter[1] = max(1,(int)round(LBWMDiskDiameterin/outSpacing3D[1]));
    LBWMDiskDiameter[2] = max(1,(int)round(LBWMDiskDiameterin/outSpacing3D[2]));

    EXCLUSIONZONE[0] = max(1,(int)round(EXCLUSIONZONEin/outSpacing3D[0]));
    EXCLUSIONZONE[1] = max(1,(int)round(EXCLUSIONZONEin/outSpacing3D[1]));
    EXCLUSIONZONE[2] = max(1,(int)round(EXCLUSIONZONEin/outSpacing3D[2]));

    R_PERIPHERY[0] = max(1,(int)round(R_PERIPHERYin/outSpacing3D[0])); 
    R_PERIPHERY[1] = max(1,(int)round(R_PERIPHERYin/outSpacing3D[1])); 
    R_PERIPHERY[2] = max(1,(int)round(R_PERIPHERYin/outSpacing3D[2])); 

    ERODEWM_vector[0] = max(1,(int)round(abs(ERODEWM)/outSpacing3D[0]));
    ERODEWM_vector[1] = max(1,(int)round(abs(ERODEWM)/outSpacing3D[1]));
    ERODEWM_vector[2] = max(1,(int)round(abs(ERODEWM)/outSpacing3D[2]));

    return apply;
}

void AmosProcessCase::nonLoopExecute()
{
    
    
    //FALLO LEVE:  OJO, si se usa FL3Dcube.slices(SLICE_INI_LOOP, SLICE_END_LOOP) se restringe el análisis a la región en el eje Z entre el slice inicial y fial de WM => al hacer procesado 3D, la parte por encima de SLICE_INI_LOOP o por debajo de SLICE_END_LOOP no se analizarán => las WMH que toquen WM pero tengan parte fuera, esa parte no se meterá en la segmentación del blob.  Esto no es problemático porque en esas posiciones no hay WMHs.
    numSlicesWithLBWM3DMinSize = 0;
    
    if(!fullRestrictedToROI) 
        FL3DROIcube = FL3Dcube.slices(SLICE_INI_LOOP, SLICE_END_LOOP);
//         FL3DROIcube = FL3Dcube;
    else
        FL3DROIcube = FL3Dcube.subcube(dimsWM(0, 1), dimsWM(1, 1), SLICE_INI_LOOP, dimsWM(0, 0), dimsWM(1, 0), SLICE_END_LOOP);
    
    // FLAIR  3D Analysis:histogram, mean, std
    // histogram (WM distribution in FLAIR MRI)
    flair3DAnalisys();
    
    //     thresholdLevelMateria = ImageFilters::BinarizeMatrixFilter3D(&FL3DROIcube, &Imateria);
    thresholdLevelMateria = meanWM3D - 3 * stdWM3D;
    bool create = true;
    Utils::binarizeArma< Cube<float>, Cube<unsigned char> >(&FL3DROIcube, &Imateria, thresholdLevelMateria, 1.f, create);
    
    if(Utils::showTime)
        emit sendLog("Threshold level: " + QString::number(thresholdLevelMateria, 'f', 8));
//         cout << setprecision(8) << thresholdLevelMateria << endl;
    
    setStructuresParameters();    
}

bool AmosProcessCase::setParameters(QMap<QString, QString> parameters)
{
    NSIGMADESVseed = parameters["NSIGMADESVseed"].toDouble();
    NSIGMADESVseed2 = parameters["NSIGMADESVseed2"].toDouble();
    NSIGMADESV_TH2 = parameters["NSIGMADESV_TH2"].toDouble();
    NSIGMADESV_TH4 = parameters["NSIGMADESV_TH4"].toDouble();
    ERODEWM = parameters["ERODEWM"].toInt();
    WEIGHT_CLASSIFIER_FL = parameters["WEIGHT_CLASSIFIER_FL"].toDouble(); 
    return true;
}


void AmosProcessCase::WMMask3DROICompute2D(int startSlice, int endSlice)
{
    for(int itS = startSlice; itS <= endSlice; itS++) { 	
        if(stop) {
            // 	    stop = false;
            return;
        }
        
        Mat<unsigned char> LBWMModel = WMMask3DModelCube.slice(itS);
        Mat<unsigned char> LBWMEroded;
        Mat<unsigned char> LBWMDilated;

        if(ERODEWM > 0) {
            //ImageFilters::ErodeDiskMatrixFilter2D(&LBWMModel, &LBWMEroded, ERODEWM);
            ImageFilters::ErodeDilateBallArmaFilter<Mat<unsigned char>, unsigned char, 2>(ImageFilters::Erode, &LBWMModel, &LBWMEroded, ERODEWM_vector);
            WMMask3DROICube.slice(itS) = LBWMEroded;
        }
        else if(ERODEWM < 0) {
            //ImageFilters::DilateDiskMatrixFilter2D(&LBWMModel, &LBWMDilated, -ERODEWM);
            ImageFilters::ErodeDilateBallArmaFilter<Mat<unsigned char>, unsigned char, 2>(ImageFilters::Dilate, &LBWMModel, &LBWMDilated, ERODEWM_vector);
            WMMask3DROICube.slice(itS) = LBWMDilated;
        } 
        else
            WMMask3DROICube.slice(itS) = LBWMModel;
    }
}

void AmosProcessCase::WMMask3DROICompute3D()
{
 	
    if(stop) {
        // 	    stop = false;
        return;
    }
    
    itk::ThreadProcessIdType numOfThreads = numThreads;
    if(ERODEWM > 0) {
        ImageFilters::ErodeDilateBallArmaFilter<Cube<unsigned char>, unsigned char, 3>(ImageFilters::Erode, &WMMask3DModelCube, &WMMask3DROICube, ERODEWM_vector, numOfThreads);
    }
    else if(ERODEWM < 0) {
        ImageFilters::ErodeDilateBallArmaFilter<Cube<unsigned char>, unsigned char, 3>(ImageFilters::Dilate, &WMMask3DModelCube, &WMMask3DROICube, ERODEWM_vector, numOfThreads);
    } 
    else
        WMMask3DROICube = WMMask3DModelCube;
}

void AmosProcessCase::thresholdsLoop(int startSlice, int endSlice)
{
    for(int itS = startSlice; itS <= endSlice; itS++) { 	
        if(stop) {
            // 	    stop = false;
            return;
        }
        
        Mat<unsigned char> LBWMModel = WMMask3DModelCube.slice(itS - SLICE_INI_LOOP);
        uvec sizeLBWM = find(LBWMModel);
        if(sizeLBWM.size() <= LBWM3DMinSize) {// only slices with enough WM
            emit sendProgress(++sliceCounter);
            // 	    cout << "thresholdsLoop non " << sliceCounter << endl;
            continue;
        }
        
        numSlicesWithLBWM3DMinSize++;
        //Tail histogram analisys	
        fmat FL = FL3DROIcube.slice(itS - SLICE_INI_LOOP);
        ResultScanTailHisto Feat_scan_WM;
        Feat_scan_WM = scanSegmentTailHistogram (&FL, &LBWMModel);
        THwm_noiseLevel(itS - SLICE_INI_LOOP) = Feat_scan_WM.K_noiseLevel;
        THwm_highestJumpNObjs(itS - SLICE_INI_LOOP) = Feat_scan_WM.K_highestJumpNBlobs;
        THwm_firstJumpNObjs(itS - SLICE_INI_LOOP) = Feat_scan_WM.K_firstJumpNBlobs;
        THwm_areaObj(itS - SLICE_INI_LOOP) = Feat_scan_WM.K_areaSeg;
        
        Mat<unsigned char> LBWMDilated;
        //ImageFilters::DilateDiskMatrixFilter2D(&LBWMModel, &LBWMDilated, WMDisk10Diameter);
        ImageFilters::ErodeDilateBallArmaFilter<Mat<unsigned char>, unsigned char, 2>(ImageFilters::Dilate, &LBWMModel, &LBWMDilated, WMDisk10Diameter);
        
        Mat<unsigned char> Iwmborder = (LBWMDilated - LBWMModel) % Imateria.slice(itS - SLICE_INI_LOOP);
        ResultScanTailHisto Feat_scan_WMborder;
        Feat_scan_WMborder = scanSegmentTailHistogram (&FL, &Iwmborder);
        THgm_highestJumpNObjs(itS - SLICE_INI_LOOP) = Feat_scan_WMborder.K_highestJumpNBlobs;
        THgm_firstJumpNObjs(itS - SLICE_INI_LOOP) = Feat_scan_WMborder.K_firstJumpNBlobs;
        THgm_areaObj(itS - SLICE_INI_LOOP) = Feat_scan_WMborder.K_areaSeg;	
        
        emit sendProgress(++sliceCounter);
    }    
}

void AmosProcessCase::mainLoop2D(int startSlice, int endSlice, int threadIndex)
{
    for(int itS = startSlice; itS <= endSlice; itS++) { 
        //     for(int itS = 21; itS <= endSlice; itS++) { 
        
        if(stop) {
            // 	    stop = false;
            return;
        }
        
        // emit sendLog("Start slice ------------  " + QString::number(itS));
        // 	cout << endl << "Start slice ------------ " << itS << endl;	
        
        // asign 2D slice from volume
        fmat FL = FL3DROIcube.slice(itS - SLICE_INI_LOOP);
        fmat T1 = T13Dcube.slice(itS - SLICE_INI_LOOP);		
        Mat<unsigned char> WMMaskROI = WMMask3DROICube.slice(itS - SLICE_INI_LOOP);
        Mat<unsigned char> WMMaskModel = WMMask3DModelCube.slice(itS - SLICE_INI_LOOP);
/*        if(itS == endSlice) 
            WMMaskROI.save("WMMaskROI.mat", arma_ascii);*/        
        
        
        // 	WMMaskROI.save("WMMaskROIAmos.mat", arma_ascii);	
        
        if(!Utils::isNotZero< Mat<unsigned char> >(&WMMaskROI)) {
            // emit sendLog("End slice WMMaskROI ------------  " + QString::number(itS));
            // 	    cout << endl << "End slice WMMaskROI -------- " << itS << endl << endl;
            emit sendProgress(++sliceCounter);
            continue;
        }
        
        // 	FL.save("FLAmos.mat", arma_ascii);
        // 	Imateria.slice(itS-SLICE_INI_LOOP).save("ImateriaAmos.mat", arma_ascii);
        
        Mat<unsigned char> WMmaskROIBin;
        Utils::logicalArma<unsigned char>(&WMMaskROI, &WMmaskROIBin);
        Mat<unsigned char> WMmaskModelBin;
        Utils::logicalArma<unsigned char>(&WMMaskModel, &WMmaskModelBin);        
        
        Mat<unsigned char> ANOT;
        if(!modeAutomatic)
            ANOT = ANOT3Dcube.slice(itS - SLICE_INI_LOOP);	
        // 	if(!modeAutomatic) {
        // 	    Utils::logicalArma<unsigned char>(&(ANOT3Dcube.slice(itS - SLICE_INI_LOOP)), &ANOT);
        // 	    ANOT = ANOT % WMmaskROIBin;
        // 	}
        
        // Distance to convexhull of the region of interest
        // Used to classify WML as function of periphery distance.
        fmat DistToCHULL;	
        
        //brain tissue of interest for WM
        Mat<unsigned char> Iroi;
        //ImageFilters::DilateDiskMatrixFilter2D(&WMmaskROIBin, &Iroi, WMDisk10Diameter);
        ImageFilters::ErodeDilateBallArmaFilter<Mat<unsigned char>, unsigned char, 2>(ImageFilters::Dilate, &WMmaskROIBin, &Iroi, WMDisk10Diameter);
        // 	Iroi.save("IroiAmos.mat", arma_ascii);
        ImageFilters::NotEmptyForeground IroiHasForeground = Utils::isNotZero< Mat<unsigned char> >(&Iroi) ? ImageFilters::YES : ImageFilters::NO;
        bool notSignedI = true;
        Mat<unsigned char> Ich;
        if(IroiHasForeground == ImageFilters::YES) {
            auto begin = chrono::high_resolution_clock::now();
            
//             if(numThreads > 1)
//                 mutex.lock();
            regionProps.at(threadIndex)->setInputMatrix(&Iroi);
            regionProps.at(threadIndex)->execute();
            Ich = regionProps.at(threadIndex)->getConvexImage();
//             if(numThreads > 1) {
//                 mutex.unlock();
//                 if(!wait(60000))
//                     cout << "AmosProcessCase::mainLoop2D regionProps time exceeded" << endl;
//             }
            
            auto end = chrono::high_resolution_clock::now();
            if(Utils::showTime)
                emit sendLog("regionProps " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
            // cout << "regionProps " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
            
            // 	    Ich.save("IchAmos.mat", arma_ascii);
            Ich = Utils::inverseArma(Ich);	    
        }
        else
//             Ich.set_size(Iroi.n_rows, Iroi.n_cols);
            Ich.zeros(Iroi.n_rows, Iroi.n_cols);
        ImageFilters::DistanceMapFilter< Mat<unsigned char>, unsigned char, Mat<float>, float, 2>(&Ich, &DistToCHULL, IroiHasForeground, notSignedI);
        // 	DistToCHULL.save("DistToCHULLAmos.mat", arma_ascii);	    
        
        if(stop) {
            // 	    stop = false;
            return;
        }
        
        Mat<unsigned char>Imateriainroi = Imateria.slice(itS - SLICE_INI_LOOP) % Iroi;		
        
        // Fill holes
        Mat<unsigned char> Ifilled;
        ImageFilters::FillHolesBinaryFilter<Mat<unsigned char>, 2>(&Imateriainroi, &Ifilled);
        // 	Ifilled.save("IfilledAmos.mat", arma_ascii);
        // 	Imateriainroi.save("ImateriainroiAmos.mat", arma_ascii);
        
        fmat DistToBorder;
        ImageFilters::NotEmptyForeground notEmptyForeground = ImageFilters::YES;
        bool notSignedB = false;
        ImageFilters::DistanceMapFilter< Mat<unsigned char>, unsigned char, Mat<float>, float, 2>(&Ifilled, &DistToBorder, notEmptyForeground, notSignedB);
        // 	DistToBorder.save("DistToBorderAmos.mat", arma_ascii);		
        
        // Distance to edges of WMmaskModel	
        ImageFilters::NotEmptyForeground WMmaskHasForeground = Utils::isNotZero< Mat<unsigned char> >(&WMmaskModelBin) ? ImageFilters::YES : ImageFilters::NO;
        notSignedB = false;
        fmat DistToWMBorder;
        ImageFilters::DistanceMapFilter< Mat<unsigned char>, unsigned char, Mat<float>, float, 2>(&WMmaskModelBin, &DistToWMBorder, WMmaskHasForeground, notSignedB);	
        // 	DistToWMBorder.save("DistToWMBorderAmos.mat", arma_ascii);		
        
        if (WMmaskHasForeground == ImageFilters::NO){
            // emit sendLog("End slice WMmaskModelBin ------------  " + QString::number(itS));
            // 	    cout << endl << "End slice WMmaskModelBin ----- " << itS << endl << endl;
            emit sendProgress(++sliceCounter);
            continue;
        }		
        
        if(stop) {
            // 	    stop = false;
            return;
        }
        
        // *******************  Step 1: SEGMENTATION  *******************
        
        //  Definition of the mask where THsedd is applied
        // In MatLab switch (METODO_UMBRAL) case 'S1_S2'
        double noiseLevel = THwm_noiseLevel(itS - SLICE_INI_LOOP);
//         cout << "noiseLevel " << noiseLevel << endl;
        
        double Ks1 = noiseLevel  + NSIGMADESVseed;
        double Kth1 = noiseLevel + NSIGMADESV_TH2;
        double Ks2 = noiseLevel  + NSIGMADESVseed2;
        double Kth2 = noiseLevel + NSIGMADESV_TH4;
        
        if(Ks1 < Kth1) // if seed 1 is lesser than threshold 1, segment with threshold,  
            Ks1 = Kth1; // (this situation should not appear due to parameters configuration)
        if(Kth1 < Kth2)
            Kth2 = Kth1;
        if(Ks2 > Ks1) // do not apply threshold 2
            Ks2 = 1000;
        
        double TH = min(.99, p_Iseg.meanWM3D + Ks1 * p_Iseg.stdWM3D); // seed
        Mat<float> MaskFloat = conv_to< Mat<float> >::from(WMmaskROIBin);
/*        if(itS == endSlice) 
            WMmaskROIBin.save("WMmaskROIBin_Amos.mat", arma_ascii); */       
        // 	cout << "MaskFloat " << MaskFloat.max() << " " << MaskFloat.min() << endl;
        // 	cout << "FL " << FL.max() << " " << FL.min() << endl;
        Mat<float> Fl_x_Mask = FL % MaskFloat;
        Mat<float> Fl_x_Mask2 = FL % Iroi;

        Mat<unsigned char> Iseg1;
        bool create = true;
        Utils::binarizeArma< Mat<float>, Mat<unsigned char> >(&Fl_x_Mask, &Iseg1, TH, 1, create);
        // 	Iseg1.save("Iseg1Amos.mat", arma_ascii);
        
        TH = min(0.99, p_Iseg.meanWM3D + Kth1 * p_Iseg.stdWM3D);
        Mat<unsigned char> Iseg2;
        Utils::binarizeArma< Mat<float>, Mat<unsigned char> >(&Fl_x_Mask2, &Iseg2, TH, 1, create); // segmenting growth.
        // 	Iseg2.save("Iseg2Amos.mat", arma_ascii);
        
//         Mat<unsigned char> IsegA;
//         ImageFilters::filteringImageWithSeed< Mat<unsigned char>, unsigned char, 2 >(&Iseg2, &Iseg1, &IsegA);
//         IsegA.save("IsegAAmos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed	

        Mat<unsigned char> IsegA;
//         ImageFilters::filteringImageWithSeed1< Mat<unsigned char>, unsigned char, 2 >(&Iseg2, &Iseg1, &IsegA);
//         IsegA.save("IsegA1Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed	
 
        
        //cout << "filtrar con semilla Iseg2 con Iseg1 -> IsegA" << endl;
        
        ImageFilters::filteringImageWithSeed2< Mat<unsigned char>, unsigned char, 2 >(&Iseg2, &Iseg1, &IsegA);
//         IsegA.save("IsegA2Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed	
         
        if(stop) {
            // 	    stop = false;
            return;
        }	
        
        Mat<unsigned char> IsegB;
        Mat<unsigned char> IsegR;
        // second segmentation. Use a value NSIGMADESVseed2>100 for not perform it
        if(Ks2 < NSIGMADESVseed2Max) {
            TH = min(.99, p_Iseg.meanWM3D + Ks2 * p_Iseg.stdWM3D);
            
            // % segmentation for generating seed 2
            Mat<unsigned char> Iseg3;
            create = true;
            Utils::binarizeArma< Mat<float>, Mat<unsigned char> >(&Fl_x_Mask, &Iseg3, TH, 1, create);
            
            TH = min(0.99, p_Iseg.meanWM3D + Kth2 * p_Iseg.stdWM3D);
            Mat<unsigned char> Iseg4;
            Utils::binarizeArma< Mat<float>, Mat<unsigned char> >(&Fl_x_Mask2, &Iseg4, TH, 1, create); // segmenting growth
        
            
                  //cout << "filtrar con semilla Iseg4 con Iseg3 -> IsegB" << endl;

            
/*            ImageFilters::filteringImageWithSeed1< Mat<unsigned char>, unsigned char, 2 >(&Iseg4, &Iseg3, &IsegB);
            IsegB.save("IsegB1Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed*/	
            ImageFilters::filteringImageWithSeed2< Mat<unsigned char>, unsigned char, 2 >(&Iseg4, &Iseg3, &IsegB);
//             IsegB.save("IsegB2Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed	
//             ImageFilters::filteringImageWithSeed3< Mat<unsigned char>, unsigned char, 2 >(&Iseg4, &Iseg3, &IsegB);
//             IsegB.save("IsegB3Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed
                        
            // connecting segmenta1ion 1 with segmentation 2
            // removing blobs from segmentation 2 coincident with segmentation 1
            Mat<unsigned char> IsegC;
//             ImageFilters::filteringImageWithSeed1< Mat<unsigned char>, unsigned char, 2 >(&IsegB, &IsegA, &IsegC);
//             IsegC.save("IsegC1Amos.mat", arma_ascii);
            ImageFilters::filteringImageWithSeed2< Mat<unsigned char>, unsigned char, 2 >(&IsegB, &IsegA, &IsegC);
//             IsegC.save("IsegC2Amos.mat", arma_ascii);
/*            ImageFilters::filteringImageWithSeed3< Mat<unsigned char>, unsigned char, 2 >(&IsegB, &IsegA, &IsegC);
            IsegC.save("IsegC3Amos.mat", arma_ascii);*/  

            IsegR = IsegB - IsegC + IsegA;  // mixing segmentations
/*            if(itS == endSlice)  {
                IsegA.save("IsegA_Amos.mat", arma_ascii);
                IsegB.save("IsegB_Amos.mat", arma_ascii);
                IsegC.save("IsegC_Amos.mat", arma_ascii);
                IsegR.save("IsegR_Amos.mat", arma_ascii);
            }*/            
            // 	    IsegR.save("IsegRAmos.mat", arma_ascii);            
            if(MINWMLSIZE > 0) 
                ImageFilters::ThresholdByLabelsSizeFilter<Mat< unsigned char>, 2>(&IsegR, MINWMLSIZE);
        }
        else {
            if(MINWMLSIZE > 0) {
                ImageFilters::ThresholdByLabelsSizeFilter<Mat< unsigned char>, 2>(&IsegA, MINWMLSIZE);
                IsegB = IsegA;
                IsegR = IsegA;
            }
            else {
                IsegB = IsegA;
                IsegR = IsegA;		
            }
        }
        
        // This is the difference with Amos-test
        // IsegR = IsegR % Iroi;   //mari: eliminado porque ya no tiene sentido
        // case 'S1_S2'
        
        // ********************* Step 2: CLASIFICATION *********************
        
        if(stop) {
            // 	    stop = false;
            return;
        }	 
        
        ImageFilters::NotEmptyForeground IsegRHasForeground = Utils::isNotZero< Mat<unsigned char> >(&IsegR) ? ImageFilters::YES : ImageFilters::NO;
        
        if (IsegRHasForeground == ImageFilters::NO) {
            // emit sendLog("End slice IsegR ------------  " + QString::number(itS));
            // 	    cout << endl << "End slice IsegR ------ " << itS << endl << endl;
            emit sendProgress(++sliceCounter);
            continue;
        }
        
        // ***************** Step 2.1  COMPUTING BLOB FEATURES ****************************
        
        // slice segmentations: 1) before (initial segmentation) and 2) after the classifier (final segmentation)
        Mat<unsigned char>Iseg2D_BC = IsegR;

        // 	if(hasMask)
        // 	    Iseg2D_BC = IsegR % MASK3Dcube.slice(itS - SLICE_INI_LOOP); //Mask_analysys eliminates ischemic areas
        // 	else
        // 	    Iseg2D_BC = IsegR;
        Mat<unsigned char> Iseg2D_AC = Iseg2D_BC;
//         if(itS == endSlice)
//             Iseg2D_BC.save("Iseg2D_BCAmos1.mat", arma_ascii);
        
        // characterizing the blobs SEG
        
        if(stop) {
            // 	    stop = false;
            return;
        }
        
        // For IndexesTransform tests
        
//         Mat<unsigned char> matImage = Iseg2D_BC * 255;
//         typedef unsigned char     PixelType;
//         const     unsigned int    Dimension = 2;
//         typedef itk::Image< PixelType, Dimension >  ImageType;                
//         typedef  itk::ImageFileWriter< ImageType  > WriterType;                
//         arma::uvec size1 = Utils::armaSize<PixelType>(&Iseg2D_BC);
//         QString outputFileName = "BC_" + QString::number(itS) + ".png";
//         ImageType::Pointer image = Utils::armaToITKImage<PixelType, 2>(matImage.memptr(), size1);
//         
// 
//         WriterType::Pointer writer = WriterType::New();
//         writer->SetFileName(outputFileName.toStdString());
//         writer->SetInput(image);
//         writer->Update();
//         QString FLFileName = "FL_" + QString::number(itS) + ".mat";
//         FL.save(FLFileName.toStdString(), arma_ascii);
//         QString T1FileName = "T1_" + QString::number(itS) + ".mat";
//         T1.save(T1FileName.toStdString(), arma_ascii);
//         QString DistToBFileName = "DistToBorder_" + QString::number(itS) + ".mat";
//         DistToBorder.save(DistToBFileName.toStdString(), arma_ascii);
//         QString DistToCHFileName = "DistToCHULL_" + QString::number(itS) + ".mat";
//         DistToCHULL.save(DistToCHFileName.toStdString(), arma_ascii);        
        
        const int DimensionBC = 2;
        // 	vector<ImageFilters::LabelFeatures<DimensionBC> > labelsInfoBC;
        // 	ImageFilters::BinaryLabelGeometryFilter<Mat< unsigned char>, unsigned char, DimensionBC >(&labelsInfoBC, &Iseg2D_BC);
        // 	typename vector<ImageFilters::LabelFeatures<DimensionBC> >::iterator labelsInfoItBC = labelsInfoBC.begin();
        // 	cout << labelsInfoBC.size() << " blobs" << endl;
        itk::ThreadProcessIdType numOfThreads = 1;
        bool fullConnectivity = true;
        bool calculateBlobDeep = true;	
        vector<ImageFilters::LabelSimpleFeatures<DimensionBC> > labelsInfoBC;
        ImageFilters::BinaryLabelsFilter<Mat< unsigned char>, DimensionBC >(&labelsInfoBC, &Iseg2D_BC, numOfThreads, fullConnectivity, calculateBlobDeep);	    
        
        typename vector<ImageFilters::LabelSimpleFeatures<DimensionBC> >::iterator labelsInfoItBC = labelsInfoBC.begin();		
        
        int badBlobValue = featuresClasses.at(0);
        for( ; labelsInfoItBC != labelsInfoBC.end(); labelsInfoItBC++) {
            if((*labelsInfoItBC).volume > MINWMLSIZE) { // don't analyze very small blobs (possibly noise)
                // obtains feature vector
                fvec featuresVector;
                if(modeAutomatic)
                    featuresVector.set_size(featuresNumber);
                else
                    featuresVector.set_size(featuresNumber + 1); // + 1 classSVM
                featuresVector.zeros();		
                // calculateRegionFeatures(&FL, &T1, &DistToBorder, &DistToCHULL, &(*labelsInfoItBC), itS, &featuresVector);        // **mari_todo:cambiado  DistToBorder  a DistToWMBorder      
                calculateRegionFeatures(&FL, &T1, &DistToWMBorder, &DistToCHULL, &(*labelsInfoItBC), itS, &featuresVector);        // **mari_todo:cambiado  DistToBorder  a DistToWMBorder      

                arma::Mat<itk::IndexValueType>::fixed<2, 2> boundings;
                boundings.col(0) = (*labelsInfoItBC).boundingBox.col(1);
                boundings.col(1) = (*labelsInfoItBC).boundingBox.col(0);

                if( boundings.at(0,0) -  EXCLUSIONZONE[0] - R_PERIPHERY[0] >= 0 && boundings.at(0,1) - EXCLUSIONZONE[1] - R_PERIPHERY[1] >= 0) {
                    boundings.at(0,0) -= EXCLUSIONZONE[0] + R_PERIPHERY[0];
                    boundings.at(0,1) -= EXCLUSIONZONE[1] + R_PERIPHERY[1];
                }
                else if(boundings.at(0,0) - EXCLUSIONZONE[0] - R_PERIPHERY[0] >= 0) {
                    boundings.at(0,0) -= EXCLUSIONZONE[0] + R_PERIPHERY[0];
                    boundings.at(0,1) = 0;
                }
                else if(boundings.at(0,1) - EXCLUSIONZONE[1] - R_PERIPHERY[1] >= 0) {
                    boundings.at(0,0) = 0;
                    boundings.at(0,1) -= EXCLUSIONZONE[1] + R_PERIPHERY[1];
                }
                else {
                    boundings.at(0,0) = 0;
                    boundings.at(0,1) = 0;
                }
                
                if(boundings.at(1,0) + EXCLUSIONZONE[0] + R_PERIPHERY[0] < (int)Iseg2D_BC.n_rows && boundings.at(1,1) + EXCLUSIONZONE[1] + R_PERIPHERY[1] < (int)Iseg2D_BC.n_cols) {
                    boundings.at(1,0) += EXCLUSIONZONE[0] + R_PERIPHERY[0];
                    boundings.at(1,1) += EXCLUSIONZONE[1] + R_PERIPHERY[1];
                }
                else if(boundings.at(1,0) + EXCLUSIONZONE[0] + R_PERIPHERY[0] < (int)Iseg2D_BC.n_rows) {
                    boundings.at(1,0) += EXCLUSIONZONE[0] + R_PERIPHERY[0];
                    boundings.at(1,1) = Iseg2D_BC.n_cols - 1;
                }
                else if(boundings.at(1,1) + EXCLUSIONZONE[1] + R_PERIPHERY[1] < (int)Iseg2D_BC.n_cols) {                
                    boundings.at(1,0) = Iseg2D_BC.n_rows - 1;
                    boundings.at(1,1) += EXCLUSIONZONE[1] + R_PERIPHERY[1];
                }
                else {
                    boundings.at(1,0) = Iseg2D_BC.n_rows - 1;
                    boundings.at(1,1) = Iseg2D_BC.n_cols - 1;
                }                

                Mat<unsigned char> blobsMat;
                
/*                if(!fullRestrictedToROI) 
                    blobsMat = zeros< Mat<unsigned char> >((int)dimImg3D[0], (int)dimImg3D[1]);
                else
                    blobsMat = zeros< Mat<unsigned char> >((unsigned int)(dimsWM(0, 0) - dimsWM(0, 1) + 1), (unsigned int)(dimsWM(1, 0) - dimsWM(1, 1) + 1));
                blobsMat.elem((*labelsInfoItBC).indexOfPixels) = ones< Col<unsigned char> >(size((*labelsInfoItBC).indexOfPixels));*/                

                blobsMat = zeros< Mat<unsigned char> >(boundings.at(1,0) - boundings.at(0,0) + 1, boundings.at(1,1) - boundings.at(0,1) + 1);                                
                
                arma::Col<long long unsigned int> newIndexes;

                arma::Col<long long unsigned int> transIndexes = (*labelsInfoItBC).indexOfPixels;
//                 cout << transIndexes.t() << endl;
                newIndexes.zeros(blobsMat.n_elem);

                unsigned int blobRows = blobsMat.n_rows;
                newIndexes = (transIndexes / Iseg2D_BC.n_rows -  boundings.at(0, 1)) * blobRows;
                newIndexes += transIndexes - (transIndexes / Iseg2D_BC.n_rows) * Iseg2D_BC.n_rows - boundings.at(0, 0);            
                
//                 cout << newIndexes.t() << endl;            
//                 
//                 cout << (*labelsInfoItBC).boundingBox << endl;
//                 cout << boundings << endl;

                blobsMat.elem(newIndexes) = ones< Col<unsigned char> >(arma::size((*labelsInfoItBC).indexOfPixels));                                 

                Mat<unsigned char> Mdil;
                //ImageFilters::DilateDiskMatrixFilter2D(&blobsMat, &Mdil, EXCLUSIONZONE);
                ImageFilters::ErodeDilateBallArmaFilter<Mat<unsigned char>, unsigned char, 2>(ImageFilters::Dilate, &blobsMat, &Mdil, EXCLUSIONZONE);
                Mat<unsigned char> Mper;
                
                ImageFilters::ErodeDilateBallArmaFilter<Mat<unsigned char>, unsigned char, 2>(ImageFilters::Dilate, &Mdil, &Mper, R_PERIPHERY);
                Mper = Mper - Mdil;   
                

//                 calculatePerimeterFeatures(&FL, &T1, &Mper, &featuresVector);
                calculatePerimeterFeatures(&FL, &T1, &Mper, boundings, &featuresVector);
                
                if(modeAutomatic) { // use classifier to eliminate false blobs
                    // 		    cout << featuresVector.t();
                    scaleFeatures(&featuresVector);
                    // 		    cout << featuresVector.t();
                    
                    if(numThreads > 1)
                        mutex.lock();
                    classifier->setData(featuresVector);
                    vec classifierResults = classifier->predict();
                    if(numThreads > 1) {
                        mutex.unlock();
                        wait();
                    }
                    
                    bool correct = (int)round(classifierResults(0)) == badBlobValue ? false : true;
                    if(!correct)
                        Iseg2D_AC.elem((*labelsInfoItBC).indexOfPixels) = zeros< Mat<unsigned char> >(size((*labelsInfoItBC).indexOfPixels));  // delete blob from output
                        // 		cout << classifierResults.t();
                }
                else { // for classifier training
                    
                    double classSVM = intersectBlobWML<Mat<unsigned char>, DimensionBC, ImageFilters::LabelSimpleFeatures<DimensionBC> >(&ANOT, &Iseg2D_BC, &(*labelsInfoItBC)) ? 1 : 0;
                    featuresVector(featuresNumber) = classSVM;
                    // 		    cout << featuresVector.t();	
                    addClassificationLine(featuresVector);
                }
            }
            else
                Iseg2D_AC.elem((*labelsInfoItBC).indexOfPixels) = zeros< Mat<unsigned char> >(size((*labelsInfoItBC).indexOfPixels));  // delete blob from output
        }                
            
        if(!fullRestrictedToROI) {
            Iseg3D_BC.slice(itS) = Iseg2D_BC;
            Iseg3D_AC.slice(itS) = Iseg2D_AC; 
        }
        else {
            Iseg3D_BC.subcube(dimsWM(0, 1), dimsWM(1, 1), itS, dimsWM(0, 0), dimsWM(1, 0), itS) = Iseg2D_BC;
            Iseg3D_AC.subcube(dimsWM(0, 1), dimsWM(1, 1), itS, dimsWM(0, 0), dimsWM(1, 0), itS) = Iseg2D_AC;
        }
//         if(itS == endSlice)
//         Iseg3D_BC.slice(itS).save("Iseg2D_BCAmos.mat", arma_ascii);
        
        // emit sendLog("End slice ------------  " + QString::number(itS));
        emit sendProgress(++sliceCounter);
        // 	cout << endl << "End slice ------------ " << itS << endl << endl;
    }    
}

void AmosProcessCase::startMainLoop3D()
{
    if(stop) {
        // 	    stop = false;
        return;
    }
    //  Definition of the mask where THseed is applied
    // In MatLab switch (METODO_UMBRAL) case 'S1_S2'
    double noiseLevel = K3D_wm_noiseLevel;
//     cout << "noiseLevel " << noiseLevel << endl;

    Ks1_3D = noiseLevel  + NSIGMADESVseed;
    Kth1_3D = noiseLevel + NSIGMADESV_TH2;
    Ks2_3D = noiseLevel  + NSIGMADESVseed2;
    Kth2_3D = noiseLevel + NSIGMADESV_TH4;    
    
    if(Ks2_3D < NSIGMADESVseed2Max)
        progressRange = 13;
    else
        progressRange = 15;               
//     QTimer::singleShot(0, this, SLOT(changeProgres()));    
    InternalProcessCase::startMainLoop3D();
}

void AmosProcessCase::computeIROI3D()
{
    
    itk::ThreadProcessIdType numOfThreads = numThreads;	

    Utils::logicalArma<unsigned char>(&WMMask3DROICube, &WMmask3DROIBinCube);
    Utils::logicalArma<unsigned char>(&WMMask3DModelCube, &WMmask3DModelBinCube); 
    emit sendProgress(++sliceCounter); // 1
    

//     fcube DistToCHULL;	
    
    //brain tissue of interest for WM    
    ImageFilters::ErodeDilateBallArmaFilter<Cube<unsigned char>, unsigned char, 3>(ImageFilters::Dilate, &WMmask3DROIBinCube, &Iroi3D, WMDisk10Diameter, numOfThreads);
    // 	Iroi3D.save("IroiAmos.mat", arma_ascii);
    emit sendProgress(++sliceCounter); // 2    
}

void AmosProcessCase::computeRegionProps3D(uint iniSlice, uint endSlice, int threadIndex)
{        
    auto begin = chrono::high_resolution_clock::now();
        
    for(uint i = iniSlice; i <= endSlice; i++) {
        if(IroiHasForeground3D == ImageFilters::YES) {
            Mat<unsigned char>* slice = &(Iroi3D.slice(i - SLICE_INI_LOOP));
            regionProps.at(threadIndex)->setInputMatrix(slice);
            regionProps.at(threadIndex)->execute();
            Ich3D.slice(i - SLICE_INI_LOOP) = regionProps.at(threadIndex)->getConvexImage();
//             convexHullOutlineBinaryCube.slice(i) = regionProps.at(threadIndex)->getConvexOutlineHullBinaryMat();            
//             convexHullSize += regionProps.at(threadIndex)->getConvexHullSize();
//             convexHullContourSize += regionProps.at(threadIndex)->getConvexHullContourSize();
            
            // 	    Ich.save("IchAmos.mat", arma_ascii);
            /*Ich3D = Utils::inverseArma(Ich3D);*/            
            Ich3D.slice(i - SLICE_INI_LOOP) = Utils::inverseArma(Ich3D.slice(i - SLICE_INI_LOOP));
        }
        else
//             Ich.set_size(Iroi.n_rows, Iroi.n_cols);
            Ich3D.slice(i - SLICE_INI_LOOP).zeros(Iroi3D.n_rows, Iroi3D.n_cols);
    }        

//         regionProps3D->setInputCube(&Iroi3D);
//         regionProps3D->execute();
//         Ich3D = regionProps3D->getConvexImage();
        
    auto end = chrono::high_resolution_clock::now();
    if(Utils::showTime)
        emit sendLog("regionProps3D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        // cout << "regionProps " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;        	                          
}

// void AmosProcessCase::computeRegionProps3D(uint iniSlice, uint endSlice, int threadIndex)
// {        
//     if(IroiHasForeground3D == ImageFilters::YES) {
//         auto begin = chrono::high_resolution_clock::now();
//         
//         for(uint i = iniSlice; i <= endSlice; i++) {
//             Mat<unsigned char>* slice = &(Iroi3D.slice(i - SLICE_INI_LOOP));
//             regionProps.at(threadIndex)->setInputMatrix(slice);
//             regionProps.at(threadIndex)->execute();
//             Ich3D.slice(i - SLICE_INI_LOOP) = regionProps.at(threadIndex)->getConvexImage();
// //             convexHullOutlineBinaryCube.slice(i) = regionProps.at(threadIndex)->getConvexOutlineHullBinaryMat();            
// //             convexHullSize += regionProps.at(threadIndex)->getConvexHullSize();
// //             convexHullContourSize += regionProps.at(threadIndex)->getConvexHullContourSize();
//         }        
// 
// //         regionProps3D->setInputCube(&Iroi3D);
// //         regionProps3D->execute();
// //         Ich3D = regionProps3D->getConvexImage();
//         
//         auto end = chrono::high_resolution_clock::now();
//         if(Utils::showTime)
//             emit sendLog("regionProps3D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
//         // cout << "regionProps " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
//         
//         // 	    Ich.save("IchAmos.mat", arma_ascii);
//         Ich3D = Utils::inverseArma(Ich3D);	    
//     }
//     else
// //             Ich.set_size(Iroi.n_rows, Iroi.n_cols);
//         Ich3D.zeros(Iroi3D.n_rows, Iroi3D.n_cols, Iroi3D.n_slices);                      
// }

//mari void AmosProcessCase::computeDistsTo(arma::fcube* DistToWMBorder)
void AmosProcessCase::computeDistsTo()
{
    emit sendProgress(++sliceCounter); // 3 
    
    bool notSignedI = true;
    itk::ThreadProcessIdType numOfThreads = numThreads;
    ImageFilters::DistanceMapFilter< Cube<unsigned char>, unsigned char, Cube<float>, float, 3>(&Ich3D, &DistToCHULL3D, IroiHasForeground3D, notSignedI, numOfThreads);
    // 	DistToCHULL3D.save("DistToCHULL3DAmos.mat", arma_ascii);	    
    emit sendProgress(++sliceCounter); // 4
    
    if(stop) {
        // 	    stop = false;
        return;
    }
    
    Cube<unsigned char>Imateriainroi = Imateria % Iroi3D;		
    
    // Fill holes
    Cube<unsigned char> Ifilled;
    bool fullConnectivity = true;
    ImageFilters::FillHolesBinaryFilter<Cube<unsigned char>, 3>(&Imateriainroi, &Ifilled, fullConnectivity, numOfThreads);
    // 	Ifilled.save("IfilledAmos.mat", arma_ascii);
    // 	Imateriainroi.save("ImateriainroiAmos.mat", arma_ascii);
    emit sendProgress(++sliceCounter); // 5

    //mari
    //ImageFilters::NotEmptyForeground notEmptyForeground = ImageFilters::YES;
    //bool notSignedB = false;
    //ImageFilters::DistanceMapFilter< Cube<unsigned char>, unsigned char, Cube<float>, float, 3>(&Ifilled, &DistToBorder3D, notEmptyForeground, notSignedB, numOfThreads);
    // // 	DistToBorder3D.save("DistToBorder3DAmos.mat", arma_ascii);		
    //emit sendProgress(++sliceCounter); // 6
    
    // Distance to edges of WMmaskModel	
    ImageFilters::NotEmptyForeground WMmaskHasForeground = Utils::isNotZero< Cube<unsigned char> >(&WMmask3DModelBinCube) ? ImageFilters::YES : ImageFilters::NO;
    bool notSignedB = false;    
    ImageFilters::DistanceMapFilter< Cube<unsigned char>, unsigned char, Cube<float>, float, 3>(&WMmask3DModelBinCube, &DistToWMBorder3D, WMmaskHasForeground, notSignedB, numOfThreads);	
    // 	DistToWMBorder.save("DistToWMBorderAmos.mat", arma_ascii);				
    emit sendProgress(++sliceCounter); // 6

//   // **1 **mari:  dist to WMCM
//     // Distance to WMCM	
//     uvec ind = find(WMmask3DModelBinCube);
//     umat t = ind2sub(size(WMmask3DModelBinCube), ind);
//     if(t.n_elem > 0)
//         WMCM = conv_to<fvec>::from(arma::mean(t,1));
//     else
//         WMCM = {0,0,0};
//     //cout << "WMCM: " << WMCM.t() << endl;
//     //cout << "WMCM2: " << WMCM(0)<< ", " << WMCM(1) << ", " << WMCM(2) << endl;    
//     arma::Cube<unsigned char> Icm;
//     Icm.zeros(Iroi3D.n_rows, Iroi3D.n_cols, Iroi3D.n_slices);
//     Icm.at( WMCM(0), WMCM(1), WMCM(2)) = 1;
// 
//     WMmaskHasForeground = Utils::isNotZero< Cube<unsigned char> >(&Icm) ? ImageFilters::YES : ImageFilters::NO;
//     notSignedB = false;    
//     ImageFilters::DistanceMapFilter< Cube<unsigned char>, unsigned char, Cube<float>, float, 3>(&Icm, &DistToWMCM, WMmaskHasForeground, notSignedB, numOfThreads);	
//     DistToWMCM = abs(DistToWMCM);
//     emit sendProgress(++sliceCounter); // 8
  
 
    if(false){
        WMmask3DROIBinCube.save("WMmask3DROIBinCube.mat", arma_ascii);
        WMmask3DModelBinCube.save("WMmask3DModelBinCube.mat", arma_ascii);
        //DistToBorder3D.save("DistToBorder3D.mat", arma_ascii); 
        DistToWMBorder3D.save("DistToWMBorder3D.mat", arma_ascii); 
        FL3DROIcube.save("FL3DROIcube.mat", arma_ascii);
        DistToCHULL3D.save("DistToCHULL3D.mat", arma_ascii);     
    }
    
    if(stop) {
        // 	    stop = false;
        return;
    }    
}

void AmosProcessCase::computeISEGR(arma::Cube<unsigned char>* IsegR)
{
    itk::ThreadProcessIdType numOfThreads = numThreads;
    
    if(Ks1_3D < Kth1_3D) // if seed 1 is lesser than threshold 1, segment with threshold,  
        Ks1_3D = Kth1_3D; // (this situation should not appear due to parameters configuration)
    if(Kth1_3D < Kth2_3D)
        Kth2_3D = Kth1_3D;
    if(Ks2_3D > Ks1_3D) // do not apply threshold 2
        Ks2_3D = 1000;
    
    double TH = min(.99, p_Iseg.meanWM3D + Ks1_3D * p_Iseg.stdWM3D); // seed
    Cube<float> MaskFloat = conv_to< Cube<float> >::from(WMmask3DROIBinCube);
    // 	cout << "MaskFloat " << MaskFloat.max() << " " << MaskFloat.min() << endl;
    // 	cout << "FL " << FL.max() << " " << FL.min() << endl;
    Cube<float> Fl_x_Mask = FL3DROIcube % MaskFloat;

    //mari:
    Cube<float> Fl_x_Mask2 = FL3DROIcube % Iroi3D;
    
    
    
//     FL3DROIcube.save("FL3DROIcube.mat", arma_ascii);
//     Fl_x_Mask.save("Fl_x_Mask.mat", arma_ascii);
//     MaskFloat.save("MaskFloat.mat", arma_ascii);
    
    Cube<unsigned char> Iseg1;
    bool create = true;
    Utils::binarizeArma< Cube<float>, Cube<unsigned char> >(&Fl_x_Mask, &Iseg1, TH, 1, create);
    // 	Iseg1.save("Iseg1Amos.mat", arma_ascii);
    emit sendProgress(++sliceCounter); // 7
    
    TH = min(0.99, p_Iseg.meanWM3D + Kth1_3D * p_Iseg.stdWM3D);
    Cube<unsigned char> Iseg2;
    
    // Utils::binarizeArma< Cube<float>, Cube<unsigned char> >(&FL3DROIcube, &Iseg2, TH, 1, create); // segmenting growth.
    Utils::binarizeArma< Cube<float>, Cube<unsigned char> >(&Fl_x_Mask2, &Iseg2, TH, 1, create); // segmenting growth.

    // 	Iseg2.save("Iseg2Amos.mat", arma_ascii);
    emit sendProgress(++sliceCounter); // 8
    
    Cube<unsigned char> IsegA;
//     ImageFilters::filteringImageWithSeed1< Cube<unsigned char>, unsigned char, 3 >(&Iseg2, &Iseg1, &IsegA, create, numOfThreads);
//     Cube<unsigned char> IsegA2;
    
                  // cout << "filtrar con semilla Iseg2 con Iseg1 -> IsegA" << endl;
    
    
    ImageFilters::filteringImageWithSeed2< Cube<unsigned char>, unsigned char, 3 >(&Iseg2, &Iseg1, &IsegA, create, numOfThreads);
//     IsegA2.save("IsegA2Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed	
    emit sendProgress(++sliceCounter); // 9
    
    if(stop) {
        // 	    stop = false;
        return;
    }	
    
    Cube<unsigned char> IsegB;
    
    // second segmentation. Use a value NSIGMADESVseed2>NSIGMADESVseed2Max for not perform it
    if(Ks2_3D < NSIGMADESVseed2Max) {
        TH = min(.99, p_Iseg.meanWM3D + Ks2_3D * p_Iseg.stdWM3D);
        
        // % segmentation for generating seed 2
        Cube<unsigned char> Iseg3;
        create = true;
        Utils::binarizeArma< Cube<float>, Cube<unsigned char> >(&Fl_x_Mask, &Iseg3, TH, 1, create);
        
        TH = min(0.99, p_Iseg.meanWM3D + Kth2_3D * p_Iseg.stdWM3D);
        Cube<unsigned char> Iseg4;
        Utils::binarizeArma< Cube<float>, Cube<unsigned char> >(&Fl_x_Mask2, &Iseg4, TH, 1, create); // segmenting growth
        
//         ImageFilters::filteringImageWithSeed1< Cube<unsigned char>, unsigned char, 3 >(&Iseg4, &Iseg3, &IsegB, create, numOfThreads);
//         IsegB.save("IsegB1Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed
//         Cube<unsigned char> Iseg333 = Iseg3;
//         Cube<unsigned char> IsegB2;
        
        
      //  cout << "filtrar con semilla Iseg4 con Iseg3 -> IsegB" << endl;
 
        
        ImageFilters::filteringImageWithSeed2< Cube<unsigned char>, unsigned char, 3 >(&Iseg4, &Iseg3, &IsegB, create, numOfThreads);
//         IsegB2.save("IsegB2Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed        
/*        Cube<unsigned char> IsegB3;*/        
//         ImageFilters::filteringImageWithSeed3< Cube<unsigned char>, unsigned char, 3 >(&Iseg4, &Iseg333, &IsegB3, create, numOfThreads);
//         IsegB3.save("IsegB3Amos.mat", arma_ascii);  // segmentation 2: select blobs coincident with seed
        
        emit sendProgress(++sliceCounter); // 10
        
    //salvar todas las segmentaciones
    if(false){ 
     Iseg3.save("Iseg3.mat", arma_ascii);
     Iseg4.save("Iseg4.mat", arma_ascii);
    }

        // connecting segmenta1ion 1 with segmentation 2
        // removing blobs from segmentation 2 coincident with segmentation 1
        Cube<unsigned char> IsegC;
//         ImageFilters::filteringImageWithSeed1< Cube<unsigned char>, unsigned char, 3 >(&IsegB, &IsegA, &IsegC, create, numOfThreads);
//         IsegC.save("IsegC1Amos.mat", arma_ascii);
//         Cube<unsigned char> IsegC2;
        
        
      //  cout << "filtrar con semilla IsegB con IsegA -> IsegC" << endl;
 
                           
        ImageFilters::filteringImageWithSeed2< Cube<unsigned char>, unsigned char, 3 >(&IsegB, &IsegA, &IsegC, create, numOfThreads);
//         IsegC2.save("IsegC2Amos.mat", arma_ascii);
//         Cube<unsigned char> IsegC3;
//         ImageFilters::filteringImageWithSeed3< Cube<unsigned char>, unsigned char, 3 >(&IsegB, &IsegA, &IsegC3, create, numOfThreads);
//         IsegC3.save("IsegC3Amos.mat", arma_ascii);
        
        emit sendProgress(++sliceCounter); // 11
        
        *IsegR = IsegB - IsegC + IsegA;  // mixing segmentations
        // 	    IsegR.save("IsegRAmos.mat", arma_ascii);
        if(MINWMLSIZE > 0) 
            ImageFilters::ThresholdByLabelsSizeFilter<Cube< unsigned char>, 3>(IsegR, MINWMLSIZE);
        emit sendProgress(++sliceCounter); // 12
    }
    else {
        if(MINWMLSIZE > 0) {
            ImageFilters::ThresholdByLabelsSizeFilter<Cube< unsigned char>, 3>(&IsegA, MINWMLSIZE);
            IsegB = IsegA;
            *IsegR = IsegA;
        }
        else {
            IsegB = IsegA;
            *IsegR = IsegA;		
        }
        emit sendProgress(++sliceCounter); // 10
    }
    
    // This is the difference with Amos-test   . Esto hace que si se coge el cráneo, en 3D, se produzcan muchos artefactos
    // *IsegR = *IsegR % Iroi3D;      //mari: eliminado
    
     
        //mari:
    //salvar todas las segmentaciones
    if(false){
        WMmask3DROIBinCube.save("WMmask3DROIBinCube.mat", arma_ascii);
        FL3DROIcube.save("FL3DROIcube.mat", arma_ascii);
        Fl_x_Mask.save("Fl_x_Mask.mat", arma_ascii); 
        MaskFloat.save("MaskFloat.mat", arma_ascii);
        Iseg1.save("Iseg1.mat", arma_ascii);
        Iseg2.save("Iseg2.mat", arma_ascii);
        IsegA.save("IsegA.mat", arma_ascii);
        IsegB.save("IsegB.mat", arma_ascii);
        IsegR->save("IsegR.mat", arma_ascii);
        Iroi3D.save("Iroi3D.mat", arma_ascii);   
    }
   
}

void AmosProcessCase::computeBlobsForFeatures(Cube<unsigned char>* IsegR)
{
    if(stop) {
        // 	    stop = false;
        return;
    }	 

    
    Iseg3D_BC_blobs = *IsegR;
    // 	if(hasMask)
    // 	    Iseg3D_BC_blobs = IsegR % MASK3Dcube.slice(itS - SLICE_INI_LOOP); //Mask_analysys eliminates ischemic areas
    // 	else
    // 	    Iseg3D_BC_blobs = IsegR;
    
    // 	Iseg3D_BC_blobs.save("Iseg3D_BC_blobsAmos.mat", arma_ascii);
    
    // characterizing the blobs SEG
    emit sendProgress(++sliceCounter); // 11 13
    if(stop) {
        // 	    stop = false;
        return;
    }	
    
    const int DimensionBC = 3;
//     	vector<ImageFilters::LabelFeatures<DimensionBC> > labelsInfoBC3D;
//     	ImageFilters::BinaryLabelGeometryFilter<Cube< unsigned char>, unsigned char, DimensionBC >(&labelsInfoBC3D, &Iseg3D_BC_blobs);
//     	typename vector<ImageFilters::LabelFeatures<DimensionBC> >::iterator labelsInfoItBC = labelsInfoBC3D.begin();
//     	cout << labelsInfoBC3D.size() << " blobs" << endl;
    
    itk::ThreadProcessIdType numOfThreads = numThreads;
    bool fullConnectivity = true;
    bool calculateBlobDeep = true;	
    
    ImageFilters::BinaryLabelsFilter<Cube< unsigned char>, DimensionBC >(&labelsInfoBC3D, &Iseg3D_BC_blobs, numOfThreads, fullConnectivity, calculateBlobDeep);

    
    emit sendProgress(++sliceCounter);  // 12 14
    
}

void AmosProcessCase::blobsFeaturesLoop(int blobInit, int blobEnd)
{

    const int DimensionBC = 3;
    typename vector<ImageFilters::LabelSimpleFeatures<3> >::iterator labelsInfoItBC = labelsInfoBC3D.begin() + blobInit;
    
    int badBlobValue = featuresClasses.at(0);
    for( ; labelsInfoItBC != labelsInfoBC3D.end() - blobEnd; labelsInfoItBC++) {
        QString out = QString::number((*labelsInfoItBC).labelValue) + " " + QString::number((*labelsInfoItBC).volume);
        cout << out.toStdString() << endl;
        if((*labelsInfoItBC).volume > MINWMLSIZE) { // don't analyze very small blobs (possibly noise
            // obtains feature vector
            fvec featuresVector;
            if(modeAutomatic)
                featuresVector.set_size(featuresNumber);
            else
                featuresVector.set_size(featuresNumber + 1); // + 1 classSVM
            featuresVector.zeros();                        
            
            calculateRegionFeatures3D(&(*labelsInfoItBC), &featuresVector);                                                  
            
            Cube<unsigned char> blobsCube;
//             if(!fullRestrictedToROI) 
//                 blobsCube = zeros< Cube<unsigned char> >((int)dimImg3D[0], (int)dimImg3D[1], (int)dimImg3D[2]);
//             else
//                 blobsCube = zeros< Cube<unsigned char> >((unsigned int)(dimsWM(0, 0) - dimsWM(0, 1) + 1), (unsigned int)(dimsWM(1, 0) - dimsWM(1, 1) + 1), (unsigned int)(dimsWM(2, 0) - dimsWM(2, 1) + 1));
//             blobsCube.elem((*labelsInfoItBC).indexOfPixels) = ones< Col<unsigned char> >(size((*labelsInfoItBC).indexOfPixels));
            
            arma::Mat<itk::IndexValueType>::fixed<2, 3> boundings/* = (*labelsInfoItBC).boundingBox*/;
            
            boundings.col(0) = (*labelsInfoItBC).boundingBox.col(1);
            boundings.col(1) = (*labelsInfoItBC).boundingBox.col(0);
            boundings.col(2) = (*labelsInfoItBC).boundingBox.col(2);

            if(boundings.at(0,0) - EXCLUSIONZONE[0] - R_PERIPHERY[0] >= 0 && boundings.at(0,1) - EXCLUSIONZONE[1] - R_PERIPHERY[1] >= 0) {
                boundings.at(0,0) -= EXCLUSIONZONE[0] + R_PERIPHERY[0];
                boundings.at(0,1) -= EXCLUSIONZONE[1] + R_PERIPHERY[1];
            }
            else if(boundings.at(0,0) - EXCLUSIONZONE[0] - R_PERIPHERY[0] >= 0) {
                boundings.at(0,0) -= EXCLUSIONZONE[0] + R_PERIPHERY[0];
                boundings.at(0,1) = 0;
            }
            else if(boundings.at(0,1) - EXCLUSIONZONE[1] - R_PERIPHERY[1] >= 0) {
                boundings.at(0,0) = 0;
                boundings.at(0,1) -= EXCLUSIONZONE[1] + R_PERIPHERY[1];
            }
            else {
                boundings.at(0,0) = 0;
                boundings.at(0,1) = 0;
            }
            if(boundings.at(0,2) - EXCLUSIONZONE[2] - R_PERIPHERY[2] >= 0)
                boundings.at(0,2) -= EXCLUSIONZONE[2] + R_PERIPHERY[2];
            else
                boundings.at(0,2) = 0;
            
            if(boundings.at(1,0) + EXCLUSIONZONE[0] + R_PERIPHERY[0] < (int)Iseg3D_BC_blobs.n_rows && boundings.at(1,1) + EXCLUSIONZONE[1] + R_PERIPHERY[1] < (int)Iseg3D_BC_blobs.n_cols) {
                boundings.at(1,0) += EXCLUSIONZONE[0] + R_PERIPHERY[0];
                boundings.at(1,1) += EXCLUSIONZONE[1] + R_PERIPHERY[1];
            }
            else if(boundings.at(1,0) + EXCLUSIONZONE[0] + R_PERIPHERY[0] < (int)Iseg3D_BC_blobs.n_rows) {
                boundings.at(1,0) += EXCLUSIONZONE[0] + R_PERIPHERY[0];
                boundings.at(1,1) = Iseg3D_BC_blobs.n_cols - 1;
            }
            else if(boundings.at(1,1) + EXCLUSIONZONE[1] + R_PERIPHERY[1] < (int)Iseg3D_BC_blobs.n_cols) {                
                boundings.at(1,0) = Iseg3D_BC_blobs.n_rows - 1;
                boundings.at(1,1) += EXCLUSIONZONE[1] + R_PERIPHERY[1];
            }
            else {
                boundings.at(1,0) = Iseg3D_BC_blobs.n_rows - 1;
                boundings.at(1,1) = Iseg3D_BC_blobs.n_cols - 1;
            } 
            if(boundings.at(1,2) + EXCLUSIONZONE[2] + R_PERIPHERY[2] < (int)Iseg3D_BC_blobs.n_slices)
                boundings.at(1,2) += EXCLUSIONZONE[2] + R_PERIPHERY[2];
            else
                boundings.at(1,2) = Iseg3D_BC_blobs.n_slices - 1;

//             cout << (*labelsInfoItBC).boundingBox << endl;
//             cout << boundings << endl;            
            
            blobsCube = zeros< Cube<unsigned char> >(boundings.at(1,0) - boundings.at(0,0) + 1, boundings.at(1,1) - boundings.at(0,1) + 1, boundings.at(1,2) - boundings.at(0,2) + 1);  

            arma::Col<long long unsigned int> newIndexes;

            arma::Col<long long unsigned int> transIndexes = (*labelsInfoItBC).indexOfPixels;
//             cout << transIndexes.t() << endl;
//             newIndexes.zeros(blobsCube.n_elem);
                        
    
            uvec cociente = (transIndexes/(Iseg3D_BC_blobs.n_rows*Iseg3D_BC_blobs.n_cols) - boundings.at(0, 2)) * blobsCube.n_rows * blobsCube.n_cols;
//             cout << "cociente " << cociente.t();
            uvec resto = transIndexes - (transIndexes/(Iseg3D_BC_blobs.n_rows * Iseg3D_BC_blobs.n_cols)) * Iseg3D_BC_blobs.n_rows * Iseg3D_BC_blobs.n_cols;
//             cout << "resto " << resto.t();
            uvec cociente2 = (resto/Iseg3D_BC_blobs.n_rows - boundings.at(0, 1)) * blobsCube.n_rows;
//             cout << "cociente2 " << cociente2.t();
            uvec resto2 = resto - (resto/Iseg3D_BC_blobs.n_rows) * Iseg3D_BC_blobs.n_rows;
//             cout << "resto2 " << resto2.t() << endl;
            newIndexes = cociente + cociente2 + resto2 - boundings.at(0, 0); 
//             cout << newIndexes.t();

            blobsCube.elem(newIndexes) = ones< Col<unsigned char> >(arma::size((*labelsInfoItBC).indexOfPixels));             
                        
            Cube<unsigned char> Mdil;
            //ImageFilters::DilateDiskMatrixFilter2D(&blobsCube, &Mdil, EXCLUSIONZONE);
            
//             cout << "ErodeDilateBallArmaFilter blobsCube" << endl;
            ImageFilters::ErodeDilateBallArmaFilter<Cube<unsigned char>, unsigned char, DimensionBC>(ImageFilters::Dilate, &blobsCube, &Mdil, EXCLUSIONZONE);
            Cube<unsigned char> Mper;
            // 		int radius = max(MIN_R_PERIPHERY, (int)round((*labelsInfoItBC).minorAxisLength));
            //ImageFilters::DilateDiskMatrixFilter2D(&Mdil, &Mper, R_PERIPHERY);
            
//             cout << "ErodeDilateBallArmaFilter Mper" << endl;
            ImageFilters::ErodeDilateBallArmaFilter<Cube<unsigned char>, unsigned char, DimensionBC>(ImageFilters::Dilate, &Mdil, &Mper, R_PERIPHERY);
            Mper = Mper - Mdil;
            
            /*calculatePerimeterFeatures3D(&Mper, &featuresVector);*/
            calculatePerimeterFeatures3D(&Mper, boundings, &featuresVector);
            
            if(modeAutomatic) { // use classifier to eliminate false blobs
                // 		    cout << featuresVector.t();
                scaleFeatures(&featuresVector);
                // 		    cout << featuresVector.t();
                
//                 if(numThreads > 1)
//                     mutex.lock();
                classifier->setData(featuresVector);
                vec classifierResults = classifier->predict();
//                 if(numThreads > 1) {
//                     mutex.unlock();
//                     wait();
//                 }
                
                bool correct = (int)round(classifierResults(0)) == badBlobValue ? false : true;
                if(!correct)
                    Iseg3D_AC_blobs.elem((*labelsInfoItBC).indexOfPixels) = zeros< Mat<unsigned char> >(size((*labelsInfoItBC).indexOfPixels));  // delete blob from output
//                 cout << classifierResults.t();
            }
            else { // for classifier training
//                 double classSVM = intersectBlobWML<Cube<unsigned char>, DimensionBC, ImageFilters::LabelFeatures<DimensionBC> >(&ANOT3Dcube, &Iseg3D_BC_blobs, &(*labelsInfoItBC)) ? 1 : 0;
                double classSVM = intersectBlobWML<Cube<unsigned char>, DimensionBC, ImageFilters::LabelSimpleFeatures<DimensionBC> >(&ANOT3Dcube, &Iseg3D_BC_blobs, &(*labelsInfoItBC)) ? 1 : 0;
                featuresVector(featuresNumber) = classSVM;
                // 		    cout << featuresVector.t();	
                addClassificationLine(featuresVector);
            }
        }
        else
            Iseg3D_AC_blobs.elem((*labelsInfoItBC).indexOfPixels) = zeros< Mat<unsigned char> >(size((*labelsInfoItBC).indexOfPixels));  // delete blob from output
    }

    
    // 	Iseg3D_BC.slice(itS).save("Iseg3D_BC_blobsAmos.mat", arma_ascii);    
}

void AmosProcessCase::overwriteACBC3D()
{
    emit sendProgress(++sliceCounter);  // 13 15
    if(!fullRestrictedToROI) {
        Iseg3D_BC = Iseg3D_BC_blobs;
        Iseg3D_AC = Iseg3D_AC_blobs; 
    }
    else {
        Iseg3D_BC.subcube(dimsWM(0, 1), dimsWM(1, 1), dimsWM(2, 1), dimsWM(0, 0), dimsWM(1, 0), dimsWM(2, 0)) = Iseg3D_BC_blobs;
        Iseg3D_AC.subcube(dimsWM(0, 1), dimsWM(1, 1), dimsWM(2, 1), dimsWM(0, 0), dimsWM(1, 0), dimsWM(2, 0)) = Iseg3D_AC_blobs;
    }       
}

void AmosProcessCase::mainLoop3D()
{
//     double Ks1, Kth1, Ks2, Kth2;
    
//     startMainLoop3D();
    InternalProcessCase::mainLoop3D();

//     Cube<unsigned char> Iroi3D;
//     Cube<unsigned char> WMmask3DROIBinCube;
//     Cube<unsigned char> WMmask3DModelBinCube;
    computeIROI3D();
//     Cube<unsigned char> Ich3D;
    Ich3D.set_size(size(Iroi3D));
    IroiHasForeground3D = Utils::isNotZero< Cube<unsigned char> >(&Iroi3D) ? ImageFilters::YES : ImageFilters::NO;
    computeRegionProps3D(initialLoopSlice, endLoopSlice);
    
    // Distance to convexhull of the region of interest
    // Used to classify WML as function of periphery distance.    
//     fcube DistToCHULL3D;
//     fcube DistToBorder3D;
    
    //fcube DistToWMBorder;
    //computeDistsTo(&DistToWMBorder);
    computeDistsTo();
    
    
    
    // *******************  Step 1: SEGMENTATION  *******************
    
    Cube<unsigned char> IsegR;
    computeISEGR(&IsegR);
    
    // ********************* Step 2: CLASIFICATION *********************
    
    // ***************** Step 2.1  COMPUTING BLOB FEATURES ****************************
    
    // slice segmentations: 1) before (initial segmentation) and 2) after the classifier (final segmentation)
    
//     Cube<unsigned char>Iseg3D_AC_blobs;
//     Cube<unsigned char>Iseg3D_BC_blobs;
//     const int DimensionBC = 3;
//     vector<ImageFilters::LabelSimpleFeatures<DimensionBC> > labelsInfoBC3D;
    computeBlobsForFeatures(&IsegR);
    
    Iseg3D_AC_blobs = Iseg3D_BC_blobs;
    
    blobsFeaturesLoop(/*, int blobInit, int blobEnd*/);

    overwriteACBC3D();

}

void AmosProcessCase::preMainLoop()
{
    auto beginThres = chrono::high_resolution_clock::now();
    
    numSlicesWithLBWM3DMinSize = 0;
    
    caseProgress = 2;
    progressType = tr("Thresholding");
    progressRange = SLICE_END_LOOP - SLICE_INI_LOOP + 1;
    QTimer::singleShot(0, this, SLOT(changeProgres()));     
    
    // Global histogram analisys to obtain parameters for thresholding
    // These parameters could be global or slice dependent
    sliceCounter = 0;    
    
    vectorsForScanSegmentTH();
    thresholdsLoop(initialLoopSlice, endLoopSlice);
    
    if(numSlicesWithLBWM3DMinSize == 0) {
        emit sendNotEnoughWM(subjectName);
        finishedOk = false;
//         executionFinished(false);
        stop = true;
//         quit();
        return;
    }
    
    calculateThresholds();
    
    if(stop) {
        //     	stop = false;
        return;
    }           
    
    auto endThres = chrono::high_resolution_clock::now();
    if(Utils::showTime)
        emit sendLog(tr("Thresholding") + " " + QString::number(chrono::duration_cast<chrono::nanoseconds>(endThres - beginThres).count()/1000000.0) + "ms.");
    
    caseProgress = 3;
    progressType = tr("Segmenting");
    if(!mode3D) {
        progressRange = SLICE_END_LOOP - SLICE_INI_LOOP + 1;
        QTimer::singleShot(0, this, SLOT(changeProgres()));
    }    
}

void AmosProcessCase::calculateThresholds() 
{
//         cout << "THwm_noiseLevel 1 " << THwm_noiseLevel.t() << endl;
//         cout << "THwm_firstJumpNObjs 1 " << THwm_firstJumpNObjs.t() << endl;    
    
    // Assigning global parameters, medians to favor most habitual values
    K3D_wm_noiseLevel = median(THwm_noiseLevel.elem(find(THwm_noiseLevel > vnl_math::eps)));
    K3D_wm_highestJumpNObjs = median(THwm_highestJumpNObjs.elem(find(THwm_highestJumpNObjs > vnl_math::eps)));
    K3D_wm_firstJumpObjs = median(THwm_firstJumpNObjs.elem(find(THwm_firstJumpNObjs > vnl_math::eps)));
    K3D_wm_areaSeg = median(THwm_areaObj.elem(find(THwm_areaObj > vnl_math::eps)));
    
    K3D_gm_highestJumpNObjs = median(THgm_highestJumpNObjs.elem(find(THgm_highestJumpNObjs > vnl_math::eps)));
    K3D_gm_firstJumpObjs = median(THgm_firstJumpNObjs.elem(find(THgm_firstJumpNObjs > vnl_math::eps)));
    K3D_gm_areaSeg = median(THgm_areaObj.elem(find(THgm_areaObj > vnl_math::eps)));
    
    THwm_noiseLevel.elem(find(THwm_noiseLevel <= vnl_math::eps)) += K3D_wm_noiseLevel;
    THwm_highestJumpNObjs.elem(find(THwm_highestJumpNObjs <= vnl_math::eps)) += K3D_wm_highestJumpNObjs; 
    THwm_firstJumpNObjs.elem(find(THwm_firstJumpNObjs <= vnl_math::eps)) += K3D_wm_firstJumpObjs;
    THwm_areaObj.elem(find(THwm_areaObj <= vnl_math::eps)) += K3D_wm_areaSeg;
    
    THgm_highestJumpNObjs.elem(find(THgm_highestJumpNObjs <= vnl_math::eps)) += K3D_gm_highestJumpNObjs; 
    THgm_firstJumpNObjs.elem(find(THgm_firstJumpNObjs <= vnl_math::eps)) += K3D_gm_firstJumpObjs;
    THgm_areaObj.elem(find(THgm_areaObj <= vnl_math::eps)) += K3D_gm_areaSeg;   
    
}


void AmosProcessCase::setOutputNames(QList<QString> outputNames)
{
    this->ACName = outputNames.at(0);
    this->BCName = outputNames.at(1);
}

void AmosProcessCase::saveResults()
{
    //     emit sendCaseProgress(4);
    
    if(!modeAutomatic) {        
        writeClassification(); 
        emit sendCaseProgress(5);
    }
    else {   
        DataOrientation::OrientationData oData;
        bool avoidReorientation = imagesPreviouslyLoaded;
        if(imagesPreviouslyLoaded) {
            mitk::PropertyList::Pointer propertyList = orgFL3D->GetPropertyList();
            bool emptyList = propertyList->IsEmpty();
            QString FLpath;
            if(!emptyList)                            
                FLpath = QString::fromStdString(orgFL3D->GetProperty("path")->GetValueAsString());
            else
                // Image not loaded but generated by another process (e.g. contrast enhancement)
                FLpath = "Flair_enhanced.nii";
            QFileInfo fileF(outputDirectory, FLpath);
            
            if(emptyList) { 
                arma::fcube newFL3Dcube;
                revertSegmentedCubeOrientationSampling<float>(&FL3Dcube, &newFL3Dcube, avoidReorientation);
                FL3D = mitk::Image::New();
                FL3D->Initialize(orgFL3D);
                FL3D->SetVolume(newFL3Dcube.memptr());
//                 IOUtil::Save(orgFL3D, fileF.absoluteFilePath().toStdString());
            }
            else
                FL3D = AmosItkImageIO::LoadAmosNifti(FLpath.toStdString());
            
            oData.icod = icod;
            oData.jcod = jcod;
            oData.kcod = kcod;
            oData.reorderI = reorderI;
            oData.reorderJ = reorderJ;
            oData.reorderK = reorderK;
            AmosItkImageIO::addImageToList(FL3D, oData);
            // 	    AmosNiftiImageIO::Pointer amosNiftiIO = AmosNiftiImageIO::New();
            // 	    AmosItkImageIO* niftiIO = new AmosItkImageIO(mitk::IOMimeTypes::NIFTI_MIMETYPE(), amosNiftiIO.GetPointer(), 0);
            // 	    niftiIO->mitk::AbstractFileReader::SetInput(orgFL3D->GetProperty("path")->GetValueAsString());
            // 	    std::vector<itk::SmartPointer<mitk::BaseData> > result = niftiIO->Read();
            // 	    mitk::BaseData::Pointer baseData= result[0];
            // 	    FL3D = dynamic_cast<mitk::Image*> (baseData.GetPointer());
            emit sendSaveImage(FL3D, fileF);
            // 	    delete niftiIO;	    
        }                       
        
        QFileInfo fileB(outputDirectory, BCName);
        revertSegmentedCubeOrientationSampling<uchar>(&Iseg3D_BC, &newIseg3D_BC, avoidReorientation);
        
//         string fileName("AmosBCcubenew.nii");
//         Image::Pointer AmosBCImg = Utils::cubeToMITKImage(&newIseg3D_BC);    
//         IOUtil::Save(AmosBCImg, fileName);         
        
        Amos_BC_Image->SetVolume(newIseg3D_BC.memptr());
        Amos_BC_Image->SetProperty("descrip", StringProperty::New("Amos_BC"));
        if(imagesPreviouslyLoaded)
            AmosItkImageIO::addImageToList(Amos_BC_Image, oData);        
        
        emit sendSaveImage(Amos_BC_Image, fileB);
        
        QFileInfo fileA(outputDirectory, ACName);
        revertSegmentedCubeOrientationSampling<uchar>(&Iseg3D_AC, &newIseg3D_AC, avoidReorientation);
        Amos_AC_Image->SetVolume(newIseg3D_AC.memptr());
        Amos_AC_Image->SetProperty("descrip", StringProperty::New("Amos_AC"));
        if(imagesPreviouslyLoaded)
            AmosItkImageIO::addImageToList(Amos_AC_Image, oData);         
        
        emit sendSaveImage(Amos_AC_Image, fileA);
    }    
}

void AmosProcessCase::generateLBWM3D()
{
    uvec P = find(LB3Dcube > 255);
    LB3Dcube.elem(P).fill(240);
    
    // WM labels
    bool wholeCube = true;
    Col<unsigned int> labelsWM;
    labelsWM << 2 << 41 << 77 << 78 << 79 << 100 << 109 << 213 << 221 << 250 << 251 << 252 << 253 << 254 << 255;    
//     Col<unsigned int> labelsWM = {2, 41, 77, 78, 79, 100, 109, 213, 221, 250, 251, 252, 253, 254, 255};
    WMMask3DModelCube = labelsCube(labelsWM, wholeCube);    
}

/**
 * @brief Calculate WM volume and distance map to its center
 * 
 * @return void
 */
void AmosProcessCase::calculateWMVol()
{
//     LB3Dcube.reset();
    //WMMask3DModelCube.reset();
    dimsWM = limitsData3D(WMMask3DROICube);
    // region of interest for analysis
    if(fullRestrictedToROI) {
        dimsWM(0,1) = dimsWM(0,1) < (int)WMDisk10Diameter[0] + 1 ? 0 : dimsWM(0,1) - ((int)WMDisk10Diameter[0] + 1);
        dimsWM(1,1) = dimsWM(1,1) < (int)WMDisk10Diameter[1] + 1 ? 0 : dimsWM(1,1) - ((int)WMDisk10Diameter[1] + 1);
        dimsWM(0,0) = dimsWM(0,0) >  (int)dimImg3D[0] - 1 - ((int)WMDisk10Diameter[0] + 1) ? (int)dimImg3D[0] - 1: dimsWM(0,0) + (int)WMDisk10Diameter[0] + 1;
        dimsWM(1,0) = dimsWM(1,0) >  (int)dimImg3D[1] - 1 - ((int)WMDisk10Diameter[1] + 1) ? (int)dimImg3D[1] - 1: dimsWM(1,0) + (int)WMDisk10Diameter[1] + 1;
    }
    
    int SLICE_INI_WM = dimsWM(2, 1);
    int SLICE_END_WM = dimsWM(2, 0);
    
    cout << dimsWM << endl;
    
    if(SLICE_INI_LOOP == -1)
        SLICE_INI_LOOP = SLICE_INI_WM;
    if(SLICE_END_LOOP == -1)
        SLICE_END_LOOP = SLICE_END_WM;
    
    initialLoopSlice = SLICE_INI_LOOP;
    endLoopSlice = SLICE_END_LOOP;    
    
    THwm_noiseLevel.zeros(SLICE_END_LOOP - SLICE_INI_LOOP + 1);
    THwm_highestJumpNObjs.zeros(SLICE_END_LOOP - SLICE_INI_LOOP + 1);
    THwm_firstJumpNObjs.zeros(SLICE_END_LOOP - SLICE_INI_LOOP + 1);
    THwm_areaObj.zeros(SLICE_END_LOOP - SLICE_INI_LOOP + 1);     
    THgm_highestJumpNObjs.zeros(SLICE_END_LOOP - SLICE_INI_LOOP + 1);
    THgm_firstJumpNObjs.zeros(SLICE_END_LOOP - SLICE_INI_LOOP + 1);
    THgm_areaObj.zeros(SLICE_END_LOOP - SLICE_INI_LOOP + 1);
    
}

Cube<unsigned char> AmosProcessCase::labelsCube(Col<unsigned int> labels, bool whole)
{
    Cube<unsigned char> cube;
    if(whole)
        cube = zeros< Cube<unsigned char> >((int)dimImg3D[0], (int)dimImg3D[1], (int)dimImg3D[2]);
    else {
        if(!fullRestrictedToROI)
            cube = zeros< Cube<unsigned char> >((int)dimImg3D[0], (int)dimImg3D[1], SLICE_END_LOOP - SLICE_INI_LOOP + 1);
//             cube = zeros< Cube<unsigned char> >((int)dimImg3D[0], (int)dimImg3D[1], (int)dimImg3D[2]);
        else
            cube = zeros< Cube<unsigned char> >(dimsWM(0, 0) - dimsWM(0, 1) + 1, dimsWM(1, 0) - dimsWM(1, 1) + 1, SLICE_END_LOOP - SLICE_INI_LOOP + 1);
    }
    unsigned int numLabels = labels.n_elem;
    for(unsigned int itE = 0; itE < numLabels; itE++) {
        uvec pixelsWM = find(LB3Dcube == labels(itE));
        cube(pixelsWM).fill(1);
    }
    return cube;
}

/**
 * @brief return limits for non-zero data in labels 3Dimage . Format (row,col,slice)
 * dims = [rmax, rmin; cmax, cmin; smax, smin].
 * 
 * @param I cube associated to image data
 * @return Mat< int > limits for non-zero data
 */

Mat<int> AmosProcessCase::limitsData3D(Cube<unsigned char> I)
{
    uvec ind = find(I);
    umat t = ind2sub(size(I), ind);
    Mat<int> D;
    if(t.n_elem > 0)
        D << max(t.row(0)) << min(t.row(0)) << endr << max(t.row(1)) << min(t.row(1)) << endr << max(t.row(2)) << min(t.row(2));
    else
        D.zeros(3, 2);
//     cout << D;
//     cout << D1;
    return D;
    
}

void AmosProcessCase::flair3DAnalisys()
{
    LevenbergMarquardt lm;
    
    int Ri = 0;
    int Rs = NBINS - 1;    
    fcube FL3DWM = FL3DROIcube % WMMask3DModelCube;
    
    fvec FL3DWM_vec = fvec(FL3DWM.memptr(), FL3DWM.n_elem, false, true);
    uvec HS_FLwm3D = hist(FL3DWM_vec, histoCenters); 
    //	For testing	    
//     cout << HS_FLwm3D.n_elem << endl << endl;
//     cout << sum(HS_FLwm3D) << endl << endl;
//     cout << HS_FLwm3D.t() << endl;
    
    // histogram normalization
    HS_FLwm3D(0) = 0;    
    fvec HN_FLwm3D = conv_to<fvec>::from(HS_FLwm3D)/(float)sum(HS_FLwm3D);
    
    //mean and std WM     
    fvec meanStd = lm.fitPeak<float>(posH(span(Ri, Rs)), HN_FLwm3D(span(Ri, Rs)), NPOINTSGAUSSIANAPROX);
    meanWM3D = meanStd(0);
    stdWM3D = meanStd(1);
    
//     cout << setprecision(8) << "meanWM3D " << meanWM3D << " stdWM3D " << stdWM3D << endl;
    
}

void AmosProcessCase::setStructuresParameters()
{
    // different parameters of WM and thresholds: based on characteristics of WM, GM the whole brain
    p_Iseg.meanWM3D = meanWM3D;
    p_Iseg.stdWM3D = stdWM3D;
    
    // image analysis
    p_Scan.NBINS = NBINS;
    p_Scan.mean = p_Iseg.meanWM3D;
    p_Scan.std = p_Iseg.stdWM3D;
    p_Scan.Kh = 7;
    p_Scan.Kl = 2;
    p_Scan.JUMPTHRESBLOBS = 9;
    p_Scan.AREA = 20;   
}

void AmosProcessCase::vectorsForScanSegmentTH()
{
    double K_start = 3.6;
    double K_end = 2.8;
    double K_spacing = 0.1;
    K_size = (unsigned int)round(((K_start - K_end) / K_spacing)) + 1;
    
    K_values = linspace<vec>(K_start, K_end, K_size);
    cuts << 0 << 6 << 25 << 40 << 10000;
    numBlobs_until.copy_size(cuts);    
}

// barridoSegmentarColaHistograma.m
// For histogram analisys from a very high K value to a very low one
AmosProcessCase::ResultScanTailHisto AmosProcessCase::scanSegmentTailHistogram(Mat< float >* matrixRMI, Mat< unsigned char >* matrixBIN)
{
    
    fmat matrixBINFloat = conv_to<Mat<float> >::from(*matrixBIN);
    ResultScanTailHisto result = scanSegmentTailHistogram(matrixRMI, &matrixBINFloat);
    
    return result;
    
}

// For histogram analisys from a very high K value to a very low one
AmosProcessCase::ResultScanTailHisto AmosProcessCase::scanSegmentTailHistogram(Mat< float >* matrixRMI, Mat< float >* matrixBINFloat)
{
    ResultScanTailHisto result;
    
    fmat RMI_x_BIN = *matrixRMI % *matrixBINFloat;
    
    uvec ind = find(RMI_x_BIN);
    double areaAnalisys = ind.n_elem / 1000.0 / (LBWM3DMinSizeFactor * LBWM3DMinSizeFactor);    
    
    result.table = zeros< Mat<double> >(K_values.n_elem, 6);
    
    for(uword itK = 0; itK < K_size; itK++) {
        
        double K = K_values(itK);
        double TH = meanWM3D + K * stdWM3D;
        
        Mat<unsigned char> Iseg;
        bool create = true;
        Utils::binarizeArma< Mat<float>, Mat<unsigned char> >(&RMI_x_BIN, &Iseg, TH, 1, create);
        uvec onesIseg = find(Iseg); // sum(Iseg(:)); more efficient that converting Iseg to a big Mat type and performing accu()
        uword S = onesIseg.n_elem;
        // 	cout << "S " << S << endl;
        if(S) {	    
            vector<ImageFilters::LabelSimpleFeatures<2> > labelsSimpleInfo;
            ImageFilters::BinaryLabelsFilter<Mat< unsigned char>, 2 >(&labelsSimpleInfo, &Iseg);	    
            size_t numBlobs = labelsSimpleInfo.size();
            
            typename vector<ImageFilters::LabelSimpleFeatures<2> >::iterator labelsSimpleInfoIt;
            Col<itk::SizeValueType> areaBlob(numBlobs);
            
            size_t i = 0;
            for( labelsSimpleInfoIt = labelsSimpleInfo.begin(); labelsSimpleInfoIt != labelsSimpleInfo.end(); labelsSimpleInfoIt++, i++) 
                areaBlob(i) = (*labelsSimpleInfoIt).volume;
            
            for( i = 0; i < cuts.n_elem; i++) {
                uvec Blobs_until = find(areaBlob < cuts(i));
                numBlobs_until(i) = Blobs_until.n_elem;
            }
            
            result.table(itK, 0) = TH;
            result.table(itK, 1) = K;
            result.table(itK, 2) = numBlobs;
            result.table(itK, 3) = S;
            result.table(itK, 4) = numBlobs_until(1) / areaAnalisys;
            result.table(itK, 5) = numBlobs_until(numBlobs_until.n_elem - 1) / areaAnalisys;
        }
        else {
            result.table(itK, 0) = TH;
            result.table(itK, 1) = K;

            for(uint i = 2; i <= 5; i++)
                result.table(itK, i) = 0;
        }
    } 
    
    uvec idx = find(result.table(span(0, K_values.n_elem - 1),  4) > K_noiseLevelFindLimit);
    if(idx.n_elem == 0)
        result.K_noiseLevel = K_noiseLevelDefault;
    else
        result.K_noiseLevel = result.table(idx(0), 1);
    
    Col<double> numBlobsJumps(result.table.n_rows - 1);
    for(unsigned int i = 0; i < result.table.n_rows - 1; i++)
        numBlobsJumps(i) = result.table(i + 1, 2) - result.table(i, 2);
    
    // highest jump in blobs number
    double vmax = numBlobsJumps.max();
    if((int)round(vmax) > 0) {
        Col<double> difference = (vmax - numBlobsJumps) / vmax;
        uvec ind = find(difference < 0.1);
        uword b = min(ind);
        if(numBlobsJumps(b) > 2)
            result.K_highestJumpNBlobs = result.table(b, 1);
        else
            result.K_highestJumpNBlobs = p_Scan.Kl;	
    }
    else
        result.K_highestJumpNBlobs = p_Scan.Kl;
    
    // big jump in blobs number starting at TH2
    uvec indJumpTH = find(numBlobsJumps > p_Scan.JUMPTHRESBLOBS);
    if(indJumpTH.n_elem > 0)
        result.K_firstJumpNBlobs = result.table(indJumpTH(0), 1);
    else
        result.K_firstJumpNBlobs = result.K_highestJumpNBlobs;
    
    // first TH con area bigger than result.AREA (ah? there is something)
    uvec indArea = find(result.table.col(3) > p_Scan.AREA);
    if(indArea.n_elem > 0)
        result.K_areaSeg = result.table(indArea(0), 1);
    else
        result.K_areaSeg = p_Scan.Kl;
    
    return result;
    
}

// **regionfeatures**distTo**  // caracteristicas version 2D
void AmosProcessCase::calculateRegionFeatures(arma::fmat* FL, arma::fmat* T1, arma::fmat* DistToWMBorder, arma::fmat* DistToCHULL, ImageFilters::LabelSimpleFeatures<2>* blobInfo, int slice, fvec* featuresVector)
{
    
    fvec blobDistToWMBorder = DistToWMBorder->elem(blobInfo->indexOfPixels);
    //fvec absBlobDistToWMBorder = abs(blobDistToWMBorder);  // se ha quitado el abs porque el signo es significativo (o el blob está dentro o fuera de la WM)
    //float meanDistToWMBorder = mean(absBlobDistToWMBorder);    
    float meanDistToWMBorder = mean(blobDistToWMBorder);
    (*featuresVector)(0) = meanDistToWMBorder;            // 
    
//     cout << "analisis de meanDistToWMBorder" << endl;
//     cout << "valor medio = " << meanDistToWMBorder << endl;
//     cout << "valores individuales = " << endl;
    
//      for(int i = 0; i < blobDistToWMBorder.size(); i++) 
//         cout << blobDistToWMBorder[i] << endl;
  
    
//      cout << "spacing" << outSpacing3D[0] << ", " << outSpacing3D[1] << ", " << outSpacing3D[2] << endl; 
    
    fvec blobDistToCHULL = DistToCHULL->elem(blobInfo->indexOfPixels);
    fvec absBlobDistToCHULL = abs(blobDistToCHULL);
    float meanDistToCHULL = mean(absBlobDistToCHULL);
    (*featuresVector)(1) = meanDistToCHULL;    

//       cout << "convexhull" <<  endl; 
//    
//      for(int i = 0; i < absBlobDistToCHULL.size(); i++) 
//         cout << absBlobDistToCHULL[i] << endl;

    float distz = (float)(slice - dimsWM(2, 1)) / (dimsWM(2, 0) - dimsWM(2, 1));
    (*featuresVector)(2) = distz; 
    
    (*featuresVector)(3) = blobInfo->blobDeep;
    
    fvec regFL = FL->elem(blobInfo->indexOfPixels);
    uvec ind = find(regFL >= thresholdLevelMateria);
    regFL = regFL.elem(ind); // replace product with Imateria
    float meanFL = mean(regFL);    
    
    fvec regT1 = T1->elem(blobInfo->indexOfPixels);
    regT1 = regT1.elem(ind); // replace product with Imateria
    float meanT1 = mean(regT1);
    
    // for passing to calculatePerimeterFeatures() and then complete calculations
    (*featuresVector)(4) = meanFL;
    (*featuresVector)(5) = meanT1;
   
//     (*featuresVector)(6) = blobInfo->volume*outSpacing3D[0]*outSpacing3D[1];
    
}

void AmosProcessCase::calculatePerimeterFeatures(arma::fmat* FL, arma::fmat* T1, Mat< unsigned char >* Mper, fvec* featuresVector)
{
    Col<uword> indexOfPixels = find(*Mper);
    fvec perFL = FL->elem(indexOfPixels);
//     cout << perFL.t() << endl;
    uvec ind = find(perFL >= thresholdLevelMateria);
    perFL = perFL.elem(ind); // replace product with Imateria
    if(perFL.n_elem == 0)
        return;
    float meanFL = mean(perFL);
    
    fvec perT1 = T1->elem(indexOfPixels);
    perT1 = perT1.elem(ind); // replace product with Imateria
    if(perT1.n_elem == 0)
        return;
    float meanT1 = mean(perT1);     
    //  complete calculations from calculateRegionFeatures()
    (*featuresVector)(4) = WEIGHT_CLASSIFIER_FL * ((*featuresVector)(4) - meanFL)/(*featuresVector)(4);
    (*featuresVector)(5)= ((*featuresVector)(5) - meanT1)/(*featuresVector)(5);    
}

void AmosProcessCase::calculatePerimeterFeatures(Mat< float >* FL, Mat< float >* T1, Mat< unsigned char >* Mper, arma::Mat<itk::IndexValueType>::fixed< 2, 2 > boundings, Col< float >* featuresVector)
{
    Col<uword> indexOfPixels = find(*Mper);
//     cout << indexOfPixels.t() << endl;    
    Col<uword> indexInImage;
    unsigned int nBlobRows = boundings.at(1,0) - boundings.at(0,0) + 1;
//     cout << indexOfPixels.t() << endl;
    indexInImage = (indexOfPixels / nBlobRows + boundings.at(0, 1)) * FL->n_rows;
//     cout << indexInImage.t() << endl;
    indexInImage += boundings.at(0, 0) + indexOfPixels - (indexOfPixels/nBlobRows) * nBlobRows;
//     cout << indexInImage.t() << endl;
    
    fvec perFL = FL->elem(indexInImage);
//     cout << perFL.t() << endl;
    uvec ind = find(perFL >= thresholdLevelMateria);
    perFL = perFL.elem(ind); // replace product with Imateria
    if(perFL.n_elem == 0)
        return;
    float meanFL = mean(perFL);
    
    fvec perT1 = T1->elem(indexInImage);
    perT1 = perT1.elem(ind); // replace product with Imateria
    if(perT1.n_elem == 0)
        return;
    float meanT1 = mean(perT1);     
    //  complete calculations from calculateRegionFeatures()
    (*featuresVector)(4) = WEIGHT_CLASSIFIER_FL * ((*featuresVector)(4) - meanFL)/(*featuresVector)(4);
    (*featuresVector)(5)= ((*featuresVector)(5) - meanT1)/(*featuresVector)(5); 
    
//     cout << featuresVector->t();
}   

// caracteristicas version 3D
void AmosProcessCase::calculateRegionFeatures3D(ImageFilters::LabelSimpleFeatures<3>* blobInfo, fvec* featuresVector)
// void AmosProcessCase::calculateRegionFeatures3D(arma::fcube* FL, arma::fcube* T1, arma::fcube* DistToBorder3D, arma::fcube* DistToCHULL3D, ImageFilters::LabelFeatures<3>* blobInfo, fvec* featuresVector)
{
    
    // numFeaturesSVM :  ajustar la variable numFeaturesSVM en función del número de variables 
    // (*featuresVector)(0) = meanDistToWMBorder;
    // (*featuresVector)(1) = meanDistToCHULL;    
    // (*featuresVector)(2) = distz; 
    // (*featuresVector)(3) = blobInfo->blobDeep;    
    // (*featuresVector)(4) = meanFL; 
    // (*featuresVector)(5) = meanT1;   
    // (*featuresVector)(6) = volume;   
    // (*featuresVector)(7) = area/volume (da idea de roundness);   
    
    
    fvec blobDistToWMBorder = DistToWMBorder3D.elem(blobInfo->indexOfPixels);
    //fvec absBlobDistToWMBorder = abs(blobDistToWMBorder);   // el signo es significativo
    //float meanDistToWMBorder = mean(absBlobDistToWMBorder);
    float meanDistToWMBorder = mean(blobDistToWMBorder);
    (*featuresVector)(0) = meanDistToWMBorder;
    
//     cout << "analisis de meanDistToWMBorder" << endl;
//     cout << "valor medio = " << meanDistToWMBorder << endl;
//     cout << "valores individuales = " << endl;
//     
//      for(int i = 0; i < blobDistToWMBorder.size(); i++) 
//         cout << blobDistToWMBorder[i] << endl;

    fvec blobDistToCHULL = DistToCHULL3D.elem(blobInfo->indexOfPixels);
    fvec absBlobDistToCHULL = abs(blobDistToCHULL);
    float meanDistToCHULL = mean(absBlobDistToCHULL);
    (*featuresVector)(1) = meanDistToCHULL;    
    
//       for(int i = 0; i < absBlobDistToCHULL.size(); i++) 
//         cout << absBlobDistToCHULL[i] << endl;
    
 
    // OJO: el centroide se calcula con el tamaño de la ROI y las dimsWM con el tamaño total del volumen
//     cout << "centroid: " << blobInfo->centroid[0] << ", " << blobInfo->centroid[1] << ", " << blobInfo->centroid[2] << endl;
//     cout << "dimsWMx: " << dimsWM(0, 0) << ", " << dimsWM(0, 1) << endl;
//     cout << "dimsWMy: " << dimsWM(1, 0) << ", " << dimsWM(1, 1) << endl;
//     cout << "dimsWMz: " << dimsWM(2, 0) << ", " << dimsWM(2, 1) << endl;
//     
//     cout << "outSpacing3D: " << outSpacing3D << endl;
//     cout << "blob surface / volume: " << blobInfo->surface << " / " << blobInfo->volume << endl;
 
    float distz = (float)(blobInfo->centroid(2) / (dimsWM(2, 0) - dimsWM(2, 1))); //dimsWM: matriz 3 filas (x,y,z), 2 cols: (max,min)
    (*featuresVector)(2) = distz; 

    (*featuresVector)(3) = blobInfo->blobDeep;
    
    fvec regFL = FL3DROIcube.elem(blobInfo->indexOfPixels);
    uvec ind = find(regFL >= thresholdLevelMateria);
    regFL = regFL.elem(ind); // replace product with Imateria
    float meanFL = mean(regFL);    
    
    fvec regT1 = T13Dcube.elem(blobInfo->indexOfPixels);
    regT1 = regT1.elem(ind); // replace product with Imateria
    float meanT1 = mean(regT1);
    
    // for passing to calculatePerimeterFeatures() and then complete calculations
    (*featuresVector)(4) = meanFL;
    (*featuresVector)(5) = meanT1;
    
//     (*featuresVector)(6) = blobInfo->volume*outSpacing3D[0]*outSpacing3D[1]*outSpacing3D[2];
//     
// // distancia máxima al centro en x,y,z   
//     (*featuresVector)(11) = D(0);
//     (*featuresVector)(12) = D(1);
//     (*featuresVector)(13) = D(2);
//  
// 
//     if(t.n_elem > 0)
//         D << (float)(arma::min(t.row(0))) - (float)WMCM(0) << (float)(arma::min(t.row(1))) - (float)(WMCM(1)) << (float)(arma::min(t.row(2))) - (float)(WMCM(2));
//     else
//         D = {10000, 10000, 10000};  
// 
//         cout << D << endl;
// 
//     
//     // distancia máxima al centro en x,y,z   
//     (*featuresVector)(14) = D(0);
//     (*featuresVector)(15) = D(1);
//     (*featuresVector)(16) = D(2);
//     
// 
//     if(t.n_elem > 0)
//         D << (float)(arma::max(t.row(0))) - (float)WMCM(0) << (float)(arma::max(t.row(1))) - (float)(WMCM(1)) << (float)(arma::max(t.row(2))) - (float)(WMCM(2));
//     else
//         D = {10000, 10000, 10000};  
//     
//         cout << D << endl;
// 
//     
// // distancia máxima al centro en x,y,z   
//     (*featuresVector)(17) = D(0);
//     (*featuresVector)(18) = D(1);
//     (*featuresVector)(19) = D(2);
//     
//         cout << D << endl;
//         int kk = 0;  
// 
// 
// (*featuresVector)(20) = (float) atoi(subjectName.toUtf8());
//     
//  
// 
//     cout << "size vector = " << (*featuresVector).size()  << endl;
//     cout << endl << "RESULTADOS: " << endl;    
//     for(int i = 0; i < (*featuresVector).size(); i++) 
//         cout << (*featuresVector)(i) << endl;
//     
//      kk = 0;  
//     
//     
// //     DONE: POR QUÉ NO SEGMENTA LA PARTE DE LA WMH QUE ESTÁ FUERA DE WM, EN GM: esto ocurre solo en los slices que no tienen WM en el plano Z debido a la definición de la ROI. No es preocupante.   
//    
//     */  

}

void AmosProcessCase::calculatePerimeterFeatures3D(Cube< unsigned char >* Mper, fvec* featuresVector)
{
    Col<uword> indexOfPixels = find(*Mper);
    fvec perFL = FL3DROIcube.elem(indexOfPixels);
    uvec ind = find(perFL >= thresholdLevelMateria);
    perFL = perFL.elem(ind); // replace product with Imateria
    if(perFL.n_elem == 0)
        return;
    float meanFL = mean(perFL);
    
    fvec perT1 = T13Dcube.elem(indexOfPixels);
    perT1 = perT1.elem(ind); // replace product with Imateria
    if(perT1.n_elem == 0)
        return;
    float meanT1 = mean(perT1);     
    //  complete calculations from calculateRegionFeatures()
    (*featuresVector)(4) = WEIGHT_CLASSIFIER_FL * ((*featuresVector)(4) - meanFL)/(*featuresVector)(4);
    (*featuresVector)(5)= ((*featuresVector)(5) - meanT1)/(*featuresVector)(5);
//     cout << featuresVector->t();    
}


void AmosProcessCase::calculatePerimeterFeatures3D(Cube< unsigned char >* Mper, arma::Mat<itk::IndexValueType>::fixed<2, 3> boundings, fvec* featuresVector)
{
    Col<uword> indexOfPixels = find(*Mper);
//     cout << indexOfPixels.t() << endl;    
    Col<uword> indexInImage;
    unsigned int nBlobRows = boundings.at(1,0) - boundings.at(0,0) + 1;
    unsigned int nBlobCols = boundings.at(1,1) - boundings.at(0,1) + 1;
    
    
            
    uvec cociente = (indexOfPixels/(nBlobRows*nBlobCols) + boundings.at(0, 2))*FL3DROIcube.n_rows*FL3DROIcube.n_cols;
//     cout << "cociente " << cociente.t(); 
    uvec resto = indexOfPixels - (indexOfPixels/(nBlobRows*nBlobCols))*nBlobRows*nBlobCols;
//     cout << "resto " << resto.t();
    uvec cociente2 = (resto/nBlobRows + boundings.at(0, 1))*FL3DROIcube.n_rows;
//     cout << "cociente2 " << cociente2.t();
    uvec resto2 = resto - (resto/nBlobRows)*nBlobRows;
//     cout << "resto2 " << resto2.t();
    indexInImage = cociente + cociente2 + resto2 + boundings.at(0, 0);                     
    
//     cout << indexInImage.t() << endl;       

    fvec perFL = FL3DROIcube.elem(indexInImage);
    uvec ind = find(perFL >= thresholdLevelMateria);
    perFL = perFL.elem(ind); // replace product with Imateria
    if(perFL.n_elem == 0)
        return;
    float meanFL = mean(perFL);
    
    fvec perT1 = T13Dcube.elem(indexInImage);
    perT1 = perT1.elem(ind); // replace product with Imateria
    if(perT1.n_elem == 0)
        return;
    float meanT1 = mean(perT1);     
    //  complete calculations from calculateRegionFeatures()
    (*featuresVector)(4) = WEIGHT_CLASSIFIER_FL * ((*featuresVector)(4) - meanFL)/(*featuresVector)(4);
    (*featuresVector)(5)= ((*featuresVector)(5) - meanT1)/(*featuresVector)(5); 
//     cout << featuresVector->t();
}

void AmosProcessCase::addClassificationLine(fvec featuresVector)
{
    int length = featuresVector.n_elem;
    QString line = QString::number((int) round(featuresVector(length - 1)));
    for(int i = 0; i < length - 1; i++) 
        line += " " + QString::number(i + 1) + ":" + QString::number(featuresVector(i));
    
    if(numThreads > 1)
        mutex.lock();
    classificationText.append(line);
    if(numThreads > 1) {
        mutex.unlock();
        wait(); 
    }
}

void AmosProcessCase::writeClassification()
{
    caseProgress = 4;
    progressType = tr("Writing classif.");
    progressRange = 1;
    QTimer::singleShot(0, this, SLOT(changeProgres()));
    
    QFileInfo fileInfoRes(objectsFeaturesDir.absoluteFilePath(), QString("Objects_features_") + subjectName + QString(".libsvm"));
    QFile fileRes(fileInfoRes.absoluteFilePath());
    //     cout << fileInfoRes.absoluteFilePath().toStdString() << endl;
    if(!fileRes.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit sendLog("Can not write " + fileInfoRes.absoluteFilePath());
        cout << "Can not write " << fileInfoRes.absoluteFilePath().toStdString() << endl;
        return;
    }
    QTextStream out(&fileRes);
    int length = classificationText.size();
    for(int i = 0; i < length; i++) 
        out << classificationText.at(i) << Qt::endl;
    
    fileRes.flush();
    fileRes.close();
    classificationText.clear();
    
}

void AmosProcessCase::readScaleFile()
{
    QFile scaleFile(SVMScaleFile);
    if(!scaleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // emit sendLog("Can not read " + scaleFile.fileName());
        // 	cout << "Can not read " << scaleFile.fileName().toStdString() << endl;
        return;
    }  
    QTextStream in(&scaleFile);
    QString lineIn;
    QString splitter(' ');
    QStringList fieldsIn;    
    int cnt = 0;
    while(!in.atEnd()) {
        cnt++;
        lineIn = in.readLine(); 
        if(cnt == 1)
            continue;
        fieldsIn = lineIn.trimmed().split(splitter, Qt::SkipEmptyParts);
        if(cnt == 2) {
            for(int i = 0; i < fieldsIn.size(); i++) 
                featuresClasses.append(fieldsIn.at(i).toInt());
        }
        else {
            if(fieldsIn.length() != 3) {
                // emit sendLog("Invalid scaling SVM file " + scaleFile.fileName());
                // 		cout << "Invalid scaling SVM file " << scaleFile.fileName().toStdString() << endl;
                minmaxFeatures.clear();
                featuresClasses.clear();
                return;
            }		
            minmaxFeatures.append(QPointF(fieldsIn.at(1).toFloat(), fieldsIn.at(2).toFloat()));
        }
    }
    return;
}

void AmosProcessCase::scaleFeatures(fvec* featuresVector)
{
    for(int i = 0; i < featuresNumber; i++) 
        (*featuresVector)(i) = ((*featuresVector)(i) - minmaxFeatures.at(i).x()) / (minmaxFeatures.at(i).y() - minmaxFeatures.at(i).x());
}

void AmosProcessCase::stopProcess()
{

    InternalProcessCase::stopProcess();
    
    if(imagesPreviouslyLoaded && geometryFLChanged) // revert geometryFL for running again
        Utils::revertGeometry(geometryFL, icod, jcod, kcod, reorderI, reorderJ, reorderK);
    //     terminate();
}

/*
void AmosProcessCase::revertSegmentedCubeOrientationSampling(arma::Cube<uchar>* inCube, arma::Cube<uchar>* outCube, bool avoidReorientation)
{    

    DataOrientation::OrientationData oData;
    oData.icod = icod;
    oData.jcod = jcod;
    oData.kcod = kcod;
    oData.reorderI = reorderI;
    oData.reorderJ = reorderJ;
    oData.reorderK = reorderK;
    DataOrientation::OrientationData invData = Utils::invertOData(oData);
    int dims[] = {invData.reorderI, invData.reorderJ, invData.reorderK};
    Col<int> indexes;
    indexes << invData.icod << invData.jcod << invData.kcod;
//     Col<int> indexes = {invData.icod, invData.jcod, invData.kcod};    
    
    if(!avoidReorientation) {
        if(applyRescale) {
            arma::Cube<uchar> tmpCubeRescaled;
            if(!mode3D) {
                mitk::Vector2D inSpacing2D;
                inSpacing2D[0] = inSpacing3D[0];
                inSpacing2D[1] = inSpacing3D[1];
                mitk::Vector2D outSpacing2D;
                outSpacing2D[0] = outSpacing3D[0];
                outSpacing2D[1] = outSpacing3D[1];                
                mitk::Vector2D outOrigin2D;
                outOrigin2D[0] = outOrigin3D[0];
                outOrigin2D[1] = outOrigin3D[1];
                mitk::Vector2D inOrigin2D;
                inOrigin2D[0] = inOrigin3D[0];
                inOrigin2D[1] = inOrigin3D[1];                 
//                 ImageFilters::ResampleCubeFilter2D(inCube, &tmpCubeRescaled, ImageFilters::NEAREST, outSpacing2D, inSpacing2D, outOrigin2D, inOrigin2D, numThreadsForConvert);
                ImageFilters::ResampleCubeFilter2D(inCube, &tmpCubeRescaled, ImageFilters::NEAREST, outSpacing2D, inSpacing2D, outOrigin2D, outOrigin2D, numThreadsForConvert);
            }
            else
//                 ImageFilters::ResampleCubeFilter3D(inCube, &tmpCubeRescaled, ImageFilters::NEAREST, outSpacing3D, inSpacing3D, outOrigin3D, inOrigin3D, numThreadsForConvert);
                ImageFilters::ResampleCubeFilter3D(inCube, &tmpCubeRescaled, ImageFilters::NEAREST, outSpacing3D, inSpacing3D, outOrigin3D, outOrigin3D, numThreadsForConvert);

            Utils::revertCubeOrientation(&tmpCubeRescaled, outCube, dims, indexes);
        }
        else
            Utils::revertCubeOrientation(inCube, outCube, dims, indexes);
    }
    else {
        if(applyRescale) {
            if(!mode3D) {
                mitk::Vector2D inSpacing2D;
                inSpacing2D[0] = inSpacing3D[0];
                inSpacing2D[1] = inSpacing3D[1];
                mitk::Vector2D outSpacing2D;
                outSpacing2D[0] = outSpacing3D[0];
                outSpacing2D[1] = outSpacing3D[1];                
                mitk::Vector2D outOrigin2D;
                outOrigin2D[0] = outOrigin3D[0];
                outOrigin2D[1] = outOrigin3D[1];
                mitk::Vector2D inOrigin2D;
                inOrigin2D[0] = inOrigin3D[0];
                inOrigin2D[1] = inOrigin3D[1];                 
//                 ImageFilters::ResampleCubeFilter2D(inCube, outCube, ImageFilters::NEAREST, outSpacing2D, inSpacing2D, outOrigin2D, inOrigin2D, numThreadsForConvert);
                ImageFilters::ResampleCubeFilter2D(inCube, outCube, ImageFilters::NEAREST, outSpacing2D, inSpacing2D, outOrigin2D, outOrigin2D, numThreadsForConvert);
            }
            else
//                 ImageFilters::ResampleCubeFilter3D(inCube, outCube, ImageFilters::NEAREST, outSpacing3D, inSpacing3D, outOrigin3D, inOrigin3D, numThreadsForConvert);
                ImageFilters::ResampleCubeFilter3D(inCube, outCube, ImageFilters::NEAREST, outSpacing3D, inSpacing3D, outOrigin3D, outOrigin3D, numThreadsForConvert);
            
//             string fileName("AmosACcube2.nii");
//             Image::Pointer AmosACImg = Utils::cubeToMITKImage(outCube);    
//             IOUtil::Save(AmosACImg, fileName);             
        }        
        else
            Utils::copyCastingArma(inCube, outCube);	    
    }
    
}
*/
