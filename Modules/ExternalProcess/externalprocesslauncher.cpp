#include "externalprocesslauncher.h"
#include <iostream>
#include <QString>
#include <QTime>

using namespace std;

ExternalProcessLauncher::ExternalProcessLauncher(QObject *parent) : QObject(parent)
{
    stop = false;
    
    casesStarted = 0;
    casesLaunched = 0;
    casesFinished = 0;
    casesStopped = 0;
    numValidCases = 0;    
    amosProcessRange = 0;
    subjectsAsMainDirs = false;
    numImageFilesIn = 0;
    numImageFilesOut = 0;
    delay = 0;
    lastDelay = 0;
    chronoTime = 0;
    numThreads = 1;
    numSimultaneousSubjects = 1;
    useParallel = false;
    
    connect(this, SIGNAL(sendStartCase()), this, SLOT(processNewDir()));    
}

ExternalProcessLauncher::~ExternalProcessLauncher()
{
    for(int i = 0; i < externalProcesses.size(); i++) {
        if(!externalProcesses.at(i).isNull()) {
            externalProcesses[i]->kill();
            externalProcesses[i]->deleteLater();            
        }
    }
    externalProcesses.clear();
    avaliableExternalProcesses.clear();
    cout << "ExternalProcessLauncher deleted" << endl;
}

void ExternalProcessLauncher::setSubjects(QStringList subjects)
{
    this->subjects = subjects;
}

void ExternalProcessLauncher::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
}

void ExternalProcessLauncher::processCases()
{

    begin = chrono::high_resolution_clock::now();
    casesFinished = 0;
    casesStarted = 0;
    casesLaunched = 0;
    casesStopped = 0;
    stop = false;
    fullyProcessedSubjects.clear();
    
//     setNumThreads();

    for(int i = 0; i < numSimultaneousSubjects; i++) {
        ExternalProcessWrapper *process = newProcess();
        
        externalProcesses.append(process);
        avaliableExternalProcesses.append(process);

        int threadsPerProccess = numThreads / numSimultaneousSubjects;
        threadsPerProccess = threadsPerProccess < 1 ? 1 : threadsPerProccess;
        process->setNumThreads(threadsPerProccess);
        process->setDelay(delay * i);
        process->setSubjectsAsMainDirs(subjectsAsMainDirs);
        process->setProgramPath(m_program);
        process->setParameters(parameters);
        connect(process, SIGNAL(sendLog(QString)), this, SIGNAL(sendLog(QString)));        
        
        if(numValidCases == 1) {
            connect(process, SIGNAL(sendProgressRange(int)), this, SIGNAL(sendProgressRange(int)));
            connect(process, SIGNAL(sendProgress(int)), this, SIGNAL(sendProgress(int)));	    
            connect(process, SIGNAL(sendProgressType(QString)), this, SIGNAL(sendProgressType(QString)));
            connect(process, SIGNAL(sendCaseProgress(int)), this, SIGNAL(sendCaseProgress(int)));
        }
        else {
            if(numSimultaneousSubjects == 1) {
/*                connect(process, SIGNAL(sendProgressRange(int)), this, SIGNAL(sendProgressRange(int)));
                connect(process, SIGNAL(sendProgress(int)), this, SIGNAL(sendProgress(int)));*/	    
                connect(process, SIGNAL(sendProgressType(QString)), this, SIGNAL(sendProgressType(QString)));
//                 connect(process, SIGNAL(sendCaseProgress(int)), this, SIGNAL(sendCaseProgress(int)));
            
            }            
            emit sendCaseName(tr("Started"));
            emit sendProgressType(tr("Finished"));
            emit sendProgressRange(numValidCases);
            emit sendCaseProgressRange(numValidCases);
        }
        connect(process, SIGNAL (sendExecutionStarted(ExternalProcessWrapper*, QString)), this, SLOT(caseLaunched(ExternalProcessWrapper*, QString)));
        connect(process, SIGNAL (sendExecutionFinished(ExternalProcessWrapper*, int, bool)), this, SLOT (caseFinished(ExternalProcessWrapper*, int, bool)));
    }
    lastDelay = 0;
    for(int i = 0; i < numSimultaneousSubjects; i++) 
        emit sendStartCase();
}


