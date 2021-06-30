#ifndef EXTERNALPROCESSLAUNCHER_H
#define EXTERNALPROCESSLAUNCHER_H

// This file is in AmosWorkbenchproject-superbuild/AmosWorkbenchproject-build/Modules/ExternalProcess
#include <ExternalProcessExports.h>

#include <QDir>
#include <QPointer>
#include <QMap>

#include <chrono>

#include "externalprocesswrapper.h"
#include "utils.hpp"

class ExternalProcess_EXPORT ExternalProcessLauncher : public QObject
{
    Q_OBJECT

public:
    explicit ExternalProcessLauncher(QObject *parent = 0);
    ~ExternalProcessLauncher();

    void setSubjects(QStringList subjects);
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs); 
    
    virtual bool setAmosImagesData(QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn, QString outputParentPath, QString outputPath, QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListOut) = 0;
    
    virtual bool setParameters(QMap<QString, QString> parameters);
    virtual void setProgram(QString programPath) = 0;
    
    void setDelay(uint delay);
    uint getDelay();
    void startChronometer(quint32 time = 0);
    
    virtual void setNumThreads(int numThreads) = 0;
    virtual void setNumSimultaneousSubjects(int numSimultaneousSubjects) = 0;    

public Q_SLOTS:
//     void setProgram(const QString& program);

    void processCases();
    void stopProcess();
    void addTimeToChrono(uint intervalTime);
    
protected slots:
    void processNewDir();
    void caseFinished(ExternalProcessWrapper* externalProcess, int numCase, bool ok);
    void caseLaunched(ExternalProcessWrapper* externalProcess, QString subjectName);
    
protected:
    void badCase(int numCase);    
    
    virtual void appendImageFilesIn(int numCase) = 0;
    virtual QStringList setOutputPaths() = 0;
    virtual ExternalProcessWrapper* newProcess() = 0;

signals:
    void processedSubject(QStringList fullyProcessedSubjects);
    void processFinished();
    void sendProgressRange(int numSlices);
    void sendProgress(int progress);
    void sendProgressType(QString progressType);
    void sendCaseName(QString subjectName);
    void sendCaseProgressRange(int numCases);
    void sendCaseProgress(int caseNum); 
    void sendStartCase();
    void sendTime(int mSecs);
    void sendSegmentationResults(QList< QPair<Image::Pointer, QString> > );
    void sendMessageToUser(QString);
    void sendLog(QString log);
    void sendChronoInterval(uint);    

protected:
    QString m_program;
    QStringList m_arguments;
    int numImageFilesIn;
    int numImageFilesOut;

    QStringList subjects;
    bool subjectsAsMainDirs; 
    QDir OutParentDirectory;
    QDir outputDirectory;
    QString outputPath;

    QFileInfoList imageFiles;        
    
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    std::chrono::time_point<std::chrono::high_resolution_clock> beginAmos;
    int casesStarted;
    int casesLaunched;
    uint delay;
    quint32 chronoTime;
    uint lastDelay;
    int casesFinished;
    int casesStopped;    
    int numValidCases;
    int amosProcessRange;     
    bool stop;
    QString subjectName;
    
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn;
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListOut;
 
    QStringList fullyProcessedSubjects;
    QList< QPointer<ExternalProcessWrapper> > externalProcesses;
    QList< QPointer<ExternalProcessWrapper> > avaliableExternalProcesses;    

    int numThreads;
    int numSimultaneousSubjects;
    bool useParallel; 
    
    QMap<QString, QString> parameters;
};

#endif // EXTERNALPROCESSLAUNCHER_H
