#ifndef RESAMPLINGSLICER_H
#define RESAMPLINGSLICER_H

#include "externalprocesslauncher.h"

class ExternalProcess_EXPORT ResamplingSlicerLauncher : public ExternalProcessLauncher
{
    Q_OBJECT
public: 
    
    explicit ResamplingSlicerLauncher(QObject *parent = 0);
    ~ResamplingSlicerLauncher();
    
    bool setAmosImagesData(QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn, QString outputParentPath, QString outputPath, QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListOut);
    
    void setProgram(QString programPath);
    bool setParameters(QMap<QString, QString> parameters);
    
    void setNumThreads(int numThreads);
    void setNumSimultaneousSubjects(int /*numSimultaneousSubjects*/);
    
protected:
    void appendImageFilesIn(int numCase);
    QStringList setOutputPaths();
    ExternalProcessWrapper* newProcess();
    
protected:
    QDir FlairDirectory;
    QDir T1Directory;
    QDir LBDirectory;   

    QFileInfoList subdirsFL;
    QFileInfoList subdirsT1;
    QFileInfoList subdirsLB;

    QString flairBCPath;
    QString T1BCPath;
    QString T1RSFLPath;
    QString LBRSFLPath;
    QString TXPath;   
    
    bool doBiasCorrection;
    bool LBResampling;
};

#endif // RESAMPLINGSLICER_H
