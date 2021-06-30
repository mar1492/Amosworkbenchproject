#include <usGetModuleContext.h>
#include <usModuleActivator.h>
#include <usModuleContext.h>
#include <usModule.h>
#include <mitkIOMimeTypes.h>
#include "amosniftiimageio.h"
// #include <itkGDCMImageIO.h>

#include "amositkimageio.h"

// namespace mitk
// {
//     class AmosNiftiImageIO : public itk::NiftiImageIO
//     {
// 	public:
// 
// 	/** Standard class typedefs. */
// 	typedef AmosNiftiImageIO         Self;
// 	typedef itk::NiftiImageIO         Superclass;
// 	typedef itk::SmartPointer< Self > Pointer;
// 
// 	/** Method for creation through the object factory. */
// 	itkNewMacro(Self)
// 
// 	/** Run-time type information (and related methods). */
// 	itkTypeMacro(AmosNiftiImageIO, Superclass)
// 
// 	bool SupportsDimension(unsigned long dim) override
// 	{
// 	    return dim > 1 && dim < 5;
// 	}
// 
//     };
    
class AmosModuleActivator : public us::ModuleActivator
{
public:

    void Load(us::ModuleContext* /*context*/) override
    {
	std::list<itk::LightObject::Pointer> allobjects =
	    itk::ObjectFactoryBase::CreateAllInstance("itkImageIOBase");
	AmosNiftiImageIO::Pointer amosNiftiIO = AmosNiftiImageIO::New();
	AmosItkImageIO* niftiIO = new AmosItkImageIO(mitk::IOMimeTypes::NIFTI_MIMETYPE(),
							amosNiftiIO.GetPointer(), 0);
	m_FileIOs.push_back(niftiIO);	    

// 	    for (auto & allobject : allobjects)
// 	    {
// 		itk::ImageIOBase* io = dynamic_cast<itk::ImageIOBase*>(allobject.GetPointer());
// 	    //       m_imageIO = mitk::ToolManagerProvider::New();
// // 		m_imageIO = new AmosItkImageIO(io);
// // 		context->RegisterService<AmosItkImageIO>(m_imageIO);
// 		
// 		// NiftiImageIO does not provide a correct "SupportsDimension()" methods
// 		// and the supported read/write extensions are not ordered correctly
// 		if (dynamic_cast<itk::NiftiImageIO*>(io)) continue;
// 
// 		// Use a custom mime-type for GDCMImageIO below
// 		if (dynamic_cast<itk::GDCMImageIO*>(allobject.GetPointer()))
// 		{
// 		// MITK provides its own DICOM reader (which internally uses GDCMImageIO).
// 		continue;
// 		}
// 
// 		if (io)
// 		{
// // 		m_FileIOs.push_back(new mitk::ItkImageIO(io));
// 		    m_FileIOs.push_back(new AmosItkImageIO(io));
// 		}
// 		else
// 		{
// 		MITK_WARN << "Error ImageIO factory did not return an ImageIOBase: "
// 			    << ( allobject )->GetNameOfClass();
// 		}		
// 	    }
	
// 	    AmosNiftiImageIO::Pointer amosNiftiIO = AmosNiftiImageIO::New();
// 	    AmosItkImageIO* niftiIO = new AmosItkImageIO(mitk::IOMimeTypes::NIFTI_MIMETYPE(),
// 							    amosNiftiIO.GetPointer(), 0);
// 	    m_FileIOs.push_back(niftiIO);
    }

    void Unload(us::ModuleContext*) override
    {
	    for(auto & elem : m_FileIOs)
	    {
		delete elem;
	    }
    }

private:
//     AmosItkImageIO::Pointer m_imageIO;
    AmosItkImageIO* m_imageIO;
    std::vector<mitk::AbstractFileIO*> m_FileIOs;
};


US_EXPORT_MODULE_ACTIVATOR(AmosModuleActivator)  