void ExternalProcessLauncher::processNewDir()
{
    bool rightDir = true;
    int numCase = casesStarted;
    cout << "ExternalProcessLauncher processNewDir 1 " << casesStarted << " " << numValidCases << endl;
    if(stop || numCase >= numValidCases) { // It is numCase >= numValidCases if any case is valid
        // 	stop = false;
        return;
    }
    subjectName = subjects.at(numCase);
    casesStarted++;

    if(numValidCases == 1) {
        emit sendProgressType(tr("Loading"));
        emit sendProgressRange(0);
        emit sendCaseProgressRange(amosProcessRange);
        emit sendCaseName(subjectName);
    }
    else {
//         emit sendCaseProgress(casesStarted);
    }    

    imageFiles.clear();
    appendImageFilesIn(numCase);
    
    if(imageFiles.size() < numImageFilesIn)
        rightDir = false;
    if(!rightDir) {
        emit sendMessageToUser(subjectName + " " + tr("does not contains valid or enough images"));            
        badCase(numCase);
        return;
    }    

    beginAmos = chrono::high_resolution_clock::now();

    QFileInfo outDir;
//     cout << OutParentDirectory.absolutePath().toStdString() << endl;

    outDir.setFile(OutParentDirectory, subjectName);
    if(!outDir.exists()) {
        QDir OutParentDirectoryCase;
        if(!OutParentDirectoryCase.mkdir(outDir.absoluteFilePath())) {
            // emit sendLog(tr("Can not create output directory for case") + " " + OutParentDirectoryCase.dirName() + "\n");
            // 		cout << "Can not create output directory for case " << OutParentDirectoryCase.dirName().toStdString() << endl;
//                     emit sendMessageToUser(tr("Can not create output directory for case") + " " + OutParentDirectoryCase.dirName());
            emit sendMessageToUser(tr("Can not create output directory for case") + " " + subjectName);                     
            badCase(numCase);
            return;
        }
    }            

    if(stop) {
        // 	stop = false;
        badCase(numCase);
        return;
    }
    
    if(avaliableExternalProcesses.count() == 0) {
        casesStarted--;
        return;
    }    

    ExternalProcessWrapper* process = avaliableExternalProcesses.takeFirst();     
    process->setImages(numCase, imageFiles, subjectName);

    QString subdir;
    if(subjectsAsMainDirs) 
        subdir = subjectName + QDir::separator() + outputPath;
    else
        subdir = subjectName;
    outDir.setFile(OutParentDirectory, subdir);
    outputDirectory.setPath(outDir.absoluteFilePath());
    if(!outputDirectory.exists()) {
        QDir dir;
        if(!dir.mkdir(outDir.absoluteFilePath())) {
            emit sendLog("Can not create output directory " + outDir.absoluteFilePath());
            // 	    cout << "Can not create results directory " << objectsFeaturesDir.absoluteFilePath().toStdString() << endl;
//             stopped = true;
//             return false;
            return;
        }
    }
//         this->subjectName = outputDirectory.dirName();
    
    process->setOutputPaths(outputDirectory, setOutputPaths());
    connect(this, SIGNAL(sendChronoInterval(uint)), process, SLOT(addTimeToChrono(uint)));
    process->setDelay(lastDelay);
    process->startChronometer();
    lastDelay += delay > 0 ? delay + 1 : delay;
//     cout << "processNewDir 2 lastDelay " << casesStarted << " " << lastDelay << endl;   
}

void ExternalProcessLauncher::caseLaunched(ExternalProcessWrapper* externalProcess, QString subjectName)
{
    cout << "caseLaunched " << subjectName.toStdString() << " at " << QTime::currentTime().toString().toStdString() << endl; 
    casesLaunched++;
    emit sendCaseProgress(casesLaunched);
    disconnect(this, SIGNAL(sendChronoInterval(uint)), externalProcess, SLOT(addTimeToChrono(uint)));
}

void ExternalProcessLauncher::badCase(int caseNum)
{
    if(numValidCases > 1)
        emit sendProgress(casesFinished);
    else
        emit sendCaseProgress(amosProcessRange);
    if(casesStarted < numValidCases) {
        casesFinished++;
        emit sendStartCase();	
        // 	emit sendStartCase();
    }
    else {
        // 	proccessStarted();
        caseFinished(0, caseNum, false);
    }          
}

