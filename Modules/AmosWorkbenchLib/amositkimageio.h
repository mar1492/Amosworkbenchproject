#ifndef AMOSITKIMAGEIO_H
#define AMOSITKIMAGEIO_H

#include <mitkItkImageIO.h>
#include <mitkImage.h>
#include <QList>
#include <QPair>
#include "dataorientation.h"
#include <AmosWorkbenchLibExports.h>

class AmosWorkbenchLib_EXPORT AmosItkImageIO : public mitk::ItkImageIO
{
public:
    AmosItkImageIO(itk::ImageIOBase::Pointer imageIO);
    AmosItkImageIO(const mitk::CustomMimeType& mimeType, itk::ImageIOBase::Pointer imageIO, int rank);
    ~AmosItkImageIO();
    
    using AbstractFileReader::Read;
    std::vector<itk::SmartPointer<mitk::BaseData> > Read() override;
    void Write() override;
    
    static void addImageToList(mitk::Image::Pointer image, DataOrientation::OrientationData oData);
    static void removeImageFromList(const mitk::DataNode* node);
    static mitk::Image* LoadAmosNifti(const std::string& location);
    static void SaveAmosNifti(mitk::Image* image, const std::string& location);
    
protected:
    AmosItkImageIO(const AmosItkImageIO& other);
    
private:
    AmosItkImageIO* IOClone() const override; 
    
protected:    
    
    static QList< QPair<mitk::Image::Pointer, DataOrientation::OrientationData > > imagesOrientationData;
};

#endif // AMOSITKIMAGEIO_H
