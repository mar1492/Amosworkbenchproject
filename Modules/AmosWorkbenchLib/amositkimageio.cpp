#include "amositkimageio.h"
#include "utils.hpp"
#include "amosniftiimageio.h" // this include after #include "utils.hpp" in order to avoid conflict between armadillo and nifti_io
// #include "utils.hpp"
#include <mitkIOMimeTypes.h>

QList< QPair<mitk::Image::Pointer, DataOrientation::OrientationData > > AmosItkImageIO::imagesOrientationData;

AmosItkImageIO::AmosItkImageIO(itk::ImageIOBase::Pointer imageIO) : mitk::ItkImageIO(imageIO)
{
    
}

AmosItkImageIO::AmosItkImageIO(const mitk::CustomMimeType& mimeType, itk::ImageIOBase::Pointer imageIO, int rank) : mitk::ItkImageIO(mimeType, imageIO, rank)
{
    
}

AmosItkImageIO::~AmosItkImageIO()
{
    
}

std::vector<itk::SmartPointer<mitk::BaseData> > AmosItkImageIO::Read()
{
    cout << "AmosItkImageIO::Read()" << endl;
    std::vector<itk::SmartPointer<mitk::BaseData> > result = ItkImageIO::Read();
    mitk::BaseData::Pointer baseData= result[0];
    mitk::Image::Pointer image = dynamic_cast<mitk::Image*> (baseData.GetPointer());
    if(!image)
        return result;
    
    DataOrientation::OrientationData oData;
    mitk::BaseGeometry *geometry = image->GetGeometry();
    Utils::changeGeometry(geometry, &oData.icod, &oData.jcod, &oData.kcod, &oData.reorderI, &oData.reorderJ, &oData.reorderK); 
    
    Image::Pointer amosImage = Image::New();
    
    mitk::PixelType chPixelType = image->GetPixelType(0);
    if(chPixelType.GetNumberOfComponents() > 1) {
        cout << "AmosItkImageIO::Read() image has to contain only one component" << endl;
    }
    int pixelType = chPixelType.GetComponentType();
    string type = chPixelType.GetComponentTypeAsString();
    switch(pixelType) {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE : 
            break;
        case itk::ImageIOBase::UCHAR :
            Utils::transposeReorderImage<uchar>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase::CHAR : 
            Utils::transposeReorderImage<char>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase::USHORT : 
            Utils::transposeReorderImage<ushort>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase::SHORT : 
            Utils::transposeReorderImage<short>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase::UINT : 
            Utils::transposeReorderImage<uint>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase::INT : 
            Utils::transposeReorderImage<int>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase::ULONG : 
            Utils::transposeReorderImage<ulong>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase::LONG : 
            Utils::transposeReorderImage<long>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase::FLOAT : 
            Utils::transposeReorderImage<float>(image, amosImage, oData);		
            break;
        case itk::ImageIOBase:: DOUBLE : 
            Utils::transposeReorderImage<double>(image, amosImage, oData);		
            break;		
    }  
    
    mitk::PropertyList * propertyList = image->GetPropertyList();
    amosImage->SetPropertyList(propertyList);
    
    imagesOrientationData.append(QPair<mitk::Image::Pointer, DataOrientation::OrientationData>(amosImage, oData));
    result[0] = amosImage;
    image = NULL;
    
    return result;
}

