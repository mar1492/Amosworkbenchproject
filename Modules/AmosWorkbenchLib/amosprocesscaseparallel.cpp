#include "amosprocesscaseparallel.h"

using namespace arma;

AmosProcessCaseParallel::AmosProcessCaseParallel(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D) : AmosProcessCase(modeAutomatic, imagesPreviouslyLoaded, mode3D)
{
    threadFinished = 0;
    connect(this, SIGNAL(startExecution()), this, SLOT(run()));
    firstTimeStopped = true;
    numWatcherStopped = 0;
    watcherCreated = false;
    numUsedThreads = 1;    
}

AmosProcessCaseParallel::~AmosProcessCaseParallel()
{
    futures.clear();
    for(int i = 0; i < watchers.size(); i++)
        watchers[i]->deleteLater();
//         delete watchers[i];
    watchers.clear();
}

bool AmosProcessCaseParallel::setImages(int numCase, QList<Image::Pointer> imagesMITK, QString caseName, int SLICE_INI_LOOP, int SLICE_END_LOOP)
{
    bool ret = AmosProcessCase::setImages(numCase, imagesMITK, caseName, SLICE_INI_LOOP, SLICE_END_LOOP);
    if(ret && !stop) {
        emit startExecution();
    }
    return ret;
}

void AmosProcessCaseParallel::run()
{
    execute();
}

void AmosProcessCaseParallel::setNumThreads(int numThreads)
{
    AmosProcessCase::setNumThreads(numThreads);
    for(int i = 0; i < numThreads; i++) {
        QFutureWatcher<void>* watcher = new QFutureWatcher<void>();
        watchers.append(watcher);
        futures.append(QFuture<void>());
    }
}

/**
 * @brief Execute AMOS algorithm for one case
 * 
 * @return void
 */

void AmosProcessCaseParallel::execute()
{
    // WM labels
    emit sendLog("AmosProcessCaseParallel::execute case " + subjectName);
    //     cout << "AmosProcessCaseParallel::execute case " << outputDirectory.dirName().toStdString() << endl;
    
    caseProgress = 1;
    progressType = tr("Converting");
    progressRange = convertProgressRange;
    changeProgres();
    
    if(!calculateOrientation()) {
        cout << "AmosProcessCaseParallel::execute calculateOrientation() failed, stopping" << endl;
        clearMemory();
//         executionFinished(false);
        finishedOk = false;
        stop = true;
//         quit();
        return;
    }
    
    convertCounter = 0;
    convertImageMaskAndLB();
//     generateLBWM3D();

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
                        
            connect(watchers[i], SIGNAL(finished()), this, SLOT(onErodeLBWM3DFinished()));
            connect(watchers[i], SIGNAL(finished()), this, SLOT(executionStopped()));
            futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::WMMask3DROICompute2D, slicesRanges.at(i).first, slicesRanges.at(i).second);
//             futures.append(QtConcurrent::run(this, &AmosProcessCaseParallel::WMMask3DROICompute2D, slicesRanges.at(i).first, slicesRanges.at(i).second));
            watchers[i]->setFuture(futures.at(i));
        }        
    }    
    else {
        for(int i = 0; i < numThreads; i++) {
            watchers[i]->disconnect();
//             futures.append(QFuture<void>());
        }
        if(stop) {
            //     	stop = false;
            // 	stopFutures();
            return;
        }        
        WMMask3DROICompute3D();
        threadFinished = numUsedThreads - 1;
        onErodeLBWM3DFinished();
    }    
    
    //     parallelConvertImages();
    
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }       
}

void AmosProcessCaseParallel::parallelConvertImages()
{
    conversionFinished = 0;
    conversionStarted = 0;
    
    for(int i = 0; i < numUsedThreads; i++)
        watchers[i]->disconnect();
    
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }
    numConversions = numThreads < listTypes.size() ? listTypes.size() : numThreads;
    numThreadsForConvert = numThreads / listTypes.size() == 0 ? 1 : numThreads / listTypes.size();
    watcherCreated = true;
    for(int i = 0; i < numThreads; i++) {
        watchers[i]->disconnect();
        connect(watchers[i], SIGNAL(finished()), this, SLOT(onImagesConverted()));
        connect(watchers[i], SIGNAL(finished()), this, SLOT(executionStopped()));
        if(i < listTypes.size())
            futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::convertImages, listTypes.at(i));
        else
            futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::convertImages, Utils::ALL); // do nothing
        watchers[i]->setFuture(futures.at(i));
        conversionStarted++;
    }    
}

