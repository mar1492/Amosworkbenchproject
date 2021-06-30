#include "imagefilters.hpp"

LogSender *ImageFilters::logSender = 0;

float ImageFilters::BinarizeMatrixFilter2D(arma::Mat<float>* inputMatrix, arma::Mat<unsigned char>* outputMatrix, itk::ThreadProcessIdType numOfThreads)
{
    auto begin = chrono::high_resolution_clock::now();
    Utils::ITKFloatImageType2D::Pointer inputImageITK = Utils::armaToITKImage<float, 2>(inputMatrix->memptr(), Utils::armaSize<float>(inputMatrix));

    FilterBinary2D::Pointer filterBin = FilterBinary2D::New();
    filterBin->SetInsideValue(0);
    filterBin->SetOutsideValue(1);
    filterBin->SetNumberOfThreads(numOfThreads);
    filterBin->SetInput(inputImageITK);  
    filterBin->Update();

    const itk::Size<2> size = inputImageITK->GetLargestPossibleRegion().GetSize(); 
    *outputMatrix = arma::Mat<unsigned char>(filterBin->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);
			
    auto end = chrono::high_resolution_clock::now();
    if(Utils::showTime && logSender)
	logSender->logText("BinarizeMatrixFilter2D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
// 	cout << "BinarizeMatrixFilter2D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl; 
    return filterBin->GetThreshold();
}

float ImageFilters::BinarizeMatrixFilter3D(arma::Cube<float>* inputCube, arma::Cube<unsigned char>* outputCube, itk::ThreadProcessIdType numOfThreads)
{
    auto begin = chrono::high_resolution_clock::now();
    Utils::ITKFloatImageType3D::Pointer inputImageITK = Utils::armaToITKImage<float, 3>(inputCube->memptr(), Utils::armaSize<float>(inputCube));
    FilterBinary3D::Pointer filterBin = FilterBinary3D::New();
    filterBin->SetInsideValue(0);
    filterBin->SetOutsideValue(1);
    filterBin->SetNumberOfThreads(numOfThreads);
    filterBin->SetInput(inputImageITK);  
    filterBin->Update();

    const itk::Size<3> size = inputImageITK->GetLargestPossibleRegion().GetSize(); 
    *outputCube = arma::Cube<unsigned char>(filterBin->GetOutput()->GetBufferPointer(), size[0], size[1], size[2], true, false);
	
    auto end = chrono::high_resolution_clock::now();
    if(Utils::showTime && logSender)
	logSender->logText("BinarizeMatrixFilter3D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
// 	cout << "BinarizeMatrixFilter3D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl; 
    return filterBin->GetThreshold();
}

// void ImageFilters::DilateDiskMatrixFilter2D(Mat<unsigned char>* inputMatrix, Mat<unsigned char>* outputMatrix, unsigned int radius)
// {
//     auto begin = chrono::high_resolution_clock::now();
//     Utils::ITKBinaryImageType2D::Pointer inputImageITK = Utils::armaToITKImage<unsigned char, 2>(inputMatrix->memptr(), Utils::armaSize<unsigned char>(inputMatrix));
//     
//     StructuringElementType structuringElement;
//     structuringElement.SetRadius(radius);
//     structuringElement.CreateStructuringElement();
//     
//     BinaryDilateImageFilterType2D::Pointer dilateFilter = BinaryDilateImageFilterType2D::New();
//     dilateFilter->SetKernel(structuringElement);
//     dilateFilter->SetInput(inputImageITK);
//     dilateFilter->Update();
//     const itk::Size<2> size = inputImageITK->GetLargestPossibleRegion().GetSize();
//     *outputMatrix = Mat<unsigned char>(dilateFilter->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);    
//     
//     auto end = chrono::high_resolution_clock::now();
//     cout << "DilateDiskMatrixFilter2D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;    
// }

void ImageFilters::DilateDiskMatrixFilter2D(arma::Mat<unsigned char>* inputMatrix, arma::Mat<unsigned char>* outputMatrix, unsigned int radius, bool radiusIsParametric)
{
    auto begin = chrono::high_resolution_clock::now();
    Utils::ITKBinaryImageType2D::Pointer inputImageITK = Utils::armaToITKImage<unsigned char, 2>(inputMatrix->memptr(), Utils::armaSize<unsigned char>(inputMatrix));
        
    SE2Type::RadiusType r2;
    r2.Fill( radius );
    SE2Type k2;
    k2 = SE2Type::Ball(r2, radiusIsParametric);
    
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();

    BinaryDilateImageFilterType2D::Pointer dilateFilter = BinaryDilateImageFilterType2D::New();
    
    dilateFilter->SetDilateValue(1);
    dilateFilter->SetInput(inputImageITK);
    dilateFilter->SetKernel(k2);    
    dilateFilter->Update();
    
    const itk::Size<2> size = inputImageITK->GetLargestPossibleRegion().GetSize();
    *outputMatrix = arma::Mat<unsigned char>(dilateFilter->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);    
    
    auto end = chrono::high_resolution_clock::now();
    if(Utils::showTime && logSender)
	logSender->logText("DilateDiskMatrixFilter2D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
// 	cout << "DilateDiskMatrixFilter2D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;    
}

void ImageFilters::ErodeDiskMatrixFilter2D(arma::Mat<unsigned char>* inputMatrix, arma::Mat<unsigned char>* outputMatrix, unsigned int radius, bool radiusIsParametric)
{
    auto begin = chrono::high_resolution_clock::now();
    Utils::ITKBinaryImageType2D::Pointer inputImageITK = Utils::armaToITKImage<unsigned char, 2>(inputMatrix->memptr(), Utils::armaSize<unsigned char>(inputMatrix));
        
    SE2Type::RadiusType r2;
    r2.Fill( radius );
    SE2Type k2;
    k2 = SE2Type::Ball(r2, radiusIsParametric);
    
    StructuringElementType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();

    BinaryErodeImageFilterType2D::Pointer erodeFilter = BinaryErodeImageFilterType2D::New();
    
    erodeFilter->SetErodeValue(1);
    erodeFilter->SetInput(inputImageITK);
    erodeFilter->SetKernel(k2);    
    erodeFilter->Update();
    
    const itk::Size<2> size = inputImageITK->GetLargestPossibleRegion().GetSize();
    *outputMatrix = arma::Mat<unsigned char>(erodeFilter->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);    
    
    auto end = chrono::high_resolution_clock::now();
    if(Utils::showTime && logSender)
	logSender->logText("ErodeDiskMatrixFilter2D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
// 	cout << "ErodeDiskMatrixFilter2D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;    
}

void ImageFilters::BinaryContourMatrixFilter2D(arma::Mat<unsigned char>* inputMatrix, arma::Mat<unsigned char>* outputMatrix)
{
    auto begin = chrono::high_resolution_clock::now();
    Utils::ITKBinaryImageType2D::Pointer inputImageITK = Utils::armaToITKImage<unsigned char, 2>(inputMatrix->memptr(), Utils::armaSize<unsigned char>(inputMatrix));

    BinaryContourImageFilterType2D::Pointer binaryContourFilter = BinaryContourImageFilterType2D::New();
    binaryContourFilter->SetForegroundValue(1);
    binaryContourFilter->SetBackgroundValue(0);
    binaryContourFilter->SetFullyConnected(true);
    binaryContourFilter->SetInput(inputImageITK); 
    binaryContourFilter->Update();
    const itk::Size<2> size = inputImageITK->GetLargestPossibleRegion().GetSize();
    *outputMatrix = arma::Mat<unsigned char>(binaryContourFilter->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);
        
    auto end = chrono::high_resolution_clock::now();
    if(Utils::showTime && logSender)
	logSender->logText("BinaryContourMatrixFilter2D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
// 	cout << "BinaryContourMatrixFilter2D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl; 
}

// void ImageFilters::SkeletonEndPointsMatrixFilter2D(Mat<unsigned char>* inputMatrix, Mat<unsigned char>* outputMatrix, unsigned int iterations)
// {
//     auto begin = chrono::high_resolution_clock::now();
//     Utils::ITKBinaryImageType2D::Pointer inputImageITK = Utils::armaToITKImage<unsigned char, 2>(inputMatrix->memptr(), Utils::armaSize<unsigned char>(inputMatrix));
// 
//     FilterBinaryThinningType2D::Pointer binaryThinningFilter = FilterBinaryThinningType2D::New();
//     binaryThinningFilter->SetInput(inputImageITK);
//     binaryThinningFilter->Update();
//     
//     const itk::Size<2> size = inputImageITK->GetLargestPossibleRegion().GetSize();
//     *outputMatrix = Mat<unsigned char>(binaryThinningFilter->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);
//     
// //     outputMatrix->save("WMskelAmos.mat", arma_ascii);
// 
//     FilterBinaryEndpoints2D::Pointer branches = FilterBinaryEndpoints2D::New();
// 
//     branches->SetInput(binaryThinningFilter->GetOutput());
//     branches->SetForegroundCellConnectivity(0);
//     branches->SetEndPoints(true);
//     branches->Update();
// 
//     *outputMatrix = Mat<unsigned char>(branches->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);    
//         
//     auto end = chrono::high_resolution_clock::now();
//     if(Utils::showTime && logSender)
// 	cout << "SkeletonEndPointsMatrixFilter2D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;    
// }
