#include "contrastprocesscasethreaded.h"


ContrastProcessCaseThreaded::ContrastProcessCaseThreaded(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D) : ContrastProcessCase(modeAutomatic, imagesPreviouslyLoaded, mode3D)
{

}

ContrastProcessCaseThreaded::~ContrastProcessCaseThreaded()
{

}

void ContrastProcessCaseThreaded::run()
{
    emit sendLog("ContrastProcessCaseThreaded::execute case " + subjectName);
    //     cout << "ContrastProcessCaseParallel::execute case " << outputDirectory.dirName().toStdString() << endl;
    
    cout << QString::number(numCase + 1).toStdString()  + " ContrastProcessCaseThreaded running" << endl;
    
    EDISON edison;
    edison.SetParametersNew(parameters);
    edisons.append(edison);
    edisonNum = 0;    
    
    nonLoopExecute();
    
    if(stop) {
        clearMemory();
        return;
    } 
    execute();
    
    if(stop) {
        clearMemory();
        return;
    }    
    
    saveResults();
    
    clearMemory();
    executionFinished(true);
    
}

bool ContrastProcessCaseThreaded::setImages(int numCase, QList< mitk::Image::Pointer > imagesMITK, QString caseName, int SLICE_INI_LOOP, int SLICE_END_LOOP)
{
    bool ret = ContrastProcessCase::setImages(numCase, imagesMITK, caseName, SLICE_INI_LOOP, SLICE_END_LOOP);
    if(ret && !stop) {
        if (!isRunning())
            start();
    }
    return ret;
}