void AmosProcessCaseParallel::convertImages(Utils::ImageTypes type)
{
    if(type == Utils::ALL)
        return;
    else
        AmosProcessCase::convertImages(type);
}

void AmosProcessCaseParallel::onImagesConverted()
{
    conversionFinished++;
    if(conversionFinished == numConversions) {
        threadFinished = 0;
        sliceCounter = 0;
        numSlicesWithLBWM3DMinSize = 0;
        LB3Dcube.reset();
        nonLoopExecute();
    }
    // 	threadFinished = 0;
    // 	sliceCounter = 0;
    // 
    // 	int range = SLICE_END_LOOP - SLICE_INI_LOOP + 1;
    // 	
    // 	int quotient = range / numThreads;   
    // 	int remainder = range % numThreads;
    // 
    // 	int s_init = SLICE_INI_LOOP;
    // 	int s_end;
    // 	for(int i = 0; i < numThreads; i++) {
    // 	    if(i < remainder)
    // 		s_end = s_init + quotient;
    // 	    else
    // 		s_end = s_init + quotient - 1;
    // 	    slicesRanges.append(QPair<int, int>(s_init, s_end));
    // 	    s_init = s_end + 1;
    // 	    
    // 	    watchers[i]->disconnect();
    // 	    connect(watchers[i], SIGNAL(finished()), this, SLOT(onErodeLBWM3DFinished()));
    // 	    connect(watchers[i], SIGNAL(finished()), this, SLOT(executionStopped()));
    // 	    
    //             futures.append(QtConcurrent::run(this, &AmosProcessCaseParallel::thresholdsLoop, s_init, s_end));
    // 	    futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::WMMask3DROICompute2D(slicesRanges.at(i).first, slicesRanges.at(i).second);
    // 	    watchers[i]->setFuture(futures.at(i));
    // 
    // 	}		
    //     }
    else if(conversionStarted < numConversions) {
        for(int i = 0; i < numThreads; i++) {
            if(futures.at(i).isFinished()) {
                if(conversionStarted < listTypes.size())
                    futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::convertImages, listTypes.at(conversionStarted));
                else
                    futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::convertImages, Utils::ALL);
                watchers[i]->setFuture(futures.at(i));
                conversionStarted++;
                break;
            }
        }
    }    
    
}

void AmosProcessCaseParallel::nonLoopExecute()
{
    AmosProcessCase::nonLoopExecute();
    
    beginThres = chrono::high_resolution_clock::now();
    
    for(int i = 0; i < numThreads; i++)
        watchers[i]->disconnect();
    
    caseProgress = 2;
    progressType = tr("Thresholding");
    progressRange = SLICE_END_LOOP - SLICE_INI_LOOP + 1;
    changeProgres();
    //     QTimer::singleShot(0, this, SLOT(changeProgres()));       
    
    threadFinished = 0;
    sliceCounter = 0;
    vectorsForScanSegmentTH();
    
    int range = SLICE_END_LOOP - SLICE_INI_LOOP + 1;
    
    int quotient = range / numThreads;   
    int remainder = range % numThreads;
    
    int s_init = SLICE_INI_LOOP;
    int s_end;
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }
    slicesRanges.clear();
    //     futures.clear();
    numUsedThreads = min(range, numThreads);
    for(int i = 0; i < numUsedThreads; i++) {
        if(i < remainder)
            s_end = s_init + quotient;
        else
            s_end = s_init + quotient - 1;
        slicesRanges.append(QPair<int, int>(s_init, s_end));
        s_init = s_end + 1;
                
        connect(watchers[i], SIGNAL(finished()), this, SLOT(onThresholdsLoopFinished()));
        connect(watchers[i], SIGNAL(finished()), this, SLOT(executionStopped()));
        
        //         futures.append(QtConcurrent::run(this, &AmosProcessCaseParallel::thresholdsLoop, s_init, s_end));
        futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::thresholdsLoop, slicesRanges.at(i).first, slicesRanges.at(i).second);
        watchers[i]->setFuture(futures.at(i));
        
    }    
}

void AmosProcessCaseParallel::WMMask3DROICompute2D(int startSlice, int endSlice)
{
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }    
    AmosProcessCase::WMMask3DROICompute2D(startSlice, endSlice);    
}

void AmosProcessCaseParallel::blobsFeaturesLoop(int blobInit, int blobEnd)
{
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }    
    AmosProcessCase::blobsFeaturesLoop(blobInit, blobEnd);    
}

