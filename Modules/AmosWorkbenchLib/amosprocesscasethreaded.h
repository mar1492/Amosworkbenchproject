#ifndef AMOSPROCESSCASETHREADED_H
#define AMOSPROCESSCASETHREADED_H

#include "amosprocesscase.h"

class /*AmosWorkbenchLib_EXPORT*/ AmosProcessCaseThreaded : public AmosProcessCase
{
    
    Q_OBJECT
    
public:
    AmosProcessCaseThreaded(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D);
    ~AmosProcessCaseThreaded();
    
    bool setImages(int numCase, QList<Image::Pointer> imagesMITK, QString caseName = "", int SLICE_INI_LOOP = -1, int SLICE_END_LOOP = -1) Q_DECL_OVERRIDE;    
    
protected:
    void run() Q_DECL_OVERRIDE;
    
    
};

#endif // AMOSPROCESSCASETHREADED_H
