#ifndef CONTRASTPROCESSCASEPARALLEL_H
#define CONTRASTPROCESSCASEPARALLEL_H

#include "contrastprocesscase.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

//class EDISON;

/**
 * @todo write docs
 */
class ContrastProcessCaseParallel : public ContrastProcessCase
{
        
    Q_OBJECT
    
public:
    
    /**
     * Constructor
     *
     * @param modeAutomatic TODO
     * @param imagesPreviouslyLoaded TODO
     * @param mode3D TODO
     */
    ContrastProcessCaseParallel(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D);
    
    /**
     * @todo write docs
     */
    ~ContrastProcessCaseParallel();    

    /**
     * @todo write docs
     *
     * @param numThreads TODO
     * @return TODO
     */
    virtual void setNumThreads(int numThreads) Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @return TODO
     */
    virtual void stopProcess() Q_DECL_OVERRIDE;
    
protected slots:

    void executionStopped();
    void run() Q_DECL_OVERRIDE;
    void onGradientComputed();

protected:
    
    void execute() Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @return TODO
     */
    void saveResults() override;

    /**
     * @todo write docs
     *
     * @param  TODO
     * @param  TODO
     * @param  TODO
     * @return TODO
     */
    void mainLoop2D(int , int , int ) override;

    /**
     * @todo write docs
     *
     * @param type TODO
     * @return TODO
     */
    void convertImages(Utils::ImageTypes) override;

    /**
     * @todo write docs
     *
     * @return TODO
     */
    virtual void mainLoop3D() Q_DECL_OVERRIDE;
    void computeGradient(int startSlice, int endSlice) Q_DECL_OVERRIDE;
    
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

#endif // CONTRASTPROCESSCASEPARALLEL_H
