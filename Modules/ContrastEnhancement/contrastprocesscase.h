#ifndef CONTRASTPROCESSCASE_H
#define CONTRASTPROCESSCASE_H

#include "internalprocesscase.h"
#include "ContrastEnhancementExports.h"

#include "prompt/defs.h"
#include "prompt/edison.h"

#include <QMap>
/**
 * @todo write docs
 */
class ContrastEnhancement_EXPORT ContrastProcessCase : public InternalProcessCase
{
    
    Q_OBJECT
    
public:

    /**
     * @todo write docs
     */
    ContrastProcessCase(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D);

    /**
     * @todo write docs
     */
    ~ContrastProcessCase();
    
    // Specify the edge detection parameters
    struct EdisonParams {
        int GradientWindowRadius = 5; //2
        int MinimumLength = 5;
        double NmxRank = 0.5;
        double NmxConf = 0.5;
        int NmxType = CURVE_PARAMS::CURVE_ARC;
        double HysterisisHighRank = 0.9;
        double HysterisisHighConf = 0.9;
        int HysterisisHighType = CURVE_PARAMS::CURVE_BOX;
        double CustomCurveHystHigh[2][2] = {{0.4,0.7}, {0.6,0.3}};
        double HysterisisLowRank = 0.8;
        double HysterisisLowConf = 0.8;
        int HysterisisLowType = CURVE_PARAMS::CURVE_CUSTOM;
        double CustomCurveHystLow[2][2] = {{0.4,0.7}, {0.6,0.3}};
        QString DisplayProgress = "ON";
    };

    struct EdisonFiles {
        QString gradientMap = "gradientmap.txt";
        QString edgeMap = "edgesmap.pgm";
        QString confidenceMap = "confidencemap.txt";
    };    

    /**
     * @todo write docs
     *
     * @param parameters TODO
     * @return TODO
     */
    bool setParameters(QMap< QString, QString > parameters) Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @param outputNames TODO
     * @return TODO
     */
    void setOutputNames(QList<QString> outputNames) Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @return TODO
     */
    void initHeap() Q_DECL_OVERRIDE;
    
signals:
    void startExecution();    
    
protected slots:    
    void saveResults() Q_DECL_OVERRIDE;
    void nonLoopExecute() Q_DECL_OVERRIDE;
    void preMainLoop() Q_DECL_OVERRIDE;    

protected:
    
    void startMainLoop3D() Q_DECL_OVERRIDE;
    /**
     * @todo write docs
     *
     * @param startSlice TODO
     * @param endSlice TODO
     * @param threadIndex TODO
     * @return TODO
     */
    void mainLoop2D(int /*startSlice*/, int /*endSlice*/, int /*threadIndex*/) Q_DECL_OVERRIDE;
    void execute() Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @return TODO
     */
    bool verifySizes() Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @param imagesMITK TODO
     * @return TODO
     */
    bool loadImages(QList< Image::Pointer > imagesMITK) Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @return TODO
     */
    void computeIROI3D() Q_DECL_OVERRIDE;     
    void clearMemory() Q_DECL_OVERRIDE;     
    bool calculateOrientation() Q_DECL_OVERRIDE; 
    bool setImages(int numCase, QList<Image::Pointer> imagesMITK, QString subjectName = "", int SLICE_INI_LOOP = -1, int SLICE_END_LOOP = -1) Q_DECL_OVERRIDE;

    void enhanceContrast();
    arma::umat hist3(arma::fvec *XY, arma::fvec *Y, uint n_binsX, uint n_binsY);
    
    void createCubes();
    virtual void computeGradient(int startSlice, int endSlice);
    
protected:
    EdisonParams edisonParams;
    EdisonFiles edisonFiles;
    QString edsFile;
    bool saveGradientMap;
    bool saveEdgesMap;
    bool saveConfidenceMap;

    Image::Pointer enhancedFlair_Image;
    Image::Pointer edgesMapImage;
    arma::fcube enhancedFlairCube;
    arma::fcube orgFL3Dcube;
    arma::fcube flairLB;
    arma::Cube<unsigned char> LBbinary;
    
    QString enhancedName;
    QString pgmName;
    QString gradientName;
    
    int PGMheigth;
    int PGMwidth;   
    arma::Cube<uchar> pgmDataCube;
    arma::Cube<uchar> edgesMapCube;
    arma::fvec gradientMapData;
    arma::fvec confidenceMapData;
    float *gradientMapDataPart;
    float *confidenceMapDataPart;
    uchar *edgesMapDataPart;
    
    QMap< QString, QString > parameters;
    //QList<EDISON*> edisons;
    QList<EDISON> edisons;
    int edisonNum;    

};

#endif // CONTRASTPROCESSCASE_H
