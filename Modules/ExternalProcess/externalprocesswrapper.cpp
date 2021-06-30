#include "externalprocesswrapper.h"
#include <iostream>

using namespace std;

ExternalProcessWrapper::ExternalProcessWrapper(QObject* parent) : QProcess(parent)
{
    connect (this, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
    connect (this, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
    connect (this, SIGNAL(finished(int , QProcess::ExitStatus )), this, SLOT(finished(int , QProcess::ExitStatus ))); 
    
    amosProcessRange = 0;
    countProcess = 0;
    launched = false;
    stopped = false;
}

ExternalProcessWrapper::~ExternalProcessWrapper()
{    
}

void ExternalProcessWrapper::setNumThreads(int numThreads)
{
    this->numThreads = numThreads;
}

void ExternalProcessWrapper::launchStage()
{
    
    QStringList arguments = getArguments();
    
    start(m_program, arguments);
}

void ExternalProcessWrapper::stopProcess()
{
    stopped = true;
}

void ExternalProcessWrapper::setProgramPath(QString programPath)
{
    m_program = programPath;
}

void ExternalProcessWrapper::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
}


void ExternalProcessWrapper::printOutput()
{
    QByteArray byteArray = readAllStandardOutput();
    QStringList strLines = QString(byteArray).split("\n");

    foreach (QString line, strLines){
        cout<< line.toStdString() << endl;
//         emit sendLog(line);
    }
}

void ExternalProcessWrapper::printError()
{
    QByteArray byteArray = readAllStandardOutput();
    QStringList strLines = QString(byteArray).split("\n");

    foreach (QString line, strLines){
        cout << line.toStdString() << endl;
//         emit sendLog(line);
    }
}

uint ExternalProcessWrapper::getDelay()
{
    return delay;
}

void ExternalProcessWrapper::setDelay(uint delay)
{
    this->delay = delay;
}

void ExternalProcessWrapper::startChronometer(quint32 time)
{
    chronoTime = time;
    countProcess = 1;
        
}

void ExternalProcessWrapper::addTimeToChrono(uint intervalTime)
{
    chronoTime += intervalTime;
    if(chronoTime >= delay && !launched) {
        launched = true;
        sendExecutionStarted(this, subjectName);
        launchStage();        
    }
}

void ExternalProcessWrapper::setImages(int numCase, QFileInfoList imageFiles, QString subjectName)
{
    this->numCase = numCase;
    this->imageFiles = imageFiles;
    this->subjectName = subjectName;    
}

void ExternalProcessWrapper::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    cout << "----------------------------------------------------" << endl;
    cout << subjectName.toStdString() << " " << "exitCode " << exitCode << " exitStatus " << exitStatus << endl;
    
    bool ok = exitCode == 0 && exitStatus == QProcess::NormalExit;
    
    if(countProcess < amosProcessRange && !stopped) {
//         if(numValidCases == 1)
        emit sendCaseProgress(countProcess);
        countProcess++;        
        launchStage();
    }
    else {
        cleanAfterFinished();
        
//         sendProgressType(tr("Finished"));
//         sendProgressRange(1);
//         sendProgress(1);        
//         if(exitCode == 0 && exitStatus == QProcess::NormalExit)
//             fullyProcessedSubjects.append(subjectName);
//         caseFinished(casesStarted);
//         if(numValidCases == 1)
        if(!stopped)
            emit sendCaseProgress(amosProcessRange);
        else
            ok = false;
        
        launched = false;
        emit sendExecutionFinished(this, numCase, ok);
        if(stopped)
            stopped = false;
    }    
}
