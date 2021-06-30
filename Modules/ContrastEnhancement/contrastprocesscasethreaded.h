#ifndef CONTRASTPROCESSCASETHREADED_H
#define CONTRASTPROCESSCASETHREADED_H

#include "contrastprocesscase.h"

/**
 * @todo write docs
 */
class ContrastProcessCaseThreaded : public ContrastProcessCase
{
    
    Q_OBJECT
    
public:
    /**
     * Constructor
     *
     * @param modeAutomatic TODO
     * @param imagesPreviouslyLoaded TODO
     * @param mode3D TODO
     */
    ContrastProcessCaseThreaded(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D);
    /**
     * Destructor
     */
    ~ContrastProcessCaseThreaded();
    
protected:
    /**
     * @todo write docs
     *
     * @return TODO
     */
    void run() override;

    /**
     * @todo write docs
     *
     * @param numCase TODO
     * @param imagesMITK TODO
     * @param subjectName TODO
     * @param SLICE_INI_LOOP TODO
     * @param SLICE_END_LOOP TODO
     * @return TODO
     */
    bool setImages(int numCase, QList< mitk::Image::Pointer > imagesMITK, QString caseName, int SLICE_INI_LOOP, int SLICE_END_LOOP) override;

};

#endif // CONTRASTPROCESSCASETHREADED_H
