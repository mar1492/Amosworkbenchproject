#ifndef AMOSPROCESSCASEPARALLEL_H
#define AMOSPROCESSCASEPARALLEL_H

#include "amosprocesscase.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

class /*AmosWorkbenchLib_EXPORT*/ AmosProcessCaseParallel : public AmosProcessCase
{
    Q_OBJECT

public:
        
    AmosProcessCaseParallel(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D);
    ~AmosProcessCaseParallel();
    
    bool setImages(int numCase, QList<Image::Pointer> imagesMITK, QString caseName = "", int SLICE_INI_LOOP = -1, int SLICE_END_LOOP = -1) Q_DECL_OVERRIDE;
    void setNumThreads(int numThreads) Q_DECL_OVERRIDE;
    void stopProcess() Q_DECL_OVERRIDE;
    
protected slots:
    void onThresholdsLoopFinished();    
    void saveResults() Q_DECL_OVERRIDE;
    void nonLoopExecute() Q_DECL_OVERRIDE;
    void run() Q_DECL_OVERRIDE;
    void executionStopped();
    void onImagesConverted();
    void onErodeLBWM3DFinished();
    void onComputeRegionProps3D();
    void onBlobsFeaturesLoopFinished();
    
protected:
    void executeMainLoop();
    void execute() Q_DECL_OVERRIDE;
    void WMMask3DROICompute2D(int startSlice, int endSlice) Q_DECL_OVERRIDE;
    void thresholdsLoop(int startSlice, int endSlice) Q_DECL_OVERRIDE;
    void mainLoop2D(int startSlice, int endSlice, int threadIndex) Q_DECL_OVERRIDE;
    void mainLoop3D() Q_DECL_OVERRIDE;
    void stopFutures();
    void parallelConvertImages();
    void convertImages(Utils::ImageTypes type = Utils::ALL) Q_DECL_OVERRIDE;
    void blobsFeaturesLoop(int blobInit, int blobEnd) Q_DECL_OVERRIDE;
    void computeRegionProps3D(uint iniSlice, uint endSlice, int threadIndex) Q_DECL_OVERRIDE;
    
signals:
    void startExecution();
    
protected:    
    int threadFinished;    
    QList< QFutureWatcher<void>* > watchers;
    QList< QFuture<void> > futures;
    QList< QPair<int, int> > slicesRanges;

    bool firstTimeStopped;
    bool watcherCreated;
    int numWatcherStopped;
    
    int conversionFinished;
    int conversionStarted;    
    int numConversions;
    int numUsedThreads;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> beginThres;
    
};

#endif // AMOSPROCESSCASEPARALLEL_H
