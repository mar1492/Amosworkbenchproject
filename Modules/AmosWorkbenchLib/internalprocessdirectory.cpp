#include "internalprocessdirectory.h"
#include "amosprocesscasethreaded.h"
#include "amosprocesscaseparallel.h"

#include <iostream>

using namespace std;

InternalProcessDirectory::InternalProcessDirectory()
{
    modeAutomatic = true;
    numImagesAmos = 0;
    stop = false;
    numThreads = QThread::idealThreadCount();
    numThreads = numThreads > 0 ? numThreads : 1;
    numSimultaneousSubjects = 1;
    useParallel = false;
    
    casesStarted = 0;
    casesFinished = 0;
    casesStopped = 0;
    numValidCases = 0;
    imagesSaved = 0;
    
    amosProcessRange = 0;
    numCaseProgress = 0;
    
    imagesPreviouslyLoaded = false;
    subjectsAsMainDirs = false;
    hasMask = false;
    
    connect(this, SIGNAL(sendStartCase()), this, SLOT(processNewDir()));
}

InternalProcessDirectory::~InternalProcessDirectory()
{
    for(int i = 0; i < amosThreads.size(); i++) {
        if(!amosThreads.at(i).isNull())
            delete amosThreads[i];
    }
    amosThreads.clear();
    avaliableAmosThreads.clear();
    cout << "InternalProcessDirectory deleted" << endl;
}

void InternalProcessDirectory::setModeAutomatic(bool modeAutomatic)
{
    this->modeAutomatic = modeAutomatic;
}

void InternalProcessDirectory::setMode3D(bool mode3D)
{
    this->mode3D = mode3D;
}

void InternalProcessDirectory::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
}

bool InternalProcessDirectory::setParameters(QMap<QString, QString> parameters)
{
    this->parameters = parameters;
    return true;
}

void InternalProcessDirectory::setImagesPreviouslyLoaded(bool imagesPreviouslyLoaded)
{
    this->imagesPreviouslyLoaded = imagesPreviouslyLoaded;
}

void InternalProcessDirectory::setNumThreads(int numThreads)
{
    this->numThreads = numThreads;
}

void InternalProcessDirectory::setNumSimultaneousSubjects(int numSimultaneousSubjects)
{
    this->numSimultaneousSubjects = numSimultaneousSubjects;
}

void InternalProcessDirectory::setSubjects(QStringList subjects)
{
    this->subjects = subjects;
}

bool InternalProcessDirectory::setSubjectName(QString subjectName)
{
    if(imagesPreviouslyLoaded) {
        numValidCases = 1;
        this->subjectName = subjectName;
        return true;
    } 
    else
        return false;
    
}

void InternalProcessDirectory::setOutNames(QStringList amosImagesListOut)
{
    this->amosImagesListOut = amosImagesListOut;
}

bool InternalProcessDirectory::setAmosImagesData(QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn, QString outputParentPath, QString outputPath, QStringList amosImagesListOut)
{
    if(imagesPreviouslyLoaded) 
        return false;
    
    this->amosImagesListIn = amosImagesListIn;
    this->amosImagesListOut = amosImagesListOut;
    if(subjectsAsMainDirs)
        OutParentDirectory = QDir(outputParentPath);
    else {
        QFileInfo fileInfo(outputParentPath, outputPath);
        OutParentDirectory = QDir(fileInfo.absoluteFilePath());
    }
    this->outputPath = outputPath;

    if(!OutParentDirectory.exists()) {
        if(!OutParentDirectory.mkdir(OutParentDirectory.dirName())) {
            emit sendLog(tr("Can not create output directory ") + OutParentDirectory.dirName());
            // 	    cout << "Can not create output directory " << OutParentDirectory.dirName().toStdString() << endl;
            return false;
        }
    } 

    if(amosImagesListIn.size() < numImagesAmos) {
        emit sendLog(tr("Not enough images"));
        return false;
    }
    
    if(!processSubDirs())
        return false;
    
    if(numValidCases == 0) {
        emit sendLog(tr("No valid cases."));
        return false;
    }
    return true;    
}

