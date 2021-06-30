#ifndef AMOSPROCESSCASE_H
#define AMOSPROCESSCASE_H

#include "internalprocesscase.h"

#include <QThread>
#include <QMutex>

#include "imagefilters.hpp"

class SVMClassifier;
class RegionProperties;
class RegionProperties3D;

// class AmosWorkbenchLib_EXPORT AmosProcessCase : public QThread
class AmosWorkbenchLib_EXPORT AmosProcessCase : public InternalProcessCase
{
    Q_OBJECT

public:
        
    AmosProcessCase(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D);
    ~AmosProcessCase();
    
    enum CASES_THRESHOLD_BASE {borderWM, GM_in_border_WM};
    
    void initHeap() Q_DECL_OVERRIDE;
//     void setNumThreads(int numThreads) Q_DECL_OVERRIDE;
    
    bool setOutputDirectories(QString parentDirOut, QString pathOut) Q_DECL_OVERRIDE;
    void setOutputNames(QList<QString> outputNames) Q_DECL_OVERRIDE;
    
    void setSVMFiles(QString modelFile, QString scaleFile, int featuresNumber = 0);
    
    void stopProcess() Q_DECL_OVERRIDE;
 
    bool setParameters(QMap<QString, QString> parameters) Q_DECL_OVERRIDE;
    
    struct ImageSegmentationStats {
        double meanWM3D;
        double stdWM3D;	
    };
    
    struct ScanningParameters {
        int NBINS;
        float mean;
        float std;
        int Kh;
        int Kl;
        int JUMPTHRESBLOBS;
        int AREA;	
    };
    
    struct ResultScanTailHisto {
        arma::mat table;
        double K_firstJumpNBlobs;
        double K_highestJumpNBlobs;
        double K_areaSeg;
        double K_noiseLevel;
    };
    
    struct EditableParameters {
        double NSIGMADESVseed;
        double NSIGMADESVseed2;
        double NSIGMADESV_TH2;
        double NSIGMADESV_TH4;
        int ERODEWM;
        double WEIGHT_CLASSIFIER_FL;
    };   
    
    void setEditableParameters(EditableParameters parameters);
    static EditableParameters getDefaultEditableParameters();    
    
protected slots:
    
    void saveResults() Q_DECL_OVERRIDE;
    void nonLoopExecute() Q_DECL_OVERRIDE;
    
protected:
   
    void startMainLoop3D() Q_DECL_OVERRIDE;
    void computeIROI3D() Q_DECL_OVERRIDE;
    void preMainLoop() Q_DECL_OVERRIDE;
    
    virtual void computeRegionProps3D(uint iniSlice, uint endSlice, int threadIndex = 0);
    
    //void computeDistsTo(arma::fcube* DistToWMBorder);
    void computeDistsTo();
    void computeISEGR(arma::Cube<unsigned char>* IsegR);
    
    void computeBlobsForFeatures(arma::Cube<unsigned char>* IsegR);
    virtual void blobsFeaturesLoop(int blobInit = 0, int blobEnd = 0);
    
    void overwriteACBC3D();
    
    bool loadImages(QList<Image::Pointer> imagesMITK) Q_DECL_OVERRIDE;

//     void changeGeometry();

//     template <typename TypeOut> void extractCubeROI(arma::Cube<TypeOut>* cube)    
//     {
//         if(SLICE_INI_LOOP > 0 || SLICE_END_LOOP < (int)(LB3Dcube.n_slices)) {
//             if(!fullRestrictedToROI) {
//                 *cube = cube->slices(SLICE_INI_LOOP, SLICE_END_LOOP);
//             }
//             else {
//                 *cube = cube->subcube(dimsWM(0, 1), dimsWM(1, 1), SLICE_INI_LOOP, dimsWM(0, 0), dimsWM(1, 0), SLICE_END_LOOP);		    
//             }
//         }	
//     }

//     void revertSegmentedCubeOrientationSampling(arma::Cube<uchar>* inCube, arma::Cube<uchar>* outCube, bool avoidReorientation = false);
    
    bool calculateOrientation() Q_DECL_OVERRIDE;
    bool calculateRescaling() Q_DECL_OVERRIDE;
   
    void generateLBWM3D();
    arma::Mat<int> limitsData3D(arma::Cube<unsigned char> I);
    void calculateWMVol();
    arma::Cube<unsigned char> labelsCube(arma::Col<unsigned int> labels, bool whole);