void AmosProcessCaseParallel::onErodeLBWM3DFinished()
{
    threadFinished++;
    if(threadFinished == numUsedThreads) {
        threadFinished = 0;
        // 	string fileName("WMMask3DROICube.nii");
        // 	Image::Pointer LBWM3DEImg = Utils::cubeToMITKImage<unsigned char>(&WMMask3DROICube);    
        // 	IOUtil::Save(LBWM3DEImg, fileName);
        
        calculateWMVol();
        
        parallelConvertImages();
        
        // 	nonLoopExecute();
    }
}

void AmosProcessCaseParallel::thresholdsLoop(int startSlice, int endSlice)
{
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }    
    AmosProcessCase::thresholdsLoop(startSlice, endSlice);
}

void AmosProcessCaseParallel::mainLoop2D(int startSlice, int endSlice, int threadIndex)
{
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }
    AmosProcessCase::mainLoop2D(startSlice, endSlice, threadIndex);
}

void AmosProcessCaseParallel::onThresholdsLoopFinished()
{
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    } 
    
    threadFinished++;
    if(threadFinished == numUsedThreads) {
        threadFinished = 0;
        
        if(numSlicesWithLBWM3DMinSize == 0) {
            emit sendNotEnoughWM(subjectName);
            clearMemory();
//             executionFinished(false);
            finishedOk = false;
            stop = true;
//             quit();
            return;
        }
        
        calculateThresholds();
        
        auto endTH = chrono::high_resolution_clock::now();
        if(Utils::showTime)
            emit sendLog("TH13D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(endTH - beginThres).count()/1000000.0) + "ms."); 
        
        executeMainLoop();
    }
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }    
}

void AmosProcessCaseParallel::executeMainLoop()
{
    caseProgress = 3;
    progressType = tr("Segmenting");
    if(!mode3D)
        progressRange = SLICE_END_LOOP - SLICE_INI_LOOP + 1;
    changeProgres();
//     progressRange = SLICE_END_LOOP - SLICE_INI_LOOP + 1;
//     changeProgres();
    //     QTimer::singleShot(0, this, SLOT(changeProgres()));      
    
    threadFinished = 0;
    sliceCounter = 0;
    //     futures.clear();
    
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }     
    
    if(!mode3D) {
        for(int i = 0; i < numUsedThreads; i++)
            watchers[i]->disconnect();
        
        numUsedThreads = min(slicesRanges.last().second, numThreads);
        for(int i = 0; i < numUsedThreads; i++) {
            connect(watchers[i], SIGNAL(finished()), this, SLOT(saveResults()));
            connect(watchers[i], SIGNAL(finished()), this, SLOT(executionStopped()));
            
            //         futures.append(QtConcurrent::run(this, &AmosProcessCaseParallel::mainLoop2D, slicesRanges.at(i).first, slicesRanges.at(i).second));
            futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::mainLoop2D, slicesRanges.at(i).first, slicesRanges.at(i).second, i);
            watchers[i]->setFuture(futures.at(i));
        }
    }
    else {
        for(int i = 0; i < numThreads; i++)
            watchers[i]->disconnect();        
        mainLoop3D();
//         threadFinished = numThreads - 1;
//         saveResults();
    }
}

void AmosProcessCaseParallel::mainLoop3D()
{
//     double Ks1, Kth1, Ks2, Kth2;
    startMainLoop3D();

//     Cube<unsigned char> Iroi;
//     Cube<unsigned char> WMmask3DROIBinCube;
//     Cube<unsigned char> WMmask3DModelBinCube;
//     Cube<unsigned char> Ich;
    computeIROI3D();
    Ich3D.set_size(size(Iroi3D));
    IroiHasForeground3D = Utils::isNotZero< Cube<unsigned char> >(&Iroi3D) ? ImageFilters::YES : ImageFilters::NO;
    
    numUsedThreads = min(slicesRanges.last().second, numThreads);    
    for(int i = 0; i < numUsedThreads; i++) {
//         watchers[i]->disconnect();
        connect(watchers[i], SIGNAL(finished()), this, SLOT(onComputeRegionProps3D()));
        connect(watchers[i], SIGNAL(finished()), this, SLOT(executionStopped()));
        
        //         futures.append(QtConcurrent::run(this, &AmosProcessCaseParallel::mainLoop2D, slicesRanges.at(i).first, slicesRanges.at(i).second));
        futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::computeRegionProps3D, slicesRanges.at(i).first, slicesRanges.at(i).second, i);
        watchers[i]->setFuture(futures.at(i));
    }       
    
}

void AmosProcessCaseParallel::computeRegionProps3D(uint iniSlice, uint endSlice, int threadIndex)
{
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        return;
    }
    AmosProcessCase::computeRegionProps3D(iniSlice, endSlice, threadIndex);    
}

