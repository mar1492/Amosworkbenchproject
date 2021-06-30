#ifndef LABELLINGFREESURFERWRAPPER_H
#define LABELLINGFREESURFERWRAPPER_H

#include "externalprocesslauncher.h"

class LabellingFreeSurferWrapper : public ExternalProcessWrapper
{
    Q_OBJECT
    
public:
    
    LabellingFreeSurferWrapper(QObject* parent = Q_NULLPTR);
    ~LabellingFreeSurferWrapper(); 
    
    static int getAmosProcessRange();
    
    void setOutputPaths(QDir outputDirectory, QStringList outputPaths) Q_DECL_OVERRIDE;
    
    bool setParameters(QMap< QString, QString > parameters) Q_DECL_OVERRIDE;
    
protected:
    QStringList getArguments() Q_DECL_OVERRIDE; 
    void cleanAfterFinished() Q_DECL_OVERRIDE;
    bool copyToOutput();
    void setAparc_AsegPath();
    
protected:
    QString FSOutputPath;
    QString LBOrigPath;
    QString aparc_AsegPath;
    QString LBFSPath;
    bool onlyConvert;
};

#endif // LABELLINGFREESURFERWRAPPER_H
