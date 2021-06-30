#ifndef AMOSPROCESSDIRECTORY_H
#define AMOSPROCESSDIRECTORY_H

//#include <AmosWorkbenchLibExports.h>
#include "internalprocessdirectory.h"

using namespace mitk;

class AmosWorkbenchLib_EXPORT AmosProcessDirectory : public InternalProcessDirectory
{
    Q_OBJECT

public:
    AmosProcessDirectory();
    ~AmosProcessDirectory();

//     void setSvmFiles(QString svmModelFileName, QString svmScaleFileName);
    void setExternalFiles(QStringList files) Q_DECL_OVERRIDE;

protected:
    bool processSubDirs() Q_DECL_OVERRIDE;
    bool processNewSubDirs(bool rightDir, int numCase) Q_DECL_OVERRIDE;
    void loadExternalFiles(InternalProcessCase *amos) Q_DECL_OVERRIDE;
    InternalProcessCase* createProcess(int threadsPerProccess) Q_DECL_OVERRIDE;
    
protected:

    QString svmModelFileName;
    QString svmScaleFileName; 

    double NSIGMADESVseed;
    double NSIGMADESVseed2;
    double NSIGMADESV_TH2;
    double NSIGMADESV_TH4;
    unsigned int NBINS;
    unsigned int NPOINTSGAUSSIANAPROX;    
    int ERODEWM;
    unsigned int MINWMLSIZE;    
    unsigned int EXCLUSIONZONE;
    unsigned int MIN_R_PERIPHERY; 
    double WEIGHT_CLASSIFIER_FL;

    int numFeaturesSVM;
};

#endif // AMOSPROCESSDIRECTORY_H
