#include "amosprocesscasethreaded.h"
#include <QTimer>

AmosProcessCaseThreaded::AmosProcessCaseThreaded(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D) : AmosProcessCase(modeAutomatic, imagesPreviouslyLoaded, mode3D)
{

}

AmosProcessCaseThreaded::~AmosProcessCaseThreaded()
{

}

bool AmosProcessCaseThreaded::setImages(int numCase, QList<Image::Pointer> imagesMITK, QString caseName, int SLICE_INI_LOOP, int SLICE_END_LOOP)
{
    bool ret = AmosProcessCase::setImages(numCase, imagesMITK, caseName, SLICE_INI_LOOP, SLICE_END_LOOP);
    if(ret && !stop) {
        if (!isRunning())
            start();
    }
    return ret;
}

void AmosProcessCaseThreaded::run()
{
    emit sendLog("AmosProcessCaseThreaded::execute case " + subjectName);
    cout << QString::number(numCase + 1).toStdString()  + " AmosProcessCaseThreaded running" << endl;
    if(stop) {
    //     	stop = false;
        clearMemory();
        return;
    }
    if(!calculateOrientation()) {
        clearMemory();
//         executionFinished(false);
        finishedOk = false;
//         quit();
        stop = true;
        return;
    }
 
    caseProgress = 1;
    progressType = tr("Converting");
    progressRange = convertProgressRange;
    QTimer::singleShot(0, this, SLOT(changeProgres()));     
    
    convertCounter = 0;
    convertImageMaskAndLB();
//     generateLBWM3D();
    if(!mode3D)
        WMMask3DROICompute2D(0, (int)dimImg3D[2] - 1);
    else
        WMMask3DROICompute3D();
    calculateWMVol();
    convertImages();
    if(stop) {
    //     	stop = false;
        clearMemory();
        return;
    } 
    LB3Dcube.reset();
    execute();
    
    if(stop) {
    // 	stop = false;
        clearMemory();
        return;
    }    
    
    saveResults();
    
    clearMemory();
}
