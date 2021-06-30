#ifndef WMHSTATISTICS_H
#define WMHSTATISTICS_H

#include <QThread>
#include <QMutex>

class WMHStatistics : public QThread
{
    Q_OBJECT
    
public:
    WMHStatistics();
    ~ WMHStatistics();
    
    void initHeap();
    virtual void setNumThreads(int numThreads);
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);
    
    virtual bool setImages(int numCase, QList<Image::Pointer> imagesMITK, QString subjectName = "", int SLICE_INI_LOOP = -1, int SLICE_END_LOOP = -1);
    
    bool setOutputDirectories(QString parentDirOut, QString pathOut);

    virtual void stopProcess();
    
protected slots:
    void executionFinished();
    virtual void execute();
    virtual void saveResults();
//     virtual void nonLoopExecute();
    void changeProgres();
    
signals:
    void sendLog(QString );
    void sendProgressRange(int );
    void sendProgress(int );
    void sendProgressType(QString );
    void sendExecutionFinished(WMHStatistics*, int );
//     void sendSaveImage(Image::Pointer , QFileInfo );
//     void sendDifferentQforms(QString);
    void sendCaseProgress(int);    
    
};

#endif // WMHSTATISTICS_H