void ExternalProcessLauncher::caseFinished(ExternalProcessWrapper* externalProcess, int numCase, bool ok)
{
    //     numAmosRunning--;
    if(!stop)
        ++casesFinished;
    //auto endAmos = chrono::high_resolution_clock::now();
    /*    // emit sendLog("Segmentation time " + subdirsFlair.at(i).absoluteFilePath() + " " + QString::number(chrono::duration_cast<chrono::nanoseconds>(endAmos - beginAmos).count()/1000000.0) + "ms." + "\n");*/
    //     if(imagesPreviouslyLoaded)
    // emit sendLog("Segmentation time " + subjectName + " " + QString::number(chrono::duration_cast<chrono::nanoseconds>(endAmos - beginAmos).count()/1000000.0) + "ms.");
    //     else
    // emit sendLog("Segmentation time " + subdirsFlair.at(numCase).absoluteFilePath() + " " + QString::number(chrono::duration_cast<chrono::nanoseconds>(endAmos - beginAmos).count()/1000000.0) + "ms.");
    // emit sendLog("Num cases finished " + QString::number(casesFinished) + " out of " + QString::number(numValidCases));
    
    
    // 	cout << "Amos time " << subdirsFlair.at(i).absoluteFilePath().toStdString() << " "  << chrono::duration_cast<chrono::nanoseconds>(endAmos - beginAmos).count()/1000000.0 << "ms" << endl;
    
    //     ds = NULL;
    // 	mitk::StandaloneDataStorage::SetOfObjects::ConstPointer dataNodes = ds->GetAll();
    // 	dataNodes->clear();
    
    /*emit sendCaseProgress(i + 1);*/
    
    if(numValidCases > 1)
        emit sendProgress(casesFinished);
    if(stop) {
        casesStopped++;
        avaliableExternalProcesses.removeOne(externalProcess);
        externalProcess = 0;
        cout << "Cases stopped " << QString::number(casesStopped).toStdString() /*<< " out of " +  QString::number(numAmosRunning).toStdString()*/ << endl;
    }
    else if(ok)
        fullyProcessedSubjects.append(subjects.at(numCase));    
    
    if(casesFinished == numValidCases/* || !numAmosRunning*/) {
        cout << "externProcess.size " + QString::number(avaliableExternalProcesses.size()).toStdString() << endl;

        for(int i = 0; i < avaliableExternalProcesses.size(); i++) {
            avaliableExternalProcesses.at(i)->deleteLater();
            /*	    amosThreads.removeFirst();*/	    
        }        
        
        auto end = chrono::high_resolution_clock::now();
        int mSecs = chrono::duration_cast<chrono::nanoseconds>(end - begin).count()/1000000.0;
        // emit sendLog("Total time " + QString::number(mSecs) + "ms."); 
        emit sendTime(mSecs);
        
        // 	if(casesStopped == amosThreads.size())
        // emit sendLog(QString("Threads stopped"));
        //     cout << "Total time " << chrono::duration_cast<chrono::nanoseconds>(end - begin).count()/1000000.0 << "ms" << endl;        
        emit processedSubject(fullyProcessedSubjects);
        emit processFinished();
        stop = false;	
    }
    else if(casesStarted < numValidCases) {
        if(externalProcess != 0) {
            avaliableExternalProcesses.append(externalProcess);
//             externalProcess->setDelay(timeCounter);
        }
//         timeCounter = delay;
        emit sendStartCase();
        cout << "caseFinished " << casesStarted << " " << numValidCases << endl;
    }
 
}

void ExternalProcessLauncher::stopProcess()
{
    numValidCases = casesStarted;
    stop = true;
    emit processedSubject(fullyProcessedSubjects);
//     emit processFinished();
    if(externalProcesses.size() == 0)
        emit processFinished();    
    
    //     if(amos)
    for(int i = 0; i < externalProcesses.size(); i++) {
        externalProcesses.at(i)->stopProcess();
        externalProcesses.at(i)->kill();
        // 	amosThreads.at(i)->quit();
        externalProcesses.at(i)->deleteLater();
    }    
}

uint ExternalProcessLauncher::getDelay()
{
    return delay;
}

void ExternalProcessLauncher::setDelay(uint delay)
{
    this->delay = delay;
}

void ExternalProcessLauncher::startChronometer(quint32 time)
{
    chronoTime = time;
}

void ExternalProcessLauncher::addTimeToChrono(uint intervalTime)
{
    int second = intervalTime/1000;
    chronoTime += second;
    emit sendChronoInterval(second);
    if(lastDelay > 0)
        lastDelay -= second;  
}

bool ExternalProcessLauncher::setParameters(QMap< QString, QString > parameters)
{
    this->parameters = parameters;
    return true;
}