void AmosProcessCaseParallel::onComputeRegionProps3D()
{
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        clearMemory();
        return;
    }    
    threadFinished++;
    if(threadFinished == numUsedThreads) {
        threadFinished = 0;
        
        // Distance to convexhull of the region of interest
        // Used to classify WML as function of periphery distance.    

        //fcube DistToWMBorder;     //mari
        //computeDistsTo(&DistToWMBorder); 
        computeDistsTo();


        
        // *******************  Step 1: SEGMENTATION  *******************
        
        Cube<unsigned char> IsegR;
        computeISEGR(&IsegR);
        
        // ********************* Step 2: CLASIFICATION *********************
        
        // ***************** Step 2.1  COMPUTING BLOB FEATURES ****************************
        
        // slice segmentations: 1) before (initial segmentation) and 2) after the classifier (final segmentation)

        computeBlobsForFeatures(&IsegR);
        
        Iseg3D_AC_blobs = Iseg3D_BC_blobs;
        
        for(int i = 0; i < numUsedThreads; i++)
            watchers[i]->disconnect();
        
        int range = labelsInfoBC3D.size();
        
        int quotient = range / numThreads;   
        int remainder = range % numThreads;
        
        int s_init = 0;
        int s_end;
        if(stop) {
            //     	stop = false;
            // 	stopFutures();
            return;
        }
        QList< QPair<int, int> > blobRanges;
        numUsedThreads = min(range, numThreads);
        //     futures.clear();
        for(int i = 0; i < numUsedThreads; i++) {
            if(i < remainder)
                s_end = s_init + quotient;
            else
                s_end = s_init + quotient - 1;
            blobRanges.append(QPair<int, int>(s_init, range - s_end - 1));
            s_init = s_end + 1;
                        
            connect(watchers[i], SIGNAL(finished()), this, SLOT(onBlobsFeaturesLoopFinished()));
            connect(watchers[i], SIGNAL(finished()), this, SLOT(executionStopped()));
            
            //         futures.append(QtConcurrent::run(this, &AmosProcessCaseParallel::thresholdsLoop, s_init, s_end));
            futures[i] = QtConcurrent::run(this, &AmosProcessCaseParallel::blobsFeaturesLoop, blobRanges.at(i).first, blobRanges.at(i).second);
            watchers[i]->setFuture(futures.at(i));            
        }         
    }     
}

void AmosProcessCaseParallel::onBlobsFeaturesLoopFinished()
{ // only called in mode3D
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        clearMemory();
        return;
    }    
    threadFinished++;
    if(threadFinished == numUsedThreads) {
        threadFinished = 0;
        numThreadsForConvert = numThreads / 2 == 0 ? 1 : numThreads / 2; // 2 only for segmented images, not for Flair if imagesPreviouslyLoaded, Flair is not resampled in AmosProcessCase::saveResults()
        overwriteACBC3D();
        AmosProcessCase::saveResults();
        clearMemory();
        executionFinished(true);
    }     
}

void AmosProcessCaseParallel::saveResults()
{ // only called in !mode3D
    if(stop) {
        //     	stop = false;
        // 	stopFutures();
        clearMemory();
        return;
    }    
    threadFinished++;
    if(threadFinished == numUsedThreads) {
        threadFinished = 0;
        numThreadsForConvert = numThreads / 2 == 0 ? 1 : numThreads / 2; // 2 only for segmented images, not for Flair if imagesPreviouslyLoaded, Flair is not resampled in AmosProcessCase::saveResults()
        AmosProcessCase::saveResults();
        clearMemory();
        executionFinished(true);
    }    
}

void AmosProcessCaseParallel::stopFutures()
{
    //     for(int i = 0; i < futures.size(); i++) {
    // 	watchers[i]->disconnect();	
    //         futures[i].cancel();
    //     }
    //     futures.clear();
    //     watchers.clear();
}

void AmosProcessCaseParallel::stopProcess() {
    //     stopFutures();
    AmosProcessCase::stopProcess();
    
    stop = true;
    if(!watcherCreated) {
        cout << "AmosProcessCaseParallel::stopProcess watcherCreated failed, stopping" << endl;
        clearMemory();
//         executionFinished(false);
    }
    
}

void AmosProcessCaseParallel::executionStopped()
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
//             cout << "AmosProcessCaseParallel::executionStopped numWatcherStopped != watchers.count(), stopping" << endl;
            cout << "AmosProcessCaseParallel::executionStopped numWatcherStopped = watchers.count(), stopping" << endl;
            clearMemory();
            executionFinished(false);
            firstTimeStopped = true;
        }    
    }
}
