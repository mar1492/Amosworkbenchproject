#include "internalprocesscase.h"
// #include "amosniftiimageio.h" // this include before #include "utils.hpp" in order to avoid conflict between mat44 in armadillo and nifti_io
#include "utils.hpp"
#include "amositkimageio.h"

#include "tests.hpp"

#include <QTextStream>
#include <QTimer>
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

InternalProcessCase::InternalProcessCase(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D)
{
    this->modeAutomatic = modeAutomatic;
    this->imagesPreviouslyLoaded = imagesPreviouslyLoaded;
    this->mode3D = mode3D;
    hasMask = false;
    subjectsAsMainDirs = false;
    dimImg3D = 0;

    stop = false;
    stopped = false;
    finishedOk = true;
    numCase = -1;
    numThreads = 1;
    numThreadsForConvert = 1;
    sliceCounter = 0;

    outSpacing3D[0] = 1;
    outSpacing3D[1] = 1;    
    outSpacing3D[2] = 1;    

    maxAllowedDiffForRescale = 0.25;
    
    fullRestrictedToROI = true;
    
    connect(this, SIGNAL(finished()), this, SLOT(executionFinished()));
    // cout << QString::number(numCase).toStdString()  + " InternalProcessCase created" << endl;
}

/**
 * @brief Destructor, frees memory
 * 
 */
InternalProcessCase::~InternalProcessCase()
{
    if(dimImg3D)
        free(dimImg3D);
    
//     cout << QString::number(numCase).toStdString()  + " InternalProcessCase deleted" << endl;
}

void InternalProcessCase::clearMemory() {
    dimsWM.reset();  
    MASK3Dcube.reset();
}

void InternalProcessCase::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
}

void InternalProcessCase::setNumThreads(int numThreads)
{
    this->numThreads = numThreads;
}

bool InternalProcessCase::setOutputDirectories(QString parentDirOut, QString pathOut)
{
    if(imagesPreviouslyLoaded)
        return true;
    outputParentDirectory.setPath(parentDirOut);
    this->pathOut = pathOut;   
    return true;
}

