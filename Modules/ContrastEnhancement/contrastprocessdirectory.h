#ifndef CONTRASTPROCESSDIRECTORY_H
#define CONTRASTPROCESSDIRECTORY_H

#include "internalprocessdirectory.h"
#include "ContrastEnhancementExports.h"
/**
 * @todo write docs
 */
class ContrastEnhancement_EXPORT ContrastProcessDirectory : public InternalProcessDirectory
{
    Q_OBJECT
public:
    /**
     * @todo write docs
     */
    ContrastProcessDirectory();

    /**
     * @todo write docs
     */
    ~ContrastProcessDirectory();

    void setExternalFiles(QStringList /*files*/) Q_DECL_OVERRIDE;

protected:
    /**
     * @todo write docs
     *
     * @param threadsPerProccess TODO
     * @return TODO
     */
    InternalProcessCase* createProcess(int threadsPerProccess) Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @param amos TODO
     * @return TODO
     */
    void loadExternalFiles(InternalProcessCase* /*amos*/) Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @param rightDir TODO
     * @param numCase TODO
     * @return TODO
     */
    bool processNewSubDirs(bool rightDir, int numCase)  Q_DECL_OVERRIDE;

    /**
     * @todo write docs
     *
     * @return TODO
     */
    bool processSubDirs() Q_DECL_OVERRIDE;

};

#endif // CONTRASTPROCESSDIRECTORY_H