    void flair3DAnalisys();
    void setStructuresParameters();
    void vectorsForScanSegmentTH();
    ResultScanTailHisto scanSegmentTailHistogram(arma::Mat< float >* matrixRMI, arma::Mat< unsigned char >* matrixBIN);
    ResultScanTailHisto scanSegmentTailHistogram(arma::Mat< float >* matrixRMI, arma::Mat< float >* matrixBINFloat);
    void calculateRegionFeatures(arma::fmat* FL, arma::fmat* T1, arma::fmat* DistToBorder, arma::fmat* DistToCHULL, ImageFilters::LabelSimpleFeatures<2>* blobInfo, int slice, arma::fvec* featuresVector);
    void calculatePerimeterFeatures(arma::fmat* FL, arma::fmat* T1, arma::Mat< unsigned char >* Mper, arma::fvec* featuresVector);
    void calculatePerimeterFeatures(arma::fmat* FL, arma::fmat* T1, arma::Mat< unsigned char >* Mper, arma::Mat<itk::IndexValueType>::fixed<2, 2> boundings, arma::fvec* featuresVector);
    
    void calculateRegionFeatures3D(ImageFilters::LabelSimpleFeatures<3>* blobInfo, arma::fvec* featuresVector);
    
//     void calculateRegionFeatures3D(arma::fcube* FL, arma::fcube* T1, arma::fcube* DistToBorder, arma::fcube* DistToCHULL, ImageFilters::LabelFeatures<3>* blobInfo, arma::fvec* featuresVector);
    void calculatePerimeterFeatures3D(arma::Cube< unsigned char >* Mper, arma::fvec* featuresVector);
    void calculatePerimeterFeatures3D(arma::Cube< unsigned char >* Mper, arma::Mat<itk::IndexValueType>::fixed<2, 3> boundings, arma::fvec* featuresVector);

//     template <typename ArmaType, int Dimension> bool intersectBlobWML(ArmaType* ANOT, ArmaType* Iseg_BC, ImageFilters::LabelSimpleFeatures<Dimension>* blobInfo)
    template <typename ArmaType, int Dimension, typename LabelFeaturesType> bool intersectBlobWML(ArmaType* ANOT, ArmaType* Iseg_BC, LabelFeaturesType* blobInfo)
    {
        auto begin = chrono::high_resolution_clock::now();
        
        bool intersect;
        arma::Col<uchar> blobIseg = Iseg_BC->elem(blobInfo->indexOfPixels);
        arma::Col<uchar> blobAnot = ANOT->elem(blobInfo->indexOfPixels);
        arma::uvec inter = arma::find(blobIseg % blobAnot);
        intersect = inter.n_elem > 0 ? true : false;
        
        auto end = chrono::high_resolution_clock::now();    
        if(Utils::showTime)
            emit sendLog("intersectBlobWML " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        // 	cout << "intersectBlobWML " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;    
        
        return intersect;
    }
    void addClassificationLine(arma::fvec featuresVector);
    void writeClassification();
    void readScaleFile();
    void scaleFeatures(arma::fvec* featuresVector);
   
    bool verifySizes() Q_DECL_OVERRIDE;
    /*virtual */void convertImages(Utils::ImageTypes type = Utils::ALL) Q_DECL_OVERRIDE;
    void convertImageMaskAndLB() Q_DECL_OVERRIDE;

//     void run() Q_DECL_OVERRIDE;
    virtual void WMMask3DROICompute2D(int startSlice, int endSlice);
    void WMMask3DROICompute3D();
    virtual void thresholdsLoop(int startSlice, int endSlice);
    void calculateThresholds();
    void mainLoop2D(int startSlice, int endSlice, int threadIndex = 0) Q_DECL_OVERRIDE;
    void mainLoop3D() Q_DECL_OVERRIDE;
    
    void clearMemory() Q_DECL_OVERRIDE;     
    
protected:
    QString ACName;
    QString BCName;

/*    Image::Pointer orgFL3D;
    Image::Pointer orgT13D;    
    Image::Pointer orgLB3D;
    Image::Pointer orgANOT3D;
    Image::Pointer orgMASK3D;  */  
    
//     Image::Pointer FL3D;
    Image::Pointer Amos_AC_Image;
    Image::Pointer Amos_BC_Image;

//     arma::fcube FL3Dcube;
//     arma::fcube FL3DROIcube;
//     arma::fcube T13Dcube;    
//     arma::Cube<unsigned short> LB3Dcube;
//     arma::Cube<unsigned char> ANOT3Dcube;
//     bool LB3DConverted;
    
    vector<ImageFilters::LabelSimpleFeatures<3> > labelsInfoBC3D;
    
    arma::Cube<unsigned char> Iseg3D_AC_blobs;
    arma::Cube<unsigned char> Iseg3D_BC_blobs;
    arma::fcube DistToCHULL3D;
    //arma::fcube DistToBorder3D;
    arma::fcube DistToWMBorder3D;
    arma::fcube DistToWMCM;
    arma::Cube<unsigned char> Iroi3D;
    arma::Cube<unsigned char> Ich3D;
    arma::Cube<unsigned char> WMmask3DROIBinCube;
    arma::Cube<unsigned char> WMmask3DModelBinCube;
    ImageFilters::NotEmptyForeground IroiHasForeground3D;
    
/*    arma::Cube<unsigned char> WMMask3DModelCube;
    arma::Cube<unsigned char> WMMask3DROICube; */  
    
    arma::fvec WMCM;

    arma::Cube<unsigned char> Imateria;
    arma::Cube<unsigned char> Iseg3D_BC;
    arma::Cube<unsigned char> newIseg3D_BC;
    arma::Cube<unsigned char> Iseg3D_AC;
    arma::Cube<unsigned char> newIseg3D_AC;

    int numSlicesWithLBWM3DMinSize;
    
    
//    PARAMETROS DE ENTRADA al algoritmo
    double NSIGMADESVseed;
    double NSIGMADESVseed2;
    double NSIGMADESV_TH2;
    double NSIGMADESV_TH4;
    double NSIGMADESVseed2Max;
    int NBINS;
    unsigned int NPOINTSGAUSSIANAPROX;    

    int ERODEWM;

    unsigned int WMDisk10Diameterin;
    unsigned int LBWMDiskDiameterin;
    unsigned int EXCLUSIONZONEin;
    unsigned int R_PERIPHERYin;
    
    unsigned int ERODEWM_vector[3];
    unsigned int WMDisk10Diameter[3];
    unsigned int LBWMDiskDiameter[3];               
    unsigned int EXCLUSIONZONE[3];
    unsigned int R_PERIPHERY[3];
    
    unsigned int MINWMLSIZE; 

    uint LBWM3DMinSize;
    uint initialLBWM3DMinSize;
    double LBWM3DMinSizeFactor;
    double WEIGHT_CLASSIFIER_FL;
    int tailHistogramLowerLimit;
    
    //static EditableParameters defaultParameters;    
    
    
    // MODEL WM
    double meanWM3D;
    double stdWM3D;
        // caracteristicas de la distribución de WM para elegir el TH que meta menos ruido
    unsigned int K_size;   
    arma::vec K_values;
    arma::uvec cuts;
    arma::uvec numBlobs_until;
    double K_noiseLevelFindLimit;
        // modelado de la distribucion de WM
    double K_noiseLevelDefault;
    arma::vec THwm_noiseLevel;
    arma::vec THwm_highestJumpNObjs;
    arma::vec THwm_firstJumpNObjs;
    arma::vec THwm_areaObj;    
    arma::vec THgm_highestJumpNObjs;
    arma::vec THgm_firstJumpNObjs;
    arma::vec THgm_areaObj;
    double TH13D;
    double thresholdLevelMateria;
    
    
    // SEGMENTACION
    double K3D_wm_noiseLevel;
    double K3D_wm_highestJumpNObjs;
    double K3D_wm_firstJumpObjs;
    double K3D_wm_areaSeg;
    double K3D_gm_highestJumpNObjs;
    double K3D_gm_firstJumpObjs;
    double K3D_gm_areaSeg;  
    double Ks1_3D, Kth1_3D, Ks2_3D, Kth2_3D;
    
    QList<RegionProperties*> regionProps;
    // RegionProperties3D* regionProps3D;
    
    ImageSegmentationStats p_Iseg;
    ScanningParameters p_Scan;
    
    // Histograms centers and intervals
    arma::fvec histoCenters;
    arma::fvec posH;
    
    SVMClassifier* classifier;
    QString SVMScaleFile;
    int featuresNumber;

    QFileInfo objectsFeaturesDir;
    QStringList classificationText;
    QVector<int> featuresClasses;
    QVector<QPointF> minmaxFeatures;

};

#endif // AMOSPROCESSCASE_H