bool InternalProcessCase::setImages(int numCase, QList<Image::Pointer> imagesMITK, QString subjectName, int SLICE_INI_LOOP, int SLICE_END_LOOP)
{
    stopped = false;
    finishedOk = true;
    this->numCase = numCase;
    this->subjectName = subjectName;
    this->SLICE_INI_LOOP = SLICE_INI_LOOP;
    this->SLICE_END_LOOP = SLICE_END_LOOP;
    initialLoopSlice = SLICE_INI_LOOP;
    endLoopSlice = SLICE_END_LOOP;    
    
    listTypes.clear();
    listTypes << Utils::FLAIR << Utils::T1 << Utils::LABEL/* << Utils::MASK*/;
    
    convertProgressRange = listTypes.count();        
    
    if(!imagesPreviouslyLoaded) {
        QFileInfo outDir;
        QString subdir;
        if(subjectsAsMainDirs) 
            subdir = subjectName + QDir::separator() + pathOut;
        else
            subdir = subjectName;
        outDir.setFile(outputParentDirectory, subdir);
        outputDirectory.setPath(outDir.absoluteFilePath());
        QString p = outDir.absoluteFilePath();
        if(!outputDirectory.exists()) {
            QDir dir;
            if(!dir.mkdir(outDir.absoluteFilePath())) {
                emit sendLog("Can not create output directory " + outDir.absoluteFilePath());
                // 	    cout << "Can not create results directory " << objectsFeaturesDir.absoluteFilePath().toStdString() << endl;
                stopped = true;
                return false;
            }
        }
//         this->subjectName = outputDirectory.dirName();
    } 
    
    if(!loadImages(imagesMITK))
        return false;

    bool ret = verifySizes();
    stopped = !ret;
    
    // if(stop)
    // cout << QString::number(numCase).toStdString() + " InternalProcessCase stopped 164" << endl;
    return ret;
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

/**
 * @brief Normalize 3Dimage (float) from 0 to 1, creates a copy, and creates the cube associated to its data
 * 
 * @param image Original image
 * @param rescaledImage pointer to get the normalized image
 * @return cube* the cube associated to normalized image data
 */

void InternalProcessCase::normalizeImage(mitk::Image::Pointer image, arma::fcube* cube, mitk::Image::Pointer rescaledImage)
{       
    Utils::ITKFloatImageType3D::Pointer itkImage = Utils::ITKFloatImageType3D::New();
    
    mitk::CastToItkImage(image, itkImage);
    
    if(!rescaledImage)
        rescaledImage = Image::New();
    //     mitk::Image::Pointer rescaledImage;
    RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    rescaleFilter->SetOutputMinimum(0.0);
    rescaleFilter->SetOutputMaximum(1.0);    
    rescaleFilter->SetInput(itkImage);
    rescaleFilter->Update();
    rescaledImage = GrabItkImageMemory(rescaleFilter->GetOutput(), rescaledImage, image->GetGeometry());
    
    *cube = Utils::createCubeFromMITKImage<float>(rescaledImage);
    
    rescaleFilter = NULL;
}



void InternalProcessCase::startMainLoop3D()
{   
    QTimer::singleShot(0, this, SLOT(changeProgres()));    
}


 
void InternalProcessCase::mainLoop3D()
{
    startMainLoop3D();
}

/**
 * @brief Execute AMOS algorithm for one case
 * 
 * @return void
 */

void InternalProcessCase::execute()
{
    // emit sendLog("InternalProcessCase::execute case " + subjectName);
    
    cout << "InternalProcessCase::execute case " << outputDirectory.dirName().toStdString() << endl; 
    
    nonLoopExecute();
    
    if(stop) {
        //     	stop = false;
        return;
    }     

    sliceCounter = 0;

    preMainLoop();

    sliceCounter = 0;
    if(!mode3D)
        mainLoop2D(initialLoopSlice, endLoopSlice);
    else
        mainLoop3D();


    if(stop) {
        //     	stop = false;
        return;
    }     
    
}

void InternalProcessCase::stopProcess()
{
    if(!stopped) {
        stop = true;
        if(isRunning())
            quit();
        else
            executionFinished(false);
    }
//     if(imagesPreviouslyLoaded && geometryFLChanged) // revert geometryFL for running again
//         Utils::revertGeometry(geometryFL, icod, jcod, kcod, reorderI, reorderJ, reorderK);
}

void InternalProcessCase::executionFinished()
{
    executionFinished(finishedOk);
}

void InternalProcessCase::executionFinished(bool ok)
{
    if(!stopped) {
        stopped = true;
        QString result = ok ? "true" : "false";
        cout << "sendExecutionFinished " << subjectName.toStdString() << " " << numCase + 1 << " " << result.toStdString() << endl;
        stop = false;
        finishedOk = true;        
        emit sendExecutionFinished(this, numCase, ok);
    }
}

void InternalProcessCase::changeProgres()
{
    emit sendCaseProgress(caseProgress);
    emit sendProgressType(progressType);
    emit sendProgressRange(progressRange);    
}

void InternalProcessCase::convertImages(Utils::ImageTypes type)
{
    if(type == Utils::FLAIR || type == Utils::ALL) {
        // 	bool wholeCube = true;
        processImage2Cube(orgFL3D, &FL3Dcube, numThreadsForConvert);
        cout << " FL3Dcube " << FL3Dcube.n_rows << " " << FL3Dcube.n_cols << " " <<  FL3Dcube.n_slices << endl;	
        emit sendProgress(++convertCounter);
/*        string fileName("FL3Dcube.nii");
        Image::Pointer FL3DImg = Utils::cubeToMITKImage(&FL3Dcube);    
        IOUtil::Save(FL3DImg, fileName);*/	        
        // 	orgFL3D = NULL;
    }
    
    if(type == Utils::T1 || type == Utils::ALL) {
        // 	bool wholeCube = false;
        processImage2Cube(orgT13D, &T13Dcube, numThreadsForConvert);
        extractCubeROI(&T13Dcube);
        cout << " T13Dcube " << T13Dcube.n_rows << " " << T13Dcube.n_cols << " " <<  T13Dcube.n_slices << endl;
        emit sendProgress(++convertCounter);
        orgT13D = NULL;
    }
    
    if(type == Utils::LABEL || type == Utils::ALL) {	
        if(!LB3DConverted) {
            processImage2Cube<unsigned short>(orgLB3D, &LB3Dcube, numThreads);
/*            string fileName("LB3Dcube.nii");
            Image::Pointer LB3DImg = Utils::cubeToMITKImage(&LB3Dcube);    
            IOUtil::Save(LB3DImg, fileName);*/	    
            
            WMMask3DModelCube = zeros< Cube<unsigned char> >(LB3Dcube.n_rows, LB3Dcube.n_cols, LB3Dcube.n_slices);
            WMMask3DROICube = zeros< Cube<unsigned char> >(LB3Dcube.n_rows, LB3Dcube.n_cols, LB3Dcube.n_slices);
            emit sendProgress(++convertCounter);
            orgLB3D = NULL;
            LB3DConverted = true;
        }
        else {
            extractCubeROI(&WMMask3DROICube); 	    
            cout << "WMMask3DROICube " << WMMask3DROICube.n_rows << " " << WMMask3DROICube.n_cols << " " <<  WMMask3DROICube.n_slices << endl;
            uvec ind = find(WMMask3DROICube);
            cout << ind.size() << endl;
            
            extractCubeROI(&WMMask3DModelCube); 	    
            cout << "WMMask3DModelCube " << WMMask3DModelCube.n_rows << " " << WMMask3DModelCube.n_cols << " " <<  WMMask3DModelCube.n_slices << endl;
            ind = find(WMMask3DModelCube);
            cout << ind.size() << endl;            
        }
    }
    
    if(type == Utils::EXPERT || type == Utils::ALL) {
        if(!modeAutomatic) {
            processImage2Cube<unsigned char>(orgANOT3D, &ANOT3Dcube, numThreadsForConvert);
            extractCubeROI(&ANOT3Dcube);
            /*	    string fileName("ANOT3Dcube.nii");
             *	    Image::Pointer ANOT3DImg = Utils::cubeToMITKImage(&ANOT3Dcube);    
             *	    IOUtil::Save(ANOT3DImg, fileName);*/	
            emit sendProgress(++convertCounter);
            orgANOT3D = NULL;
        } 
    }
}

void InternalProcessCase::convertImageMaskAndLB()
{
    LB3DConverted = false;
    
    if(orgMASK3D) {
        
        MASK3Dcube = Utils::createCubeFromMITKImage<uchar>(orgMASK3D, false);
        MASK3Dcube = Utils::inverseArma(MASK3Dcube); // it is possible to define ROIs and masks         
        hasMask = true;
        emit sendProgress(++convertCounter);
    }    
    
    convertImages(Utils::LABEL);    
}

bool InternalProcessCase::calculateRescaling()
{
    bool apply = false;

    Point3D origin = geometryFL->GetOrigin();
    inOrigin3D[0] = origin[0];
    inOrigin3D[1] = origin[1];
    inOrigin3D[2] = origin[2];
    
    mitk::Vector3D spacing = geometryFL->GetSpacing();
    inSpacing3D[0] = spacing[0];
    inSpacing3D[1] = spacing[1];
    inSpacing3D[2] = spacing[2];
    
    // no rescaling
    outSpacing3D[0] = inSpacing3D[0];
    outSpacing3D[1] = inSpacing3D[1];    
    outSpacing3D[2] = inSpacing3D[2];             
    
//     cout << "inOrigin3D " << inOrigin3D[0] << " " << inOrigin3D[1] << " " << inOrigin3D[2] << endl;
//     cout << "inSpacing3D " << inSpacing3D[0] << " " << inSpacing3D[1] << " " << inSpacing3D[2] << endl;
//     cout << "outSpacing3D " << outSpacing3D[0] << " " << outSpacing3D[1] << " " << outSpacing3D[2] << endl;
//     cout << "WMDisk10Diameter " << WMDisk10Diameter[0] << " " << WMDisk10Diameter[1] << " " << WMDisk10Diameter[2] << endl;
//     cout << "LBWMDiskDiameter " << LBWMDiskDiameter[0] << " " << LBWMDiskDiameter[1] << " " << LBWMDiskDiameter[2] << endl;
//     cout << "EXCLUSIONZONE " << EXCLUSIONZONE[0] << " " << EXCLUSIONZONE[1] << " " << EXCLUSIONZONE[2] << endl;
//     cout << "R_PERIPHERY " << R_PERIPHERY[0] << " " << R_PERIPHERY[1] << " " << R_PERIPHERY[2] << endl;
//     cout << "ERODEWM_vector " << ERODEWM_vector[0] << " " << ERODEWM_vector[1] << " " << ERODEWM_vector[2] << endl;

    apply = abs(inSpacing3D[0] - outSpacing3D[0]) > maxAllowedDiffForRescale || abs(inSpacing3D[1] - outSpacing3D[1])  > maxAllowedDiffForRescale;

    if(mode3D)
       apply = apply || abs(inSpacing3D[2] - outSpacing3D[2])  > maxAllowedDiffForRescale;

// mari:  esto no se ejecutará
    unsigned int newDimImg3D[3];
    if(apply) {
        if(!mode3D) {
            for(int i = 0; i < 2; i++) {
                newDimImg3D[i] = (unsigned int) round((double)dimImg3D[i] * inSpacing3D[i] / outSpacing3D[i]);
                // recomputing outSpacing3D to avoid issues with rounding, restoring image to original size
                // could change the size slightly
                outSpacing3D[i] = (double)dimImg3D[i] * inSpacing3D[i] / newDimImg3D[i];
                dimImg3D[i] = newDimImg3D[i];
                outOrigin3D[i] = inOrigin3D[i] * inSpacing3D[i] / outSpacing3D[i];
            }		
            
//             cout << "newDimImg3D " << dimImg3D[0] << " " << dimImg3D[1] << " " << dimImg3D[2] << endl;
//         	cout << "outOrigin2D " << outOrigin3D[0] << " " << outOrigin3D[1] << endl;
//         	cout << "inSpacing2D " << inSpacing3D[0] << " " << inSpacing3D[1] << endl;
//             cout << "outSpacing2D " << outSpacing3D[0] << " " << outSpacing3D[1] << endl;
        }
        else {
            for(int i = 0; i < 3; i++) {
                newDimImg3D[i] = (unsigned int) round((double)dimImg3D[i] * inSpacing3D[i] / outSpacing3D[i]);
                // recomputing outSpacing3D to avoid issues with rounding, restoring image to original size
                // could change the size slightly
                outSpacing3D[i] = (double)dimImg3D[i] * inSpacing3D[i] / newDimImg3D[i];
                dimImg3D[i] = newDimImg3D[i];
                outOrigin3D[i] = inOrigin3D[i] * inSpacing3D[i] / outSpacing3D[i];
            }		
            
/*            cout << "newDimImg3D " << dimImg3D[0] << " " << dimImg3D[1] << " " << dimImg3D[2] << endl;
            cout << "outOrigin3D " << outOrigin3D[0] << " " << outOrigin3D[1] << " " << outOrigin3D[2] << endl;
            cout << "inSpacing3D " << inSpacing3D[0] << " " << inSpacing3D[1] << " " << inSpacing3D[2] << endl;
            cout << "outSpacing3D " << outSpacing3D[0] << " " << outSpacing3D[1] << " " << outSpacing3D[2] << endl; */           
        }
    } 
    return apply;
}
