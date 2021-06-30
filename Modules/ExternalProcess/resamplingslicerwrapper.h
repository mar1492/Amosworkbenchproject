#ifndef RESAMPLINGSLICERWRAPPER_H
#define RESAMPLINGSLICERWRAPPER_H

#include "externalprocesslauncher.h"

class ResamplingSlicerWrapper : public ExternalProcessWrapper
{
    Q_OBJECT
    
public:
    
    ResamplingSlicerWrapper(QObject* parent = Q_NULLPTR);
    ~ResamplingSlicerWrapper(); 
    
    static int getAmosProcessRange();

    void setOutputPaths(QDir outputDirectory, QStringList outputPaths) Q_DECL_OVERRIDE;
    
    bool setParameters(QMap<QString, QString> parameters) Q_DECL_OVERRIDE;
    
protected:
    QStringList getArguments() Q_DECL_OVERRIDE;
    void cleanAfterFinished() Q_DECL_OVERRIDE;
    
protected:
    QString flairBCPath;
    QString T1BCPath;
    QString T1RSFLPath;
    QString LBRSFLPath;
    QString TXPath;
    bool doBiasCorrection;
    bool LBResampling;
};

#endif // RESAMPLINGSLICERWRAPPER_H
