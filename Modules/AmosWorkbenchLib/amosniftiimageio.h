#ifndef AMOSNIFTIIMAGEIO_H
#define AMOSNIFTIIMAGEIO_H

#include <itkNiftiImageIO.h>

// namespace us {
// class ModuleResource;
// }
// 

class AmosNiftiImageIO : public itk::NiftiImageIO
{
public:

    /** Standard class typedefs. */
    typedef AmosNiftiImageIO         Self;
    typedef itk::NiftiImageIO         Superclass;
    typedef itk::SmartPointer< Self > Pointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    /** Run-time type information (and related methods). */
    itkTypeMacro(AmosNiftiImageIO, Superclass)

    bool SupportsDimension(unsigned long dim) override;
    
/*protected:
    AmosNiftiImageIO() {};
    virtual ~AmosNiftiImageIO() {}; */   
};

#endif // AMOSNIFTIIMAGEIO_H