void AmosItkImageIO::Write()
{
    cout << "AmosItkImageIO::Write()" << endl;
    bool revertNeeded = false;
    DataOrientation::OrientationData oData;
    const mitk::BaseData* dataBase = this->GetInput();
    mitk::Image::Pointer amosImage = (dynamic_cast<const mitk::Image*>(dataBase))->Clone();
    
    for(int i = 0; i < imagesOrientationData.size(); i++) {
        QPair<mitk::Image::Pointer, DataOrientation::OrientationData> data = imagesOrientationData.at(i);
        if(data.first.GetPointer() == dataBase) {
            oData = data.second;
            revertNeeded = true;
            break;
        }
    }
    
    if(!revertNeeded) {
        ItkImageIO::Write();
        return;
    }
    
    cout << "Reverting" << endl;
    Image::Pointer outImage = Image::New();   
    
    mitk::PixelType chPixelType = amosImage->GetPixelType(0);
    int pixelType = chPixelType.GetComponentType();
    string type = chPixelType.GetComponentTypeAsString();
    switch(pixelType) {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE : 
            break;
        case itk::ImageIOBase::UCHAR :
            Utils::revertImageOrientation<uchar>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase::CHAR : 
            Utils::revertImageOrientation<char>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase::USHORT : 
            Utils::revertImageOrientation<ushort>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase::SHORT : 
            Utils::revertImageOrientation<short>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase::UINT : 
            Utils::revertImageOrientation<uint>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase::INT : 
            Utils::revertImageOrientation<int>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase::ULONG : 
            Utils::revertImageOrientation<ulong>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase::LONG : 
            Utils::revertImageOrientation<long>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase::FLOAT : 
            Utils::revertImageOrientation<float>(amosImage, outImage, oData);		
            break;
        case itk::ImageIOBase:: DOUBLE : 
            Utils::revertImageOrientation<double>(amosImage, outImage, oData);		
            break;		
    } 
    
    if(amosImage->GetProperty("descrip")) {
        outImage->SetProperty("descrip", StringProperty::New(amosImage->GetProperty("descrip")->GetValueAsString())); 
    }
    
    this->mitk::AbstractFileWriter::SetInput(outImage.GetPointer());
    ItkImageIO::Write();
}

AmosItkImageIO::AmosItkImageIO(const AmosItkImageIO& other): ItkImageIO(other)
{
}

AmosItkImageIO* AmosItkImageIO::IOClone() const
{
    return new AmosItkImageIO(*this);
}

void AmosItkImageIO::addImageToList(mitk::Image::Pointer image, DataOrientation::OrientationData oData)
{
    imagesOrientationData.append(QPair<mitk::Image::Pointer, DataOrientation::OrientationData>(image, oData));
}

void AmosItkImageIO::removeImageFromList(const mitk::DataNode* node)
{
    for(int i = 0; i < imagesOrientationData.size(); i++) {
        QPair<mitk::Image::Pointer, DataOrientation::OrientationData> data = imagesOrientationData.at(i);
        if(data.first.GetPointer() == dynamic_cast<mitk::Image*>(node->GetData())) {
            imagesOrientationData.removeAt(i);
            break;
        }
    }
}

mitk::Image* AmosItkImageIO::LoadAmosNifti(const std::string& location)
{
    AmosNiftiImageIO::Pointer amosNiftiIO = AmosNiftiImageIO::New();
    AmosItkImageIO* niftiIO = new AmosItkImageIO(mitk::IOMimeTypes::NIFTI_MIMETYPE(),amosNiftiIO.GetPointer(), 0);
    niftiIO->mitk::AbstractFileReader::SetInput(location);
    std::vector<itk::SmartPointer<mitk::BaseData> > result = niftiIO->Read();
    mitk::BaseData::Pointer baseData= result[0];
    mitk::Image* image = dynamic_cast<mitk::Image*> (baseData.GetPointer());
    delete niftiIO;
    return image;
}

void AmosItkImageIO::SaveAmosNifti(mitk::Image* image, const std::string& location)
{
    AmosNiftiImageIO::Pointer amosNiftiIO = AmosNiftiImageIO::New();
    AmosItkImageIO* niftiIO = new AmosItkImageIO(mitk::IOMimeTypes::NIFTI_MIMETYPE(),amosNiftiIO.GetPointer(), 0);
    niftiIO->SetOutputLocation(location);
    niftiIO->mitk::AbstractFileWriter::SetInput(image);
    niftiIO->Write();
    delete niftiIO;
}