void InternalProcessDirectory::processCases()
{
    
    begin = chrono::high_resolution_clock::now();
    casesFinished = 0;
    casesStarted = 0;
    casesStopped = 0;
    imagesSaved = 0;
    stop = false;
    
    fullyProcessedSubjects.clear();

    int threadsPerProccess = numThreads/numSimultaneousSubjects;
    if(threadsPerProccess > 1)
        useParallel = true;
    else {
        threadsPerProccess = 1;
        useParallel = false;
    }   
    
    for(int i = 0; i < numSimultaneousSubjects; i++) {
        InternalProcessCase *amos = createProcess(threadsPerProccess);
 
        amosThreads.append(amos);
        avaliableAmosThreads.append(amos);
        
        amos->initHeap();
        amos->setSubjectsAsMainDirs(subjectsAsMainDirs);
        amos->setParameters(parameters);
        
        connect(amos, SIGNAL(sendLog(QString)), this, SIGNAL(sendLog(QString)));
        connect(amos, SIGNAL(sendDifferentQforms(QString)), this, SLOT(onDifferentQforms( QString)));
        connect(amos, SIGNAL(sendNotEnoughWM(QString)), this, SLOT(onNotEnoughWM( QString)));
        
        if(numValidCases == 1) {
            connect(amos, SIGNAL(sendProgressRange(int)), this, SIGNAL(sendProgressRange(int)));
            connect(amos, SIGNAL(sendProgress(int)), this, SIGNAL(sendProgress(int)));	    
            connect(amos, SIGNAL(sendProgressType(QString)), this, SIGNAL(sendProgressType(QString)));
            connect(amos, SIGNAL(sendCaseProgress(int)), this, SIGNAL(sendCaseProgress(int)));
        }
        else {
            emit sendCaseName(tr("Started"));
            emit sendProgressType(tr("Finished"));
            emit sendProgressRange(numValidCases);
            emit sendCaseProgressRange(numValidCases);
        }
        
        connect(amos, SIGNAL (sendExecutionFinished(InternalProcessCase*, int, bool)), this, SLOT (caseFinished(InternalProcessCase*, int, bool)));
        
        qRegisterMetaType<Image::Pointer>();
        qRegisterMetaType<QFileInfo>();
        connect(amos, SIGNAL(sendSaveImage(Image::Pointer, QFileInfo)), this, SLOT(imageToSave(Image::Pointer, QFileInfo)));
	
        if(!imagesPreviouslyLoaded)
            amos->setOutputDirectories(OutParentDirectory.absolutePath(), outputPath);
        amos->setOutputNames(amosImagesListOut);
        
        loadExternalFiles(amos);       
    }
    
    for(int i = 0; i < numSimultaneousSubjects; i++) 
        emit sendStartCase();  
    
}

void InternalProcessDirectory::processNewDir()
{
    bool rightDir = true;
    int numCase = casesStarted;
    if(stop || numCase >= numValidCases) { // It is numCase >= numValidCases if any case is valid
        // 	stop = false;
        return;
    }
    
    if(!imagesPreviouslyLoaded)
        subjectName = subjects.at(numCase);
    casesStarted++;    
    
    segmentationResults.clear();
    if(!imagesPreviouslyLoaded) {
        if(numValidCases == 1) {
            emit sendProgressType(tr("Loading"));
            emit sendCaseProgressRange(amosProcessRange);
            emit sendCaseName(subjectName);
        }
        else {
            emit sendCaseProgress(casesStarted);
        }
        imageFiles.clear();
        
        if(!processNewSubDirs(rightDir, numCase))
            return;
        
        if(!loadImages()) {
            emit sendMessageToUser(subjectName + " " + tr("does not contains valid or enough images"));             
            badCase(numCase);
            return;
        }
    }
    
    beginAmos = chrono::high_resolution_clock::now();;
    
    if(!imagesPreviouslyLoaded) {
        QFileInfo outDir;
        cout << OutParentDirectory.absolutePath().toStdString() << endl;
            outDir.setFile(OutParentDirectory, subjectName);
            if(!outDir.exists()) {
                QDir OutParentDirectoryCase;
                if(!OutParentDirectoryCase.mkdir(outDir.absoluteFilePath())) {
                    emit sendMessageToUser(tr("Can not create output directory for case") + " " + subjectName);                     
                    badCase(numCase);
                    return;
                }
            }           
        }
    if(stop) {
        badCase(numCase);
        return;
    }

    if(avaliableAmosThreads.count() == 0) {
        casesStarted--;
        return;
    }

    InternalProcessCase* thread = avaliableAmosThreads.takeFirst();
    if(!thread->setImages(numCase, imagesMITK, subjectName)) {
        emit sendMessageToUser(subjectName + " " + tr("does not contains valid or enough images"));
        badCase(numCase);
    }

}

void InternalProcessDirectory::badCase(int caseNum)
{
    // QTimer to leave some time before launch a new case
    // if there are many invalid cases launches mix
    if(numValidCases > 1)
        emit sendProgress(casesFinished);
    else
        emit sendCaseProgress(amosProcessRange);
    if(casesStarted < numValidCases) {
        casesFinished++;
        emit sendStartCase();	
    }
    else 
        caseFinished(0, caseNum, false);          
}

