#ifndef LABELLINGFREESURFER_H
#define LABELLINGFREESURFER_H

#include "externalprocesslauncher.h"

class ExternalProcess_EXPORT LabellingFreeSurferLauncher : public ExternalProcessLauncher
{
    Q_OBJECT
public: 
    
    explicit LabellingFreeSurferLauncher(QObject *parent = nullptr);
    ~LabellingFreeSurferLauncher();
    
    bool setAmosImagesData(QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn, QString outputParentPath, QString outputPath, QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListOut);
    
    void setProgram(QString /*programPath*/);
    bool setParameters(QMap<QString, QString> parameters);
    
    void setNumThreads(int numThreads);
    void setNumSimultaneousSubjects(int numSimultaneousSubjects);    
    
protected:
    void appendImageFilesIn(int numCase);
    QStringList setOutputPaths();
    void setNumThreads();
    ExternalProcessWrapper* newProcess();
    
protected:
    QDir T1Directory; 
    QFileInfoList subdirsT1;
    QString LBOrigPath;
    QString FSOutputPath;
    QString freesurferPath;
};

#endif // LABELLINGFREESURFER_H
