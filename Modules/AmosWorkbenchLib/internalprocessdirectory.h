#ifndef INTERNALPROCESSDIRECTORY_H
#define INTERNALPROCESSDIRECTORY_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QMutex>
#include <QPointer>
#include <QPair>
#include <QMetaType>
#include <QMap>

// #include <QmitkRegisterClasses.h>

#include <mitkImageReadAccessor.h>
#include <mitkIOUtil.h>

#include <chrono>

//#include <AmosWorkbenchLibExports.h>

// #include "amosprocesscase.h"
#include "internalprocesscase.h"
#include "utils.hpp"

using namespace mitk;


class AmosWorkbenchLib_EXPORT InternalProcessDirectory : public QObject
{
    Q_OBJECT

public:
    InternalProcessDirectory();
    ~InternalProcessDirectory();
    
    
    enum DirTypes {SCANSDIR, MASKDIR, EXPERTDIR, OUTPUTDIR};
    
    void setImagesPreviouslyLoaded(bool imagesPreviouslyLoaded);
    void setSubjects(QStringList subjects);
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);

    bool setAmosImagesData(QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn, QString outputParentPath, QString outputPath, QStringList amosImagesListOut);    
    void setOutNames(QStringList amosImagesListOut);
    bool setSubjectName(QString subjectName);
    void setModeAutomatic(bool modeAutomatic);
    void setMode3D(bool mode3D);
 
    virtual bool setParameters(QMap<QString, QString> parameters);

    void setNumThreads(int numThreads);
    void setNumSimultaneousSubjects(int numSimultaneousSubjects);
    void setImagesList(QList<Image::Pointer> imagesMITK);
    
    virtual void setExternalFiles(QStringList files) = 0;
    int getNumMITKOutputImages();
    void increaseNumMITKOutputImages();
    
    
    void setNewMode(bool mode);

public slots:    
    void processCases(); 
    void stopProcess();
    void caseFinished(InternalProcessCase* thread, int numCase, bool ok);
    void imageToSave(Image::Pointer image3D, QFileInfo filePath);
    void onDifferentQforms(QString subjectName);
    void onNotEnoughWM(QString subjectName);

signals:
    void sendLog(QString log);
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
    
protected:
    bool loadImages();
    void clearImages();
    void badCase(int numCase);
    virtual bool processSubDirs() = 0;
    virtual bool processNewSubDirs(bool rightDir, int numCase) = 0;
    virtual void loadExternalFiles(InternalProcessCase *amos) = 0;
    virtual InternalProcessCase* createProcess(int threadsPerProccess) = 0;
    
protected slots:
    void processNewDir();
    
protected:
    
    QDir FlairDirectory;
    QDir T1Directory;
    QDir LBDirectory;
    QDir MaskDirectory;
    QDir ExpertDirectory;

    QFileInfoList subdirsFlair;
    QFileInfoList subdirsT1;
    QFileInfoList subdirsLB;
    QFileInfoList subdirsMask;
    QFileInfoList subdirsExpert;    
    
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    std::chrono::time_point<std::chrono::high_resolution_clock> beginAmos;
    int casesStarted;
    int casesFinished;
    int casesStopped;
    int imagesSaved;
    int numValidCases; 
    
    QStringList subjects;
    QDir OutParentDirectory;
    QString outputPath;
    QFileInfoList imageFiles;
    bool subjectsAsMainDirs;
    bool hasMask;
    QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn;
    QStringList amosImagesListOut;
    QList<Image::Pointer> imagesMITK;
    QList< QPointer<InternalProcessCase> > amosThreads;
    QList< QPointer<InternalProcessCase> > avaliableAmosThreads;    
    
    QMutex mutex;

    QString subjectName;
    
    bool modeAutomatic;
    bool mode3D;
    int numThreads;
    int numSimultaneousSubjects;
    bool useParallel;
    int numImagesAmos;
    int numCaseProgress;
    bool stop;
    
    QMap<QString, QString> parameters;

    bool imagesPreviouslyLoaded;
    QList< QPair<Image::Pointer, QString> > segmentationResults;
    
    int amosProcessRange;
    
    QStringList fullyProcessedSubjects;
    int numMITKOutputImages;
};

Q_DECLARE_METATYPE(Image::Pointer)

#endif // INTERNALPROCESSDIRECTORY_H
