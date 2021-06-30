#ifndef INTERNALPROCESSCASE_H
#define INTERNALPROCESSCASE_H

#include <QThread>
#include <QMutex>

#include "imagefilters.hpp"

#include <itkRescaleIntensityImageFilter.h>
#include <QList>
#include <QDir>
#include <QVector>
#include <QPointF>

using namespace arma;

class AmosWorkbenchLib_EXPORT InternalProcessCase : public QThread
{
    Q_OBJECT

public:
        
    InternalProcessCase(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D);
    ~InternalProcessCase();

    void virtual initHeap() = 0;
    virtual void setNumThreads(int numThreads);
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);
    
    virtual bool setImages(int numCase, QList<Image::Pointer> imagesMITK, QString subjectName = "", int SLICE_INI_LOOP = -1, int SLICE_END_LOOP = -1);
    
    virtual bool setOutputDirectories(QString parentDirOut, QString pathOut);
    virtual void setOutputNames(QList<QString> outputNames) = 0;
    
    virtual void stopProcess();

    virtual bool setParameters(QMap<QString, QString> parameters) = 0;
    
    void setNewMode(bool mode);
    
protected slots:
    void executionFinished();    
    virtual void execute();
    virtual void saveResults() = 0;
    virtual void nonLoopExecute() = 0;
    virtual void preMainLoop() = 0;
    void changeProgres();
    
signals:
    void sendLog(QString );
    void sendProgressRange(int );
    void sendProgress(int );
    void sendProgressType(QString );
    void sendExecutionFinished(InternalProcessCase*, int, bool );
    void sendSaveImage(Image::Pointer , QFileInfo );
    void sendDifferentQforms(QString);
    void sendNotEnoughWM(QString);
    void sendCaseProgress(int);
    
protected:
    void executionFinished(bool ok);
    typedef itk::RescaleIntensityImageFilter< Utils::ITKFloatImageType3D, Utils::ITKFloatImageType3D > RescaleFilterType;
    void normalizeImage(mitk::Image::Pointer image, arma::fcube* cube, mitk::Image::Pointer rescaledImage = 0);
    
    virtual void startMainLoop3D();
    virtual void computeIROI3D() = 0;
    virtual bool loadImages(QList<Image::Pointer> imagesMITK) = 0;
    virtual bool calculateOrientation() = 0;

