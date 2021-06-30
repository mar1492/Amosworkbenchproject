#ifndef EXTERNALPROCESSWRAPPER_H
#define EXTERNALPROCESSWRAPPER_H

#include <QProcess>
#include <QFileInfo>
#include <QDir>

class ExternalProcessWrapper : public QProcess
{
    Q_OBJECT
    
public:
    ExternalProcessWrapper(QObject* parent = Q_NULLPTR);
    virtual ~ExternalProcessWrapper() Q_DECL_OVERRIDE;
    
    virtual bool setParameters(QMap<QString, QString> parameters) = 0;
    void setProgramPath(QString programPath);
    void setDelay(uint delay);
    uint getDelay();
    void startChronometer(quint32 time = 0);    
    
    void launchStage();
    void stopProcess();
    
    void setImages(int numCase, QFileInfoList imageFiles, QString subjectName); 
    virtual void setOutputPaths(QDir outputDirectory, QStringList outputPaths) = 0;

    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);
    void setNumThreads(int numThreads);
    
public slots:
    void addTimeToChrono(uint intervalTime);
    
protected slots:
    void printOutput();
    void printError();
    void finished(int exitCode, QProcess::ExitStatus exitStatus); 
    
protected:
    virtual QStringList getArguments() = 0;
    virtual void cleanAfterFinished() = 0;

signals:
    void sendExecutionFinished(ExternalProcessWrapper*, int, bool);
    void sendLog(QString);
    void sendProgressRange(int );
    void sendProgressType(QString );
    void sendProgress(int );
    void sendCaseProgress(int);
    void sendExecutionStarted(ExternalProcessWrapper*, QString);
    
protected:
    int numCase;
    QString subjectName;
    uint delay;
    quint32 chronoTime;
    bool launched;
    bool stopped;
    bool subjectsAsMainDirs;
    QFileInfoList imageFiles;
    int amosProcessRange;
    int countProcess;
    QString m_program;
    QDir outputDirectory;
    int numThreads;
    
};

#endif // EXTERNALPROCESSWRAPPER_H