void InternalProcessDirectory::caseFinished(InternalProcessCase* thread, int numCase, bool ok)
{
    if(!stop)
        ++casesFinished;
    
    if(numValidCases > 1)
        emit sendProgress(casesFinished);
    if(stop) {
        casesStopped++;
        avaliableAmosThreads.removeOne(thread);
        thread = 0;
        cout << "Cases stopped " << QString::number(casesStopped).toStdString() << endl;
    }
    else if(ok && !imagesPreviouslyLoaded)
        fullyProcessedSubjects.append(subjects.at(numCase));
    
    if(casesFinished == numValidCases || (casesStopped > 0 && casesStopped == casesStarted - casesFinished)/* || !numAmosRunning*/) {
        cout << "amosThreads.size " + QString::number(amosThreads.size()).toStdString() << endl;
        
        clearImages();
        for(int i = 0; i < amosThreads.size(); i++) 
            amosThreads.at(i)->deleteLater();	    

        auto end = chrono::high_resolution_clock::now();
        int mSecs = chrono::duration_cast<chrono::nanoseconds>(end - begin).count()/1000000.0;
        emit sendTime(mSecs);
      
        if(imagesPreviouslyLoaded)
            emit sendSegmentationResults(segmentationResults);
        emit processedSubject(fullyProcessedSubjects);
        emit processFinished(); 
        stop = false;	
    }
    else if(casesStarted < numValidCases) {
        if(thread != 0)
            avaliableAmosThreads.append(thread); 	
        emit sendStartCase();
    }

}

bool InternalProcessDirectory::loadImages()
{       
    Image::Pointer image;
    
    clearImages(); 
    if(stop)
        return false;   
    
    if(numValidCases == 1) 
        emit sendProgressRange(imageFiles.size());
    for(int i = 0; i < imageFiles.size(); i++) {
        
        if(i == numImagesAmos) { // Mask
            if(!imageFiles.at(numImagesAmos).exists()) {
                imagesMITK.append(0);
                if(numValidCases == 1) 
                    emit sendProgress(i + 1);
                continue;
            }
        }
        std::string fileName = imageFiles.at(i).absoluteFilePath().toStdString();
        image = IOUtil::Load<mitk::Image>(fileName);
        if(image.IsNull()) {
            clearImages();
            return false;
        }
        imagesMITK.append(image);
        if(numValidCases == 1) 
            emit sendProgress(i + 1);   
    }
    
    return true;
}

void InternalProcessDirectory::clearImages()
{
    for(int i = 0; i < imagesMITK.size(); i++)
        imagesMITK[i] = NULL; 
    
    imagesMITK.clear();
}

void InternalProcessDirectory::stopProcess()
{
    numValidCases = casesStarted;
    stop = true;
    //     if(amos)
    if(amosThreads.size() == 0)
        emit processFinished();
    for(int i = 0; i < amosThreads.size(); i++) {
        amosThreads.at(i)->stopProcess();
    }
    if(!imagesPreviouslyLoaded) {
        clearImages();
        emit processedSubject(fullyProcessedSubjects);
    }
}

void InternalProcessDirectory::imageToSave(Image::Pointer image3D, QFileInfo filePath)
{
    
    if(numValidCases == 1 && imagesSaved == 0) {
        emit sendProgressType(tr("Writing"));
        emit sendProgressRange(numMITKOutputImages);
        emit sendCaseProgress(numCaseProgress);
    }
    
    if(imagesPreviouslyLoaded)
        segmentationResults.append(QPair<Image::Pointer, QString>(image3D, filePath.baseName()));
    else {
        IOUtil::Save(image3D, filePath.absoluteFilePath().toStdString());
        image3D = NULL;
    }
    image3D = NULL;
    if(numValidCases == 1) {
        imagesSaved++;
        emit sendProgress(imagesSaved);
        if(imagesSaved == numMITKOutputImages)
            emit sendCaseProgress(amosProcessRange);
    }
}

void InternalProcessDirectory::setImagesList(QList<Image::Pointer> imagesMITK)
{
    this->imagesMITK = imagesMITK;
}

void InternalProcessDirectory::onDifferentQforms(QString subjectName)
{
    emit sendMessageToUser(subjectName + " " + tr("images qforms are different, skipping"));
}

void InternalProcessDirectory::onNotEnoughWM(QString subjectName)
{
    emit sendMessageToUser(subjectName + " " + tr("does not have slices with enough WM"));
}   

int InternalProcessDirectory::getNumMITKOutputImages()
{
    return numMITKOutputImages;
}

void InternalProcessDirectory::increaseNumMITKOutputImages()
{
    numMITKOutputImages++;
}