/**
 * @brief Convert unknown pixel type from a type another type in a 3Dimage, creates a copy, creates the cube associated to its data, apply infracted mask, transpose, reorder to MITK LAS and resample to voxel dimension for Amos algorithm
 * 
 * @param imageIn Pointer to input 3Dimage
 * @param cube Pointer to output 3Dimage cube
 * @return
 */    
    
    template <typename TypeOut> void processImage2Cube(mitk::Image::Pointer imageIn, arma::Cube<TypeOut>* cube, int numOfThreads = 1, bool normalize = true)
    {
        arma::Cube<TypeOut> tmpCubeIn;
        arma::Cube<TypeOut> tmpCubeTransposed;
        arma::Cube<TypeOut> tmpCubeRescaled;
        ImageFilters::ResampleTypes resampleType;

        tmpCubeIn = Utils::createCubeFromMITKImage<TypeOut>(imageIn, false);
        if(is_floating_point<TypeOut>::value) {	
            if(normalize)
                Utils::normalizeArma(&tmpCubeIn);
            resampleType = ImageFilters::LINEAR;
        }
        else
            resampleType = ImageFilters::NEAREST;
        
        if(hasMask)
            tmpCubeIn = tmpCubeIn % MASK3Dcube;
        
        Utils::transpose(&tmpCubeIn, &tmpCubeTransposed, icod, jcod, kcod);
        
        int dims[] = {reorderI, reorderJ, reorderK};
        
        if(!applyRescale) {
            Utils::reorder(&tmpCubeTransposed, cube, dims);
            return;
        }
        
        Utils::reorder(&tmpCubeTransposed, &tmpCubeRescaled, dims);

        if(!mode3D) {
            mitk::Vector2D inSpacing2D;
            inSpacing2D[0] = inSpacing3D[0];
            inSpacing2D[1] = inSpacing3D[1];
            mitk::Vector2D outSpacing2D;
            outSpacing2D[0] = outSpacing3D[0];
            outSpacing2D[1] = outSpacing3D[1];                
            mitk::Vector2D inOrigin2D;
            inOrigin2D[0] = inOrigin3D[0];
            inOrigin2D[1] = inOrigin3D[1];
            mitk::Vector2D outOrigin2D;
            outOrigin2D[0] = outOrigin3D[0];
            outOrigin2D[1] = outOrigin3D[1];              
        
//             ImageFilters::ResampleCubeFilter2D(&tmpCubeRescaled, cube, resampleType, inSpacing2D, outSpacing2D, inOrigin2D, outOrigin2D, numOfThreads);
            ImageFilters::ResampleCubeFilter2D(&tmpCubeRescaled, cube, resampleType, inSpacing2D, outSpacing2D, inOrigin2D, inOrigin2D, numOfThreads);
        }
        else
//             ImageFilters::ResampleCubeFilter3D(&tmpCubeRescaled, cube, resampleType, inSpacing3D, outSpacing3D, inOrigin3D, outOrigin3D, numOfThreads);
            ImageFilters::ResampleCubeFilter3D(&tmpCubeRescaled, cube, resampleType, inSpacing3D, outSpacing3D, inOrigin3D, inOrigin3D, numOfThreads);
    }  
    
    template <typename TypeOut> void extractCubeROI(arma::Cube<TypeOut>* cube)    
    {
        if(SLICE_INI_LOOP > 0 || SLICE_END_LOOP < (int)(LB3Dcube.n_slices)) {
            if(!fullRestrictedToROI) {
                *cube = cube->slices(SLICE_INI_LOOP, SLICE_END_LOOP);
            }
            else {
                *cube = cube->subcube(dimsWM(0, 1), dimsWM(1, 1), SLICE_INI_LOOP, dimsWM(0, 0), dimsWM(1, 0), SLICE_END_LOOP);		    
            }
        }	
    }   
    
    template <typename Type> void revertSegmentedCubeOrientationSampling(arma::Cube<Type>* inCube, arma::Cube<Type>* outCube, bool avoidReorientation = false)
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
        
        ImageFilters::ResampleTypes resampleType;

        if(is_floating_point<Type>::value) {	
            resampleType = ImageFilters::LINEAR;
        }
        else
            resampleType = ImageFilters::NEAREST;        
        
        if(!avoidReorientation) {
            if(applyRescale) {
                arma::Cube<Type> tmpCubeRescaled;
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
                    ImageFilters::ResampleCubeFilter2D(inCube, &tmpCubeRescaled, resampleType, outSpacing2D, inSpacing2D, outOrigin2D, outOrigin2D, numThreadsForConvert);
                }
                else
    //                 ImageFilters::ResampleCubeFilter3D(inCube, &tmpCubeRescaled, ImageFilters::NEAREST, outSpacing3D, inSpacing3D, outOrigin3D, inOrigin3D, numThreadsForConvert);
                    ImageFilters::ResampleCubeFilter3D(inCube, &tmpCubeRescaled, resampleType, outSpacing3D, inSpacing3D, outOrigin3D, outOrigin3D, numThreadsForConvert);

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
                    ImageFilters::ResampleCubeFilter2D(inCube, outCube, resampleType, outSpacing2D, inSpacing2D, outOrigin2D, outOrigin2D, numThreadsForConvert);
                }
                else
    //                 ImageFilters::ResampleCubeFilter3D(inCube, outCube, ImageFilters::NEAREST, outSpacing3D, inSpacing3D, outOrigin3D, inOrigin3D, numThreadsForConvert);
                    ImageFilters::ResampleCubeFilter3D(inCube, outCube, resampleType, outSpacing3D, inSpacing3D, outOrigin3D, outOrigin3D, numThreadsForConvert);
                
    /*            string fileName("AmosACcube2.nii");
                Image::Pointer AmosACImg = Utils::cubeToMITKImage(outCube);    
                IOUtil::Save(AmosACImg, fileName); */            
            }        
            else
                Utils::copyCastingArma(inCube, outCube);	    
        }        
    }

    virtual bool verifySizes() = 0;
    virtual void mainLoop2D(int startSlice, int endSlice, int threadIndex = 0) = 0;
    virtual void mainLoop3D();
    
    virtual void clearMemory();
    virtual void convertImages(Utils::ImageTypes type = Utils::ALL);
    virtual void convertImageMaskAndLB();
    virtual bool calculateRescaling();
    
protected:
    int numCase;
    QString subjectName;
    
    int numThreads;
    int numThreadsForConvert;
    int sliceCounter;
    int convertCounter;
    int convertProgressRange;

    bool modeAutomatic;
    bool mode3D;
    bool imagesPreviouslyLoaded;
    bool hasMask;
    QList<Utils::ImageTypes> listTypes;
    
    Image::Pointer orgFL3D;
    Image::Pointer orgT13D;    
    Image::Pointer orgLB3D;
    Image::Pointer orgANOT3D;
    Image::Pointer orgMASK3D;
    Image::Pointer FL3D;
    
    arma::fcube FL3Dcube;
    arma::fcube FL3DROIcube;
    arma::fcube T13Dcube;    
    arma::Cube<unsigned short> LB3Dcube;
    arma::Cube<unsigned char> ANOT3Dcube;
    bool LB3DConverted;  
    arma::Cube<unsigned char> WMMask3DModelCube;
    arma::Cube<unsigned char> WMMask3DROICube; 

    unsigned int *dimImg3D;
    arma::Mat<int> dimsWM;   
    arma::Cube<unsigned char> MASK3Dcube;

    int SLICE_INI_LOOP, SLICE_END_LOOP;
    int initialLoopSlice, endLoopSlice;
    int SLICE_INI_CER, SLICE_END_CER;
     
    bool subjectsAsMainDirs;
    QDir outputParentDirectory;
    QString pathOut;
    QDir outputDirectory;
    
    bool stop;
    bool stopped;
    bool finishedOk;
    QMutex mutex;
    
    int icod, jcod, kcod;
    int reorderI, reorderJ, reorderK;
    mitk::Vector3D inSpacing3D, outSpacing3D, inOrigin3D, outOrigin3D;
    
    bool applyRescale;
    double maxAllowedDiffForRescale;
    
    bool fullRestrictedToROI;
    
    QString progressType;
    int progressRange;
    int caseProgress;
    
    BaseGeometry* geometryFL;
    TimeGeometry* orgGeometryFL;
    bool geometryFLChanged;

};

#endif // INTERNALPROCESSCASE_H
