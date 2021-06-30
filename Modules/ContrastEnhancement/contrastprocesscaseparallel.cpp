#include "contrastprocesscaseparallel.h"

ContrastProcessCaseParallel::ContrastProcessCaseParallel(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D) : ContrastProcessCase(modeAutomatic, imagesPreviouslyLoaded, mode3D)
{
    threadFinished = 0;
    connect(this, SIGNAL(startExecution()), this, SLOT(run()));
    firstTimeStopped = true;
    numWatcherStopped = 0;
    watcherCreated = false;
    numUsedThreads = 1;
}

ContrastProcessCaseParallel::~ContrastProcessCaseParallel()
{
    futures.clear();
    for(int i = 0; i < watchers.size(); i++)
        watchers[i]->deleteLater();    
//         delete watchers[i];
    watchers.clear();    
}

void ContrastProcessCaseParallel::run()
{
    emit sendLog("ContrastProcessCaseParallel::execute case " + subjectName);
    //     cout << "ContrastProcessCaseParallel::execute case " << outputDirectory.dirName().toStdString() << endl;
    
    cout << QString::number(numCase + 1).toStdString()  + " ContrastProcessCaseParallel running" << endl;
    
    nonLoopExecute();
    
    execute();
}

void ContrastProcessCaseParallel::saveResults()
{
    ContrastProcessCase::saveResults();
}

void ContrastProcessCaseParallel::mainLoop2D(int , int , int )
{

}

void ContrastProcessCaseParallel::convertImages(Utils::ImageTypes /*type*/)
{

}

void ContrastProcessCaseParallel::setNumThreads(int numThreads)
{
    ContrastProcessCase::setNumThreads(numThreads);
    for(int i = 0; i < numThreads; i++) {
        QFutureWatcher<void>* watcher = new QFutureWatcher<void>();
        watchers.append(watcher);
        futures.append(QFuture<void>());
    }
}

void ContrastProcessCaseParallel::stopProcess()
{

}

void ContrastProcessCaseParallel::execute()
{

    emit sendLog("ContrastProcessCaseParallel::execute case " + subjectName);
    //     cout << "ContrastProcessCaseParallel::execute case " << outputDirectory.dirName().toStdString() << endl;
    
    cout << QString::number(numCase + 1).toStdString()  + " ContrastProcessCase running" << endl;
    
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
 
    createCubes();        
    
    SLICE_INI_LOOP = 0;
    SLICE_END_LOOP = FL3Dcube.n_slices - 1;
    
    if(stop) {
    //     	stop = false;
        clearMemory();
        return;
    } 
    caseProgress = 2;
    progressType = tr("Computing gradient");
    changeProgres();    
    
    if(!mode3D) {
        
        for(int i = 0; i < numThreads; i++)
            watchers[i]->disconnect();
    
        threadFinished = 0;
        sliceCounter = 0;
        
        int range = (int)dimImg3D[2];
        
        int quotient = range / numThreads;   
        int remainder = range % numThreads;
        
        int s_init = 0;
        int s_end;  
        
        numUsedThreads = min(range, numThreads);
        
        for(int i = 0; i < numUsedThreads; i++) {
            if(i < remainder)
                s_end = s_init + quotient;
            else
                s_end = s_init + quotient - 1;
            slicesRanges.append(QPair<int, int>(s_init, s_end));
            s_init = s_end + 1;
            
//             EDISON* edison = new EDISON();
            EDISON edison;
            edison.SetParametersNew(parameters);
            edisons.append(edison);
            edisonNum = 0;
                        
            connect(watchers[i], SIGNAL(finished()), this, SLOT(onGradientComputed()));
            connect(watchers[i], SIGNAL(finished()), this, SLOT(executionStopped()));
            futures[i] = QtConcurrent::run(this,  &ContrastProcessCaseParallel::computeGradient, slicesRanges.at(i).first, slicesRanges.at(i).second);
            watchers[i]->setFuture(futures.at(i));
        }        
    }    
    else {
        for(int i = 0; i < numThreads; i++) {
            watchers[i]->disconnect();
        }
        if(stop) {
            return;
        }        
        threadFinished = numUsedThreads - 1;
    }    
    
    if(stop) {
        return;
    }
}

void ContrastProcessCaseParallel::mainLoop3D()
{

}

void ContrastProcessCaseParallel::executionStopped()
{
    if(stop) {
        if(firstTimeStopped) {
            for(int i = 0; i < watchers.count(); i++) {
                if(watchers.at(i)->isFinished())
                    numWatcherStopped++;
            }
            firstTimeStopped = false;
        }
        else
            numWatcherStopped++;
        if(numWatcherStopped == watchers.count()) {
            cout << "ContrastProcessCaseParallel::executionStopped numWatcherStopped = watchers.count(), stopping" << endl;
            clearMemory();
            executionFinished(false);
            firstTimeStopped = true;
        }    
    }
}



void ContrastProcessCaseParallel::onGradientComputed()
{
    threadFinished++;
    if(threadFinished == numUsedThreads) {
        threadFinished = 0; 
//         for(int i = 0; i < edisons.size(); i++)             
//             delete edisons[i];
        edisons.clear();
        edisonNum = 0;
        enhanceContrast();
        saveResults();
    
        clearMemory();
        
        // To call executionFinished(true) after saveResults() for progressLabel show final text "Procedure"
        QTimer::singleShot(0, this, SLOT(executionFinished()));
        
    }
}

void ContrastProcessCaseParallel::computeGradient(int startSlice, int endSlice)
{
    // QtConcurrent::run cannot call parent protected function
    ContrastProcessCase::computeGradient(startSlice, endSlice);
}
