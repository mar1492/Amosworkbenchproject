#ifndef IMAGEFILTERS_H
#define IMAGEFILTERS_H

//#include <AmosWorkbenchLibExports.h>
#include "utils.hpp"
#include "logsender.h"

#include <itkSignedMaurerDistanceMapImageFilter.h>
#include <itkApproximateSignedDistanceMapImageFilter.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>
#include <itkFastChamferDistanceImageFilter.h>
#include <itkReinitializeLevelSetImageFilter.h>

#include <itkOtsuThresholdImageFilter.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryContourImageFilter.h>
#include <itkBinaryFillholeImageFilter.h>
#include <itkBinaryImageToLabelMapFilter.h>
#include <itkLabelGeometryImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkLabelMap.h>
#include <itkLabelObject.h>
#include <itkBinaryThinningImageFilter.h>
#include <itkBinaryPruningImageFilter.h>

#include <itkExtractImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>

#include "itkSkeletonizeBaseImageFilter.h"
#include "itkSpecialSkeletonPointsImageFilter.h"

#include <itkImageFileWriter.h>

class AmosWorkbenchLib_EXPORT ImageFilters
{
public: 
    
    enum ERODE_DILATE {Erode, Dilate};
    
    template <int Dimension> struct LabelFeatures {
        long unsigned int labelValue;
        itk::SizeValueType volume;
        itk::SizeValueType surface;  //mari-new
        itk::SpacePrecisionType integratedIntensity;
        // Revisar por el cambio de coordenadas de ITK (x, y, slice) y Arma(fila, columnad, slice)
        arma::Col<itk::SpacePrecisionType>::fixed<Dimension> centroid;
        arma::Col<itk::SpacePrecisionType>::fixed<Dimension> weightedCentroid;
        arma::Col<itk::SpacePrecisionType>::fixed<Dimension> axesLength;
        itk::SpacePrecisionType majorAxisLength;
        itk::SpacePrecisionType minorAxisLength;
        itk::SpacePrecisionType	eccentricity;
        itk::SpacePrecisionType elongation;
        itk::SpacePrecisionType orientation;
        itk::SpacePrecisionType blobDeep = 0;
        
        arma::Mat<itk::IndexValueType>::fixed<2, Dimension> boundingBox;
        arma::Mat<unsigned int> pixelsIndices; // pixelsIndices[0] = col, pixelsIndices[1] = row, pixelsIndices[2] = slice in ITK
        // but row, col, slice in Armadillo, so row and col correspond to x and y in ITK
        arma::Col<arma::uword> indexOfPixels;
        
        void print(bool pixels = false) {
            cout << "\tLabel: " << labelValue << endl;
            cout << "\tVolume: " << volume << endl;
            cout << "\tIntegrated Intensity: " << integratedIntensity << endl;
            cout << "\tCentroid: " << centroid.t() << endl;
            cout << "\tWeighted Centroid: " << weightedCentroid.t() << endl;
            cout << "\tAxes Length: " << axesLength.t() << endl;
            cout << "\tMajorAxisLength: " << majorAxisLength << endl;
            cout << "\tMinorAxisLength: " << minorAxisLength << endl;
            cout << "\tEccentricity: " << eccentricity << endl;
            cout << "\tElongation: " << elongation << endl;
            cout << "\tOrientation: " << orientation << endl;
            cout << "\tBlob deep: " << blobDeep << endl;
            cout << "\tBounding box: " << "\n" << boundingBox << endl;
            if(pixels) {
                cout << "\tPixels indexes: " << "\n" << indexOfPixels;
                cout << "\tPixels: " << "\n" << pixelsIndices << endl << endl;	    
            }
        }
    };
    
    template <int Dimension> struct LabelSimpleFeatures {
        long unsigned int labelValue;
        itk::SizeValueType volume;
        itk::SizeValueType surface;
        arma::Mat<unsigned int> pixelsIndices;// pixelsIndices[0] = col, pixelsIndices[1] = row, pixelsIndices[2] = slice
        // but row, col, slice in Armadillo, so row and col correspond to x and y in ITK
        arma::Col<arma::uword> indexOfPixels;
        itk::SpacePrecisionType blobDeep = 0;
        arma::Col<itk::SpacePrecisionType>::fixed<Dimension> centroid;
        arma::Mat<itk::IndexValueType>::fixed<2, Dimension> boundingBox;
        
        void print(bool pixels = false) {
            cout << "\tLabel: " << labelValue << endl;
            cout << "\tVolume: " << volume << endl;
            cout << "\tCentroid: " << centroid.t() << endl;
            cout << "\tBlob deep: " << blobDeep << endl;
            cout << "\tBounding box: " << "\n" << boundingBox << endl;
            if(pixels) {
                cout << "\tPixels indexes: " << "\n" << indexOfPixels;
                cout << "\tPixels: " << "\n" << pixelsIndices << endl << endl;
            }
        }
    };    
    
    enum NotEmptyForeground {NOT_KNOWN, YES, NO};
    enum ResampleTypes {LINEAR, NEAREST};
    
    template <typename ArmaInputType, typename DataInputType, typename ArmaOutputType, typename DataOutputType, int Dimension> static void DistanceMapFilter(ArmaInputType* armaInput, ArmaOutputType* armaOutput, NotEmptyForeground notEmptyForeground = NOT_KNOWN, bool notSigned = true, itk::ThreadProcessIdType numOfThreads = 1, DataOutputType defaultValueForEmpty = 1000)
    {
        // 	auto begin = chrono::high_resolution_clock::now();
        
        // 	bool hasForeground = false;
        // 	if(notEmptyForeground == NOT_KNOWN)
        // 	    notEmptyForeground = Utils::isNotZero< ArmaInputType >(armaInput) ? YES : NO;
        // 	else
        // 	    hasForeground = notEmptyForeground == YES;
        notEmptyForeground = Utils::isNotZero< ArmaInputType >(armaInput) ? YES : NO;
        if(notEmptyForeground == YES) {
            typedef itk::Image< DataInputType, Dimension > ITKBinaryImageType;
            arma::uvec size = Utils::armaSize<DataInputType>(armaInput);
            typename ITKBinaryImageType::Pointer inputImageITK = Utils::armaToITKImage<DataInputType, Dimension>(armaInput->memptr(), size);
            DistanceMapFilter<ArmaOutputType, DataOutputType, Dimension, DataInputType> (inputImageITK, armaOutput, notSigned, numOfThreads);
            // 	    if(notSigned)
            // 		DistanceMapFilter<ArmaOutputType, DataOutputType, Dimension, DataInputType> (inputImageITK, armaOutput);
            // 	    else {
            // 		DistanceMapFilter<ArmaOutputType, DataOutputType, Dimension, DataInputType> (inputImageITK, armaOutput);
            // 		ArmaInputType armaInputInverse = Utils::inverseArma(*armaInput);
            // 		inputImageITK = Utils::armaToITKImage<DataInputType, Dimension>(armaInputInverse.memptr(), size);
            // 		ArmaOutputType armaOutputInverse;
            // 		armaOutputInverse.copy_size(*armaOutput);
            // 		DistanceMapFilter<ArmaOutputType, DataOutputType, Dimension, DataInputType> (inputImageITK, &armaOutputInverse);
            // 		*armaOutput = armaOutputInverse - *armaOutput;
            // 	    }
        }
        else {
            armaOutput->copy_size(*armaInput);
            armaOutput->ones();
            *armaOutput *= defaultValueForEmpty;
        }
        
        /*	auto end = chrono::high_resolution_clock::now();
         *cout << "DistanceMapFilter" << QString::fromLatin1(notSigned ? "" : "Signed") << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;*/	
    }    
    
    // Distance map for ITKIMage, bwdist MatLab translation
    template <typename ArmaType, typename DataType, int Dimension, typename DataInputType = unsigned char > static void DistanceMapFilter(typename itk::Image<DataInputType, Dimension >::Pointer inputImageITK, ArmaType* armaType, bool notSigned = true, itk::ThreadProcessIdType numOfThreads = 1)
    {
        typedef typename itk::SignedMaurerDistanceMapImageFilter< itk::Image<DataInputType, Dimension> , itk::Image<DataType, Dimension > >  FilterDistance;	
        
        typedef typename FilterDistance::Pointer FilterDistancePtr;
        
        auto begin = chrono::high_resolution_clock::now();
        
        FilterDistancePtr filterDist = FilterDistance::New();		
        filterDist->SetInput(inputImageITK);
        filterDist->SetNumberOfThreads(numOfThreads);
        
        //MatLab inverts signs
        if(!notSigned)
            filterDist->SetInsideIsPositive(true);
        
        filterDist->Update();
        
        const itk::Size<Dimension> size = inputImageITK->GetLargestPossibleRegion().GetSize();
        arma::uvec dimensions(Dimension);
        for(int i = 0; i < Dimension; i++)
            dimensions(i) = size[i];
        Utils::createArmaFromBuffer<DataType>(armaType, filterDist->GetOutput()->GetBufferPointer(), dimensions, true, false);
        if(notSigned) {
            arma::uvec ind = find(*armaType < 0);
            if(ind.n_elem > 0)
                armaType->elem(ind) = arma::zeros< arma::Col<DataType> >(ind.n_elem);
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("DistanceMapFilter" + QString::fromLatin1(notSigned ? "" : "Signed") + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        /*	    cout << "DistanceMapFilter" << QString::fromLatin1(notSigned ? "" : "Signed")  << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;*/  
        
        filterDist = NULL;
    } 
    
    
    template <typename ArmaInputType, typename DataInputType, typename ArmaOutputType, typename DataOutputType, int Dimension> static void DistanceChamferMapFilter(ArmaInputType* armaInput, ArmaOutputType* armaOutput, NotEmptyForeground notEmptyForeground = NOT_KNOWN, bool notSigned = true, DataOutputType defaultValueForEmpty = 1000, itk::ThreadProcessIdType numOfThreads = 1)
    {
        // 	auto begin = chrono::high_resolution_clock::now();
        
        // 	bool hasForeground = false;
        // 	if(notEmptyForeground == NOT_KNOWN)
        // 	    notEmptyForeground = Utils::isNotZero< ArmaInputType >(armaInput) ? YES : NO;
        // 	else
        // 	    hasForeground = notEmptyForeground == YES;
        notEmptyForeground = Utils::isNotZero< ArmaInputType >(armaInput) ? YES : NO;
        if(notEmptyForeground == YES) {
            typedef itk::Image< DataInputType, Dimension > ITKBinaryImageType;
            arma::uvec size = Utils::armaSize<DataInputType>(armaInput);
            typename ITKBinaryImageType::Pointer inputImageITK = Utils::armaToITKImage<DataInputType, Dimension>(armaInput->memptr(), size);
            DistanceChamferMapFilter<ArmaOutputType, DataOutputType, Dimension, DataInputType> (inputImageITK, armaOutput, notSigned, numOfThreads);
            
        }
        else {
            armaOutput->copy_size(*armaInput);
            armaOutput->ones();
            *armaOutput *= defaultValueForEmpty;
        }	
    } 
    
    template <typename ArmaType, typename DataType, int Dimension, typename DataInputType = unsigned char > static void DistanceChamferMapFilter(typename itk::Image<DataInputType, Dimension >::Pointer inputImageITK, ArmaType* armaType, bool notSigned = true, itk::ThreadProcessIdType numOfThreads = 1)
    {
        typedef typename itk::ApproximateSignedDistanceMapImageFilter< itk::Image<DataInputType, Dimension>, itk::Image<DataType, Dimension> >  FilterDistance;
        typedef typename FilterDistance::Pointer FilterDistancePtr;
        
        // The use of ReinitializeLevelSetImageFilter is for avoiding the problem in IsoContourDistanceImageFilter
        // (called by ApproximateSignedDistanceMapImageFilter): "Gradient norm is lower than pixel precision"
        // This problem appears in some slices, in most of these slices it is enough
        // to set LevelSetValue to 1.0, but a few of them need 2.0	
        typedef typename itk::ReinitializeLevelSetImageFilter< itk::Image<DataInputType, Dimension> > LevelSetFilter;
        typedef typename LevelSetFilter::Pointer LevelSetFilterPtr;
        
        auto begin = chrono::high_resolution_clock::now();
        
        LevelSetFilterPtr levelSetFilter = LevelSetFilter::New();
        levelSetFilter->SetInput(inputImageITK);
        levelSetFilter->NarrowBandingOff();
        levelSetFilter->SetLevelSetValue(2.0);
        levelSetFilter->SetNumberOfThreads(numOfThreads);
        levelSetFilter->Update();	
        
        FilterDistancePtr filterDist = FilterDistance::New();
        filterDist->SetInsideValue(1);
        filterDist->SetOutsideValue(0);
        filterDist->SetNumberOfThreads(numOfThreads);
        // 	filterDist->SetInput(inputImageITK);	
        filterDist->SetInput(levelSetFilter->GetOutput());	
        filterDist->Update();
        
        const itk::Size<Dimension> size = inputImageITK->GetLargestPossibleRegion().GetSize();
        arma::uvec dimensions(Dimension);
        for(int i = 0; i < Dimension; i++)
            dimensions(i) = size[i];
        Utils::createArmaFromBuffer<DataType>(armaType, filterDist->GetOutput()->GetBufferPointer(), dimensions, true, false);
        if(notSigned) {
            arma::uvec ind = find(*armaType < 0);
            if(ind.n_elem > 0)
                armaType->elem(ind) = arma::zeros< arma::Col<DataType> >(ind.n_elem);
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("DistanceChamferMapFilter" + QString::fromLatin1(notSigned ? "" : "Signed") + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");    
        /*	    cout << "DistanceChamferMapFilter" << QString::fromLatin1(notSigned ? "" : "Signed")  << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;  */
        
        levelSetFilter = NULL;
        filterDist = NULL;
    }    
    
    template <typename ArmaType, int Dimension> static void FillHolesBinaryFilter(ArmaType* inputArma, ArmaType* outputArma, bool fullConnectivity = true, itk::ThreadProcessIdType numOfThreads = 1)
    {
        
        typedef typename itk::BinaryFillholeImageFilter< itk::Image<unsigned char, Dimension> >  FilterFillholes;	
        
        auto begin = chrono::high_resolution_clock::now();
        
        typedef typename itk::Image<unsigned char, Dimension>::Pointer  InputImagePointer;
        InputImagePointer inputImageITK = Utils::armaToITKImage<unsigned char, Dimension>(inputArma->memptr(), Utils::armaSize<unsigned char>(inputArma));	
        typedef typename FilterFillholes::Pointer FilterFillholesPtr;	
        FilterFillholesPtr filterFillholes = FilterFillholes::New();
        filterFillholes->SetForegroundValue(1);
        filterFillholes->SetFullyConnected(fullConnectivity);
        filterFillholes->SetNumberOfThreads(numOfThreads);
        filterFillholes->SetInput(inputImageITK);
        
        filterFillholes->Update();
        
        const itk::Size<Dimension> size = inputImageITK->GetLargestPossibleRegion().GetSize();
        arma::uvec dimensions(Dimension);
        for(int i = 0; i < Dimension; i++)
            dimensions(i) = size[i];
        Utils::createArmaFromBuffer<unsigned char>(outputArma, filterFillholes->GetOutput()->GetBufferPointer(), dimensions, true, false);
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("DistanceChamferMapFilter" + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        
        // 	    cout << "FillHolesFilter" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
        
        filterFillholes = NULL;
    }    
    
    typedef /*typename*/ itk::OtsuThresholdImageFilter< Utils::ITKFloatImageType2D, Utils::ITKBinaryImageType2D > FilterBinary2D;    
    static float BinarizeMatrixFilter2D(arma::Mat<float>* inputMatrix, arma::Mat<unsigned char>* outputMatrix, itk::ThreadProcessIdType numOfThreads = 1);
    
    typedef /*typename*/ itk::OtsuThresholdImageFilter< Utils::ITKFloatImageType3D, Utils::ITKBinaryImageType3D > FilterBinary3D;    
    static float BinarizeMatrixFilter3D(arma::Cube<float>* inputCube, arma::Cube<unsigned char>* outputCube, itk::ThreadProcessIdType numOfThreads = 1);
    
    typedef /*typename*/ itk::FlatStructuringElement< 2 > SE2Type;
    typedef /*typename*/ itk::BinaryBallStructuringElement< Utils::ITKBinaryImageType2D::PixelType, 2 > StructuringElementType;    
    
    typedef /*typename*/ itk::BinaryDilateImageFilter< Utils::ITKBinaryImageType2D, Utils::ITKBinaryImageType2D, SE2Type > BinaryDilateImageFilterType2D;
    
    static void DilateDiskMatrixFilter2D(arma::Mat<unsigned char>* inputMatrix, arma::Mat<unsigned char>* outputMatrix, unsigned int radius, bool radiusIsParametric = true);    
    
    typedef /*typename*/ itk::BinaryErodeImageFilter< Utils::ITKBinaryImageType2D, Utils::ITKBinaryImageType2D, SE2Type > BinaryErodeImageFilterType2D;
    static void ErodeDiskMatrixFilter2D(arma::Mat<unsigned char>* inputMatrix, arma::Mat<unsigned char>* outputMatrix, unsigned int radius, bool radiusIsParametric = true);     
    
//     typedef itk::Size< 3 > SizeType;
    template <typename ArmaType, typename DataType, int Dimension> static void ErodeDilateBallArmaFilter(ERODE_DILATE type, ArmaType* inputArma, ArmaType* outputArma, unsigned int* radius, itk::ThreadProcessIdType numOfThreads = 1, arma::Mat<itk::IndexValueType>::fixed<2, Dimension>* region = 0, DataType foregroundValue = 1, bool radiusIsParametric = true)
/*    template <typename ArmaType, typename DataType, int Dimension> static void ErodeDilateBallArmaFilter(ERODE_DILATE type, ArmaType* inputArma, ArmaType* outputArma, SizeType radius, itk::ThreadProcessIdType numOfThreads = 1, arma::Mat<itk::IndexValueType>::fixed<2, Dimension>* region = 0, DataType foregroundValue = 1, bool radiusIsParametric = true)  */  
    {
        auto begin = chrono::high_resolution_clock::now();
        
        typedef itk::Image< DataType,  Dimension > ITKBinaryImageType;
        typename ITKBinaryImageType::Pointer inputImageITK = Utils::armaToITKImage<DataType, Dimension>(inputArma->memptr(), Utils::armaSize<DataType>(inputArma));
        
//         typedef itk::RegionOfInterestImageFilter< ITKBinaryImageType, ITKBinaryImageType > ROIFilterType;
            
//         typedef itk::FlatStructuringElement< Dimension > SEType;
        typedef itk::FlatStructuringElement< Dimension > SEType;
        typename SEType::RadiusType radiusType;
        typedef itk::Size< Dimension > SizeType;
        SizeType radio;
        for (int i=0;i<Dimension;i++)
            radio[i]=radius[i];
        radiusType.SetSize( radio.m_Size );  
//         radiusType.SetSize( radius.m_Size );
        
//         cout << "StructuringElementType" << endl;
//         StructuringElementTypetypedef itk::BinaryBallStructuringElement< TypeOfPixel, Dimension > StructuringElementType;
//         StructuringElementType structuringElement;        
        
//         cout << "SEType::Ball" << endl;
//         SEType ball;
        SEType ball;
        ball = SEType::Ball(radiusType, radiusIsParametric);
        
//         structuringElement.SetRadius(radiusType);
//         structuringElement.setR
//         structuringElement.CreateStructuringElement();
        
        typedef itk::BinaryMorphologyImageFilter< ITKBinaryImageType, ITKBinaryImageType, SEType > BinaryMorphologyImageFilterType;
        typename BinaryMorphologyImageFilterType::Pointer filter;
        if(type == Erode) {
            typedef itk::BinaryErodeImageFilter< ITKBinaryImageType, ITKBinaryImageType, SEType > BinaryErodeImageFilterType;
            filter = BinaryErodeImageFilterType::New();
        }
        else {
            typedef itk::BinaryDilateImageFilter< ITKBinaryImageType, ITKBinaryImageType, SEType > BinaryDilateImageFilterType;
//             cout << "BinaryDilateImageFilterType New" << endl;
            filter = BinaryDilateImageFilterType::New();
        }

        filter->SetForegroundValue(foregroundValue);        
        filter->SetInput(inputImageITK);
        filter->SetKernel(ball);
        filter->SetNumberOfThreads(numOfThreads);
        if(region) {    
            itk::Index<Dimension> indexR;
            indexR[0] = region->at(0,1); // minCol
            indexR[1] = region->at(0,0); // minRow
            if(Dimension == 3)
                indexR[2] = region->at(0,2);
            itk::Size<Dimension> sizeR;
            sizeR[0] = region->at(1,1) - region->at(0,1) + 1;
            sizeR[1] = region->at(1,0) - region->at(0,0) + 1;
            if(Dimension == 3)
                sizeR[2] = region->at(1,2) - region->at(0,2) + 1;
            itk::ImageRegion<Dimension> regionITK(indexR, sizeR); 
            cout << indexR << " " << sizeR << endl;
            filter->GetOutput()->SetRequestedRegion(regionITK);
            cout << "tras region" << endl;
        }  
//         cout << "BinaryDilateImageFilterType Update" << endl;
        filter->Update();
        
        const itk::Size<Dimension> size = inputImageITK->GetLargestPossibleRegion().GetSize();
//         if(Dimension == 2)
//             *outputArma = ArmaType(filter->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);
//         else if(Dimension == 3)
//             *outputArma = ArmaType(filter->GetOutput()->GetBufferPointer(), size[0], size[1], size[2], true, false);        
        
//         ITKBinaryImageType* out = filter->GetOutput();

//         if(region) {
//             typedef itk::ImageFileWriter< ITKBinaryImageType > WriterType;
//             typename WriterType::Pointer writer;
//             writer = WriterType::New();
//             writer->SetFileName( "DilateTest.nii" );
//             writer->SetInput( filter->GetOutput() );
//             writer->Update();   
//         }
        
        armaFromITKPointer<ArmaType, DataType, Dimension>(filter->GetOutput()->GetBufferPointer(), outputArma, size, true, false);
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender) {
            QString sType = type == Erode ? "Erode" : "Dilate";
            logSender->logText(sType + "DiskMatrixFilter" + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        }
        // 	cout << "ErodeDilateBallArmaFilter " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;        
    }
    
    template <typename ArmaType, typename DataType, int Dimension> struct armaFromITKStruct;
    
    template <typename ArmaType, typename DataType> struct armaFromITKStruct<ArmaType, DataType, 2>
    {
        static void armaFromITKPointer(DataType* imageBuffer, ArmaType* outputArma, const itk::Size<2> size, bool copy_aux_mem, bool strict) {
            *outputArma = ArmaType(&imageBuffer[0], size[0], size[1], copy_aux_mem, strict);        
        }
    };
    
    template <typename ArmaType, typename DataType> struct armaFromITKStruct<ArmaType, DataType, 3>
    {
        static void armaFromITKPointer(DataType* imageBuffer, ArmaType* outputArma, const itk::Size<3> size, bool copy_aux_mem, bool strict) {
            *outputArma = ArmaType(&imageBuffer[0], size[0], size[1], size[2], copy_aux_mem, strict);        
        }
    };   
    
    template <typename ArmaType, typename DataType, int Dimension> static void armaFromITKPointer(DataType* imageBuffer, ArmaType* outputArma, const itk::Size<Dimension> size, bool copy_aux_mem, bool strict)
    {
        armaFromITKStruct<ArmaType, DataType, Dimension>::armaFromITKPointer(imageBuffer, outputArma, size, copy_aux_mem, strict);
    }
    
    typedef /*typename*/ itk::BinaryContourImageFilter< Utils::ITKBinaryImageType2D, Utils::ITKBinaryImageType2D > BinaryContourImageFilterType2D;    
    static void BinaryContourMatrixFilter2D(arma::Mat<unsigned char>* inputMatrix, arma::Mat<unsigned char>* outputMatrix); 

    template <typename ArmaType, typename DataType, int Dimension> static void BinaryContourArmaFilter(ArmaType* inputArma, ArmaType* outputArma, itk::ThreadProcessIdType numOfThreads = 1, DataType foregroundValue = 1)
    {
        auto begin = chrono::high_resolution_clock::now();
        
        typedef itk::Image< DataType,  Dimension > ITKBinaryImageType;
        typename ITKBinaryImageType::Pointer inputImageITK = Utils::armaToITKImage<DataType, Dimension>(inputArma->memptr(), Utils::armaSize<DataType>(inputArma));
        
        typedef itk::Image<DataType, Dimension > ITKImageType;        
        typedef itk::BinaryContourImageFilter< ITKImageType, ITKImageType > BinaryContourImageFilterType;
        typename BinaryContourImageFilterType::Pointer filter; 
        filter = BinaryContourImageFilterType::New();
        
        filter->SetForegroundValue(foregroundValue);
        filter->SetBackgroundValue(0);
        filter->SetFullyConnected(true);        
        filter->SetInput(inputImageITK);
        filter->SetNumberOfThreads(numOfThreads);
        filter->Update();

        const itk::Size<Dimension> size = inputImageITK->GetLargestPossibleRegion().GetSize();
//         if(Dimension == 2)
//             *outputArma = arma::Mat<DataType>(filter->GetOutput()->GetBufferPointer(), size[0], size[1], true, false);
//         else if(Dimension == 3)
//             *outputArma = arma::Cube<DataType>(filter->GetOutput()->GetBufferPointer(), size[0], size[1], size[2], true, false);
        armaFromITKPointer<ArmaType, DataType, Dimension>(filter->GetOutput()->GetBufferPointer(), outputArma, size, true, false);

        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("BinaryContourMatrixFilter " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
    // 	cout << "BinaryContourMatrixFilter " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;         
    }
    
    
    // compute simple blob features  (no se llama a ninguna función de ITK para calcularlas, se calculan directamente).
    typedef long unsigned int LabelObjectType;
    template <typename ArmaType, int Dimension> static void BinaryLabelsFilter(vector<typename ImageFilters::LabelSimpleFeatures< Dimension> >* labelsInfo, ArmaType* armaType, itk::ThreadProcessIdType numOfThreads = 1, bool fullConnectivity = true, bool calculateBlobDeep = false)    
    {
        typedef typename itk::BinaryImageToLabelMapFilter< itk::Image<unsigned char, Dimension> > FilterLabelMap;
        typedef typename FilterLabelMap::Pointer FilterLabelMapPtr;

        auto begin = chrono::high_resolution_clock::now();
        
        arma::uvec/*::fixed<Dimension>*/ imgDimensions = Utils::armaSize<unsigned char>(armaType);
        typedef typename itk::Image<unsigned char, Dimension>::Pointer  InputImagePointer;
        InputImagePointer inputImageITK = Utils::armaToITKImage<unsigned char, Dimension>(armaType->memptr(), imgDimensions);	
        FilterLabelMapPtr filterLabels = FilterLabelMap::New();
        
        filterLabels->SetFullyConnected(fullConnectivity);
        filterLabels->SetInputForegroundValue(1);
        filterLabels->SetInput(inputImageITK);
        filterLabels->SetNumberOfThreads(numOfThreads);
        filterLabels->Update();
        
        typedef typename itk::LabelObject< LabelObjectType, Dimension >::Pointer LabelObjectTypePtr;
        vector<LabelObjectTypePtr> labelMapVector = filterLabels->GetOutput()->GetLabelObjects();
        
        itk::SizeValueType numLabels = labelMapVector.size();
        labelsInfo->resize(numLabels);
        
        typename vector<LabelObjectTypePtr>::iterator allLabelsIt;
        typedef typename vector<ImageFilters::LabelSimpleFeatures< Dimension> >::iterator LabelsInfoIterator;
        LabelsInfoIterator labelsInfoIt = labelsInfo->begin();
//         typename FilterLabelMap::LabelType labelValue;
        typedef itk::Index<Dimension> IndexType;

//         int i = 0;
        for(allLabelsIt = labelMapVector.begin(); allLabelsIt != labelMapVector.end(); allLabelsIt++) {
            
//             labelValue = (*allLabelsIt)->GetLabel();
            (*labelsInfoIt).labelValue = (*allLabelsIt)->GetLabel();
            (*labelsInfoIt).volume = (*allLabelsIt)->Size();
            (*labelsInfoIt).blobDeep = 0.0;
            (*labelsInfoIt).centroid.zeros();
//             if(Dimension == 3)
//                 cout << (*labelsInfoIt).volume << endl;
            
            (*labelsInfoIt).pixelsIndices.set_size((*labelsInfoIt).volume, Dimension);
            (*labelsInfoIt).indexOfPixels.set_size((*labelsInfoIt).volume);	                
//             auto beginFor = chrono::high_resolution_clock::now();
            typename itk::LabelObject< LabelObjectType, Dimension >::ConstIndexIterator it( *allLabelsIt );
            unsigned int cnt = 0;
            while(!it.IsAtEnd()) {
                (*labelsInfoIt).indexOfPixels(cnt) = 0;
                const IndexType & data = it.GetIndex();
                IndexType dataSwap = data; 
                itk::IndexValueType swap = dataSwap[1];
                dataSwap[1] = dataSwap[0];
                dataSwap[0] = swap;                
                for(int j = 0; j < Dimension; j++) {
                    (*labelsInfoIt).pixelsIndices(cnt, j) = data[j];
//                     (*labelsInfoIt).centroid(j) += data[j];
                    (*labelsInfoIt).centroid(j) += dataSwap[j];
                    if(j == 0) {
                        (*labelsInfoIt).indexOfPixels(cnt) += data[j];			
                    }
                    else if(j == 1)
                        (*labelsInfoIt).indexOfPixels(cnt) += data[j] * imgDimensions(j - 1);
                    else if(j == 2)
                        (*labelsInfoIt).indexOfPixels(cnt) += data[j] * imgDimensions(j - 1) * imgDimensions(j - 2);
                }
                cnt++;
                ++it;
            }            
            (*labelsInfoIt).centroid = (*labelsInfoIt).centroid / (*labelsInfoIt).volume;

//             for(long unsigned int i = 0; i < (*labelsInfoIt).volume; i++) {                
//                 (*labelsInfoIt).indexOfPixels(i) = 0;
//                 for(int j = 0; j < Dimension; j++) {
//                     data = (*allLabelsIt)->GetIndex(i)[j];
//                     data = 5;
//                     (*labelsInfoIt).pixelsIndices(i, j) = data;
//                     if(j == 0) {
//                         (*labelsInfoIt).indexOfPixels(i) += data;			
//                     }
//                     else if(j == 1)
//                         (*labelsInfoIt).indexOfPixels(i) += data * imgDimensions(j - 1);
//                     else if(j == 2)
//                         (*labelsInfoIt).indexOfPixels(i) += data * imgDimensions(j - 1) * imgDimensions(j - 2);		   
//                 }
//             }
            
//             if(Dimension == 3) {
//                 auto endFor = chrono::high_resolution_clock::now();
//                 cout << "BinaryLabelsFilter For " << i << "D " << chrono::duration_cast<chrono::nanoseconds>(endFor-beginFor).count()/1000000.0 << "ms" << endl;
//                     
//             }
            
            unsigned int minRow;
            unsigned int minCol;
            unsigned int minSlice = 0;
            unsigned int maxRow;
            unsigned int maxCol;
            unsigned int maxSlice = 0;	    
            arma::Col<unsigned int> rows, cols, slices;
            
            if(Dimension >= 2) {		
                rows = (*labelsInfoIt).pixelsIndices.col(1);
                cols = (*labelsInfoIt).pixelsIndices.col(0);
                minRow = min(rows);
                maxRow = max(rows);
                minCol = min(cols);
                maxCol = max(cols);
                // MSVC-2013 has a bug boundingBox(0,0) not valid needs at()
                (*labelsInfoIt).boundingBox.at(0,0) = minRow;
                (*labelsInfoIt).boundingBox.at(1,0) = maxRow;
                (*labelsInfoIt).boundingBox.at(0,1) = minCol;
                (*labelsInfoIt).boundingBox.at(1,1) = maxCol;		
            }
            if(Dimension == 3) {
                slices = (*labelsInfoIt).pixelsIndices.col(2);
                minSlice = min(slices);
                maxSlice = max(slices);
                (*labelsInfoIt).boundingBox.at(0,2) = minSlice;
                (*labelsInfoIt).boundingBox.at(1,2) = maxSlice;		
            }
            
            if(calculateBlobDeep) {
                computeBlobDeep<ImageFilters::LabelSimpleFeatures< Dimension>, Dimension>(&(*labelsInfoIt));		
            }

//             i++;
            labelsInfoIt++;
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("BinaryLabelsFilter" + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        // 	    cout << "BinaryLabelsFilter" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
        
        filterLabels = NULL;
    }
    
    template <typename LabelFeaturesType, int Dimension> static void computeBlobDeep(LabelFeaturesType* blob, itk::ThreadProcessIdType numOfThreads = 1)
    {
        arma::Mat<unsigned char> blobArma;
        NotEmptyForeground notEmptyForeground = NOT_KNOWN;
        bool notSigned = true;
        unsigned int minRow = blob->boundingBox.at(0,0);
        unsigned int minCol = blob->boundingBox.at(0,1);
        unsigned int minSlice = blob->boundingBox.at(0,2);
        unsigned int maxRow = blob->boundingBox.at(1,0);
        unsigned int maxCol = blob->boundingBox.at(1,1);
        unsigned int maxSlice = blob->boundingBox.at(1,2);  
        
        // Se crea un nuevo volumen con el blob y un recubrimiento de un voxel a su alrededor. Para calcular la distancia al borde, se pone a 1 el background y a 0 el blob
        if(Dimension == 2) {
            blobArma.ones(maxRow - minRow + 3, maxCol - minCol + 3);
            for(unsigned int i = 0; i < blob->volume; i ++) {
                blobArma(blob->pixelsIndices(i, 1) - minRow + 1, blob->pixelsIndices(i, 0) - minCol + 1) = 0;    // mari: he añadido +1 a todos los índices para que el blob no toque los bordes
            }
            arma::fmat Dist;
            
            ImageFilters::DistanceMapFilter< arma::Mat<unsigned char>, unsigned char, arma::Mat<float>, float, Dimension>(&blobArma, &Dist, notEmptyForeground, notSigned, numOfThreads);		
            blob->blobDeep = Dist.max();
            // 		    cout << blobArma;
            // 		    cout << endl;
            // 		    cout << Dist;
            // 		    cout << endl << endl;
            // 		    (*labelsInfoIt).print();
            
            //mari **1** // superficie como numero de voxels en el contorno
            blob->surface = size(find(Dist == 1),0);	
            
            //Dist.save("DistToCHULL3D.mat", arma::arma_ascii);     
 
            
            

        }
        else if(Dimension == 3) {
            arma::Cube<unsigned char> blobArma;
            blobArma.ones(maxRow - minRow + 3, maxCol - minCol + 3, maxSlice - minSlice + 3);
            for(unsigned int i = 0; i < blob->volume; i ++) {
                blobArma(blob->pixelsIndices(i, 1) - minRow+1, blob->pixelsIndices(i, 0) - minCol+1, blob->pixelsIndices(i, 2) - minSlice+1) = 0;  // mari: he añadido +1 a todos los índices para que el blob no toque los bordes
            }		
            
            arma::fcube Dist;
            ImageFilters::DistanceMapFilter< arma::Cube<unsigned char>, unsigned char, arma::Cube<float>, float, Dimension>(&blobArma, &Dist, notEmptyForeground, notSigned, numOfThreads);		
            blob->blobDeep = Dist.max();	                        
            
            //mari **1** // superficie como numero de voxels en el contorno
            blob->surface = size(find(Dist == 1),0);	
            //Dist.save("DistBlob.mat", arma::arma_ascii);     
            //blobArma.save("BlobArma.mat", arma::arma_ascii); 
        }	        
    }
    
    // BinaryLabelGeometryFilter invocation   (más características)
    template <typename ArmaType, typename LabelDataType, int Dimension, typename ArmaTypeIntensity = ArmaType, typename DataTypeIntensity = LabelDataType> static void BinaryLabelGeometryFilter(vector<typename ImageFilters::LabelFeatures< Dimension> >* labelsInfo, ArmaType* armaType, itk::ThreadProcessIdType numOfThreads = 1, bool fullConnectivity = true, bool calculateBlobDeep = false, ArmaTypeIntensity* armaTypeIntensity = 0, bool excludeLabelZero = true)
    {
        auto begin = chrono::high_resolution_clock::now();
        
        arma::uvec/*::fixed<Dimension>*/ imgDimensions = Utils::armaSize<unsigned char>(armaType);
        typedef typename itk::Image<unsigned char, Dimension>::Pointer InputImagePointer;
        InputImagePointer inputImageITK = Utils::armaToITKImage<unsigned char, Dimension>(armaType->memptr(), imgDimensions);
        
        
        typedef typename itk::Image<DataTypeIntensity, Dimension> InputIntensityImageType;
        InputIntensityImageType* inputIntensityImageITK = 0;
        if(armaTypeIntensity)
            inputIntensityImageITK = Utils::armaToITKImage<DataTypeIntensity, Dimension>(armaTypeIntensity->memptr(), Utils::armaSize<DataTypeIntensity>(armaTypeIntensity));	
        
        typedef typename itk::ConnectedComponentImageFilter <itk::Image<unsigned char, Dimension>, itk::Image<LabelDataType, Dimension> > ConnectedComponentImageFilterType;
        typedef typename ConnectedComponentImageFilterType::Pointer ConnectedComponentImageFilterTypePtr; 
        ConnectedComponentImageFilterTypePtr connected = ConnectedComponentImageFilterType::New ();
        connected->SetInput(inputImageITK);
        connected->SetFullyConnected(fullConnectivity);
        connected->Update();	
        
        typedef typename itk::LabelGeometryImageFilter< itk::Image<LabelDataType, Dimension> > LabelGeometryImageFilterType;
        typedef typename LabelGeometryImageFilterType::Pointer LabelGeometryImageFilterTypePtr;
        LabelGeometryImageFilterTypePtr labelGeometryImageFilter = LabelGeometryImageFilterType::New();
        labelGeometryImageFilter->SetNumberOfThreads(numOfThreads);
        labelGeometryImageFilter->SetInput(connected->GetOutput());
        if(armaTypeIntensity)
            labelGeometryImageFilter->SetIntensityInput(inputIntensityImageITK);
        
        labelGeometryImageFilter->CalculatePixelIndicesOn();
        labelGeometryImageFilter->CalculateOrientedBoundingBoxOn();
        labelGeometryImageFilter->CalculateOrientedLabelRegionsOn();
        if(armaTypeIntensity)
            labelGeometryImageFilter->CalculateOrientedIntensityRegionsOn();
        
        labelGeometryImageFilter->Update();
        // 	typename LabelGeometryImageFilterType::LabelsType labels = labelGeometryImageFilter->GetLabels();
        
        itk::SizeValueType numLabels = labelGeometryImageFilter->GetNumberOfLabels();
        if(excludeLabelZero)
            numLabels--;
        labelsInfo->resize(numLabels);
        typedef typename LabelGeometryImageFilterType::LabelsType VectorLabels;
        VectorLabels labels = labelGeometryImageFilter->GetLabels();
        typename VectorLabels::iterator allLabelsIt = labels.begin();
        if(excludeLabelZero)
            allLabelsIt++;
        typedef typename vector<ImageFilters::LabelFeatures< Dimension> >::iterator labelsInfoIterator;
        labelsInfoIterator labelsInfoIt = labelsInfo->begin();
        
        for( ; allLabelsIt != labels.end(); allLabelsIt++) {
            // 	    LabelFeatures<Dimension> features;
            typename LabelGeometryImageFilterType::LabelPixelType labelValue = *allLabelsIt;
            (*labelsInfoIt).labelValue = labelValue;
            (*labelsInfoIt).volume = labelGeometryImageFilter->GetVolume(labelValue);
            (*labelsInfoIt).integratedIntensity = labelGeometryImageFilter->GetIntegratedIntensity(labelValue);
            
            for(int i = 0; i < Dimension; i++)
                (*labelsInfoIt).centroid[i] = labelGeometryImageFilter->GetCentroid(labelValue)[i];
            for(int i = 0; i < Dimension; i++)
                (*labelsInfoIt).weightedCentroid[i] = labelGeometryImageFilter->GetWeightedCentroid(labelValue)[i];
            for(int i = 0; i < Dimension; i++)
                (*labelsInfoIt).axesLength[i] = labelGeometryImageFilter->GetAxesLength(labelValue)[i];
            
            (*labelsInfoIt).majorAxisLength = labelGeometryImageFilter->GetMajorAxisLength(labelValue);
            (*labelsInfoIt).minorAxisLength = labelGeometryImageFilter->GetMinorAxisLength(labelValue);
            (*labelsInfoIt).eccentricity = labelGeometryImageFilter->GetEccentricity(labelValue);
            (*labelsInfoIt).elongation = labelGeometryImageFilter->GetElongation(labelValue);
            (*labelsInfoIt).orientation = labelGeometryImageFilter->GetOrientation(labelValue);
            
            for(int i = 0; i < Dimension; i++)
                (*labelsInfoIt).boundingBox.at(0, i) = labelGeometryImageFilter->GetBoundingBox(labelValue)[2*i];
            for(int i = 0; i < Dimension; i++)
                (*labelsInfoIt).boundingBox.at(1, i) = labelGeometryImageFilter->GetBoundingBox(labelValue)[2*i + 1];
            
            (*labelsInfoIt).pixelsIndices.set_size((*labelsInfoIt).volume, Dimension);
            (*labelsInfoIt).indexOfPixels.set_size((*labelsInfoIt).volume);
            
            if(calculateBlobDeep) {
                computeBlobDeep<ImageFilters::LabelFeatures< Dimension>, Dimension>(&(*labelsInfoIt));		
            }
            
            vector<itk::Index<Dimension> > data = labelGeometryImageFilter->GetPixelIndices(labelValue);
            
            for(long unsigned int i = 0; i < (*labelsInfoIt).volume; i++) {
                (*labelsInfoIt).indexOfPixels(i) = 0;
                for(int j = 0; j < Dimension; j++) {		   
                    (*labelsInfoIt).pixelsIndices(i, j) = data[i][j];
                    if(j == 0)
                        (*labelsInfoIt).indexOfPixels(i) += data[i][j];
                    else if(j == 1)
                        (*labelsInfoIt).indexOfPixels(i) += data[i][j] * imgDimensions(j - 1);
                    else if(j == 2)
                        (*labelsInfoIt).indexOfPixels(i) += data[i][j] * imgDimensions(j - 1) * imgDimensions(j - 2);		   
                }
            }                        
            labelsInfoIt++;
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("BinaryLabelGeometryFilter" + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        // 	    cout << "BinaryLabelGeometryFilter" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl; 
        
        connected = NULL;
        labelGeometryImageFilter = NULL;
        
    }
    
    typedef /*typename*/ itk::BinaryThinningImageFilter< Utils::ITKBinaryImageType2D, Utils::ITKBinaryImageType2D > FilterBinaryThinningType2D;
    /*    typedef typename itk::BinaryPruningImageFilter< Utils::ITKBinaryImageType2D, Utils::ITKBinaryImageType2D > FilterBinaryPruningType2D;*/ 

    /*    typedef typename itk::SpecialSkeletonPointsImageFilter< Utils::ITKBinaryImageType2D, Utils::ITKBinaryImageType2D > FilterBinaryEndpoints2D;    
        *   static void SkeletonEndPointsMatrixFilter2D(Mat<unsigned char>* inputMatrix, Mat<unsigned char>* outputMatrix, unsigned int iterations)*/
        
        template <typename ArmaType, int Dimension> static void ThresholdByLabelsSizeFilter(ArmaType* armaIn, long unsigned int threshold, itk::ThreadProcessIdType numOfThreads = 1/*, bool fullConnectivity = true*/) 
        {
            if(threshold == 0)
                return;
            
            auto begin = chrono::high_resolution_clock::now();
            
            vector<LabelSimpleFeatures<Dimension> > labelsSimpleInfo;
            BinaryLabelsFilter<ArmaType, Dimension >(&labelsSimpleInfo, armaIn, numOfThreads);
            typename vector<LabelSimpleFeatures<Dimension> >::iterator labelsSimpleInfoIt;
            for( labelsSimpleInfoIt = labelsSimpleInfo.begin(); labelsSimpleInfoIt != labelsSimpleInfo.end(); labelsSimpleInfoIt++) {
                if((*labelsSimpleInfoIt).volume < threshold) {
                    armaIn->elem((*labelsSimpleInfoIt).indexOfPixels) = arma::zeros< arma::Col<unsigned char> >((*labelsSimpleInfoIt).volume);
                }
            }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("ThresholdBySizeLabelsFilter" + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        // 	    cout << "ThresholdBySizeLabelsFilter" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl; 	
    }

    // segmentation 2: select blobs coincident with seed
    template <typename ArmaType, typename DataType, int Dimension> static void filteringImageWithSeed(ArmaType* armaIn, ArmaType* armaSeed, ArmaType* armaOut, bool create = true, itk::ThreadProcessIdType numOfThreads = 1/*, bool fullConnectivity = true*/)
    {	
        arma::uvec sizesDiff = find(Utils::armaSize<DataType>(armaIn) - Utils::armaSize<DataType>(armaSeed));
        if(sizesDiff.n_elem > 0) {
            cout << "filteringImageWithSeed error: armaTypeIn and armaTypeSeed sizes are different" << endl;
        }
        
        vector<LabelSimpleFeatures<Dimension> > labelsSimpleInfo;
        BinaryLabelsFilter<ArmaType, Dimension >(&labelsSimpleInfo, armaIn, numOfThreads);
        typename vector<LabelSimpleFeatures<Dimension> >::iterator labelsSimpleInfoIt;
        
        auto begin = chrono::high_resolution_clock::now();
        
        if(create) {
            armaOut->copy_size(*armaIn);
            armaOut->zeros();
        }	
        
        arma::uvec indSeed = find(*armaSeed);
        arma::uvec::iterator it = indSeed.begin();
        arma::uvec::const_iterator ite = indSeed.end();
        for(labelsSimpleInfoIt = labelsSimpleInfo.begin(); labelsSimpleInfoIt != labelsSimpleInfo.end(); labelsSimpleInfoIt++) {
            while(it < ite) {
                if( *it == arma::datum::nan) {
                    it++;
                    continue;
                }
                arma::uvec f = find((*labelsSimpleInfoIt).indexOfPixels == *it, 1);
                if(f.n_elem > 0) {
                    armaOut->elem((*labelsSimpleInfoIt).indexOfPixels) = armaIn->elem((*labelsSimpleInfoIt).indexOfPixels);
                    *it = arma::datum::nan;
                    it = indSeed.begin();
                    break;
                }
                ++it;
            }
            it = indSeed.begin();
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("FilteringImageWithSeed" + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        // 	    cout << "FilteringImageWithSeed" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;	
    }
    
     
    // segmentation 2: select blobs coincident with seed
    template <typename ArmaType, typename DataType, int Dimension> static void filteringImageWithSeed1(ArmaType* armaIn, ArmaType* armaSeed, ArmaType* armaOut, bool create = true, itk::ThreadProcessIdType numOfThreads = 1/*, bool fullConnectivity = true*/)
    {	
        auto begin = chrono::high_resolution_clock::now();
        
        if(create) {
            armaOut->copy_size(*armaIn);
            armaOut->zeros();
        }	
        
        arma::uvec indIn = find(*armaIn);
        arma::uvec indSeed = find(*armaSeed);        
        
        arma::uvec sizesDiff = find(Utils::armaSize<DataType>(armaIn) - Utils::armaSize<DataType>(armaSeed));
        if(sizesDiff.n_elem > 0) {
            cout << "filteringImageWithSeed error: armaTypeIn and armaTypeSeed sizes are different" << endl;
        }

        if(indIn.n_elem > 0 && indSeed.n_elem > 0) {
            vector<LabelSimpleFeatures<Dimension> > labelsSimpleInfo;
            BinaryLabelsFilter<ArmaType, Dimension >(&labelsSimpleInfo, armaIn, numOfThreads);
            typename vector<LabelSimpleFeatures<Dimension> >::iterator labelsSimpleInfoIt;                
            
            arma::uvec::iterator it = indSeed.begin();
            arma::uvec::const_iterator ite = indSeed.end();
            for(labelsSimpleInfoIt = labelsSimpleInfo.begin(); labelsSimpleInfoIt != labelsSimpleInfo.end(); labelsSimpleInfoIt++) {
                while(it < ite) {
                    if( *it == arma::datum::nan) {
                        it++;
                        continue;
                    }
                    arma::uvec f = find((*labelsSimpleInfoIt).indexOfPixels == *it, 1);
                    if(f.n_elem > 0) {
                        armaOut->elem((*labelsSimpleInfoIt).indexOfPixels) = armaIn->elem((*labelsSimpleInfoIt).indexOfPixels);
                        *it = arma::datum::nan;
                        it = indSeed.begin();
                        break;
                    }
                    ++it;
                }
                it = indSeed.begin();
            }
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("FilteringImageWithSeed1" + QString::number(Dimension) + "D " +  QString::number(indIn.n_elem) + "," + QString::number(indSeed.n_elem) + " " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        // 	    cout << "FilteringImageWithSeed" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;	
    }    
        
        // segmentation 2: select blobs coincident with seed
    template <typename ArmaType, typename DataType, int Dimension> static void filteringImageWithSeed2(ArmaType* armaIn, ArmaType* armaSeed, ArmaType* armaOut, bool create = true, itk::ThreadProcessIdType numOfThreads = 1/*, bool fullConnectivity = true*/)
    {	
        auto begin = chrono::high_resolution_clock::now();
        
        if(create) {
            armaOut->copy_size(*armaIn);
            armaOut->zeros();
        }
        
        arma::uvec indIn = find(*armaIn);
        arma::uvec indSeedInit = find(*armaSeed);         
        
        arma::uvec sizesDiff = find(Utils::armaSize<DataType>(armaIn) - Utils::armaSize<DataType>(armaSeed));
        if(sizesDiff.n_elem > 0) {
            cout << "filteringImageWithSeed error: armaTypeIn and armaTypeSeed sizes are different" << endl;
        }
        
        if(indIn.n_elem > 0 && indSeedInit.n_elem > 0) {
            vector<LabelSimpleFeatures<Dimension> > labelsSimpleInfo;
            BinaryLabelsFilter<ArmaType, Dimension >(&labelsSimpleInfo, armaIn, numOfThreads);
            typename vector<LabelSimpleFeatures<Dimension> >::iterator labelsSimpleInfoIt;                               
            
            ArmaType intersec = *armaIn % *armaSeed;
            
            vector<LabelSimpleFeatures<Dimension> > labelsSimpleInfoSeed;
            BinaryLabelsFilter<ArmaType, Dimension >(&labelsSimpleInfoSeed, &intersec, numOfThreads);
            typename vector<LabelSimpleFeatures<Dimension> >::iterator labelsSimpleInfoSeedIt;         
            
            arma::uvec indSeed;
            indSeed.set_size(labelsSimpleInfoSeed.size());
            int i = 0;
            for(labelsSimpleInfoSeedIt = labelsSimpleInfoSeed.begin(); labelsSimpleInfoSeedIt != labelsSimpleInfoSeed.end(); labelsSimpleInfoSeedIt++) {
                indSeed.at(i) = (*labelsSimpleInfoSeedIt).indexOfPixels.at(0);
                i++;
            }
            arma::uvec::iterator it = indSeed.begin();
            arma::uvec::const_iterator ite = indSeed.end();        
            
            for(labelsSimpleInfoIt = labelsSimpleInfo.begin(); labelsSimpleInfoIt != labelsSimpleInfo.end(); labelsSimpleInfoIt++) {
                while(it < ite) {
                    if( *it == arma::datum::nan) {
                        it++;
                        continue;
                    }
                    arma::uvec f = find((*labelsSimpleInfoIt).indexOfPixels == *it, 1);
                    if(f.n_elem > 0) {
                        armaOut->elem((*labelsSimpleInfoIt).indexOfPixels) = armaIn->elem((*labelsSimpleInfoIt).indexOfPixels);
                        *it = arma::datum::nan;
                        it = indSeed.begin();
                        break;
                    }
                    ++it;
                }
                it = indSeed.begin();
            }
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("FilteringImageWithSeed2" + QString::number(Dimension) + "D " + QString::number(indIn.n_elem) + "," + QString::number(indSeedInit.n_elem) + " " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
            // 	    cout << "FilteringImageWithSeed" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;	
        }    
     
     // segmentation 2: select blobs coincident with seed
//     template <typename ArmaType, typename DataType, int Dimension> static void filteringImageWithSeed3(ArmaType* armaIn, ArmaType* armaSeed, ArmaType* armaOut, bool create = true, itk::ThreadProcessIdType numOfThreads = 1/*, bool fullConnectivity = true*/)
//     {	
//         arma::uvec sizesDiff = find(Utils::armaSize<DataType>(armaIn) - Utils::armaSize<DataType>(armaSeed));
//         if(sizesDiff.n_elem > 0) {
//             cout << "filteringImageWithSeed error: armaTypeIn and armaTypeSeed sizes are different" << endl;
//         }
//         
//         vector<LabelSimpleFeatures<Dimension> > labelsSimpleInfo;
//         BinaryLabelsFilter<ArmaType, Dimension >(&labelsSimpleInfo, armaIn, numOfThreads);
//         typename vector<LabelSimpleFeatures<Dimension> >::iterator labelsSimpleInfoIt;
//         
//         auto begin = chrono::high_resolution_clock::now();
//         
//         if(create) {
//             armaOut->copy_size(*armaIn);
//             armaOut->zeros();
//         }	
//         typedef long unsigned int LabelObjectType;
//         arma::Cube<LabelObjectType> armaInLabels;
//         armaInLabels.zeros(size(*armaIn));
//         
//         for(labelsSimpleInfoIt = labelsSimpleInfo.begin(); labelsSimpleInfoIt != labelsSimpleInfo.end(); labelsSimpleInfoIt++) {
//             armaInLabels.elem((*labelsSimpleInfoIt).indexOfPixels) += (*labelsSimpleInfoIt).labelValue;
//         }
//         armaInLabels.save("armaInLabels.mat", arma::arma_ascii);
//         
//         arma::Cube<LabelObjectType> armaSeedLabel = arma::conv_to< arma::Cube<LabelObjectType> >::from(*armaSeed);
//         arma::Cube<LabelObjectType> armaSeed_x_Label = armaInLabels % armaSeedLabel;
//         armaSeed_x_Label.save("armaSeed_x_Label.mat", arma::arma_ascii);
//         arma::Mat<LabelObjectType> indSeed = unique(armaSeed_x_Label);        
//         
//         for(labelsSimpleInfoIt = labelsSimpleInfo.begin(); labelsSimpleInfoIt != labelsSimpleInfo.end(); labelsSimpleInfoIt++) {
//             arma::uvec f = find(indSeed == (*labelsSimpleInfoIt).labelValue, 1);
//             if(f.n_elem > 0)
//                 armaOut->elem((*labelsSimpleInfoIt).indexOfPixels) = armaIn->elem((*labelsSimpleInfoIt).indexOfPixels);
//         }
//         
//         auto end = chrono::high_resolution_clock::now();
//         if(Utils::showTime && logSender)
//             logSender->logText("FilteringImageWithSeed3" + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
//          // 	    cout << "FilteringImageWithSeed" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;	
//      }          
     
    template <typename DistArmaType, typename DataInputType, typename ArmaOutputType, typename DataOutputType, int Dimension> static void SkeletonDistanceFilter(DistArmaType* inputArma, ArmaOutputType* outputArma, int prunningSteps = 0, itk::ThreadProcessIdType numOfThreads = 1)
    {
        typedef itk::Image< DataInputType, Dimension > distanceImageType;
        typedef itk::Image< DataOutputType, Dimension > outputImageType;
        
        typedef itk::SpecialSkeletonPointsImageFilter< outputImageType, outputImageType > FilterBinaryEndpoints;
        typedef typename FilterBinaryEndpoints::Pointer endpointsFilterPtr;
        
        auto begin = chrono::high_resolution_clock::now();
        
        arma::uvec ind = find(*inputArma < 0);
        if(ind.n_elem > 0)
            inputArma->elem(ind) = arma::zeros< arma::Col<DataInputType> >(ind.n_elem);
        
        arma::uvec dimensions = Utils::armaSize<DataInputType>(inputArma);
        typename distanceImageType::Pointer distanceImageITK = Utils::armaToITKImage<DataInputType, Dimension>(inputArma->memptr(), dimensions);
        typedef typename outputImageType::Pointer outputImagePtr;
        
        typedef itk::SkeletonizeBaseImageFilter<distanceImageType, outputImageType> SkeletonBaseFilter;
        typename SkeletonBaseFilter::Pointer skel = SkeletonBaseFilter::New();
        skel->SetForegroundCellConnectivity(0);
        skel->SetBackgroundCellConnectivity(1);
        skel->SetNumberOfThreads(numOfThreads);
        
        skel->SetInput(distanceImageITK);
        skel->Update();
        outputImagePtr skelImage = skel->GetOutput();
        outputImagePtr endpointsImage;
        
        Utils::createArmaFromBuffer<DataOutputType>(outputArma, skelImage->GetBufferPointer(), dimensions, true, false);	
        ArmaOutputType endpointsArma;
        
        for(int i = 0; i < prunningSteps; i++) {
            endpointsFilterPtr endpoints = FilterBinaryEndpoints::New();
            endpoints->SetInput(skelImage);
            endpoints->SetForegroundCellConnectivity(0);
            endpoints->SetEndPoints(true);
            endpoints->Update();
            endpointsImage = endpoints->GetOutput();
            
            // 	    Utils::createArmaFromBuffer<DataOutputType>(&endpointsArma, endpointsImage->GetBufferPointer(), dimensions, true, false);
            Utils::createArmaFromBuffer<DataOutputType>(&endpointsArma, endpointsImage->GetBufferPointer(), dimensions);
            
            *outputArma = *outputArma - endpointsArma;
            if(i < prunningSteps - 1)
                skelImage = Utils::armaToITKImage<DataOutputType, Dimension>(outputArma->memptr(), dimensions);
            
            endpoints = NULL;
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("SkeletonDistanceFilter" + QString::number(Dimension) + "D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        // 	    cout << "SkeletonDistanceFilter" << Dimension << "D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
        
        skel = NULL;
    }
     
    template <typename DataType> static void ResampleCubeFilter2D(arma::Cube<DataType>* inputCube, arma::Cube<DataType>* outputCube, ResampleTypes resampleType, mitk::Vector2D inSpacing, mitk::Vector2D outSpacing, mitk::Vector2D inOrigin = mitk::Vector2D(0.0), mitk::Vector2D outOrigin = mitk::Vector2D(0.0), itk::ThreadProcessIdType numOfThreads = 1)
    {
        typedef itk::Image< DataType, 2 > ITKImageType;
        typename ITKImageType::Pointer inputSliceImage;
        
        typedef itk::ResampleImageFilter<ITKImageType, ITKImageType>  FilterType;
        typedef itk::AffineTransform< ScalarType, 2 >  TransformType;
        
        auto begin = chrono::high_resolution_clock::now();
        
        TransformType::Pointer transform = TransformType::New();
        transform->SetIdentity();
        
        arma::Mat<DataType> outputSlice;
        
        arma::uvec size = Utils::armaSize<DataType>(&(inputCube->slice(0)));
        unsigned int newWidth = (unsigned int) round((double) size[0] * inSpacing[0] / outSpacing[0]);
        unsigned int newHeight = (unsigned int) round((double) size[1] * inSpacing[1] / outSpacing[1]);
        itk::Size<2> outputSize = {{newWidth, newHeight}};
        outputCube->set_size(newWidth, newHeight, inputCube->n_slices);        
        
        typedef itk::InterpolateImageFunction<ITKImageType, double>  InterpolatorBaseType;
        typename InterpolatorBaseType::Pointer interpolator;        
        
        if(resampleType == LINEAR) {
            typedef itk::LinearInterpolateImageFunction<ITKImageType, double>  InterpolatorType;
            interpolator = InterpolatorType::New();
        }
        else if(resampleType == NEAREST) {
            typedef itk::NearestNeighborInterpolateImageFunction<ITKImageType, double>  InterpolatorType;
            interpolator = InterpolatorType::New();
        }
            
        for(unsigned int itS = 0; itS < inputCube->n_slices; itS++) {	    		
            inputSliceImage = Utils::armaToITKImage<DataType, 2>(inputCube->slice(itS).memptr(), size);
            inputSliceImage->SetSpacing(inSpacing);
            inputSliceImage->SetOrigin(inOrigin);
            // 		inputSliceImage->SetDirection(inDirection);
            typename FilterType::Pointer filter = FilterType::New();
            filter->SetDefaultPixelValue(0);
            filter->SetTransform(transform);
            filter->SetInterpolator(interpolator);
            filter->SetOutputOrigin(outOrigin);
            filter->SetOutputSpacing(outSpacing);
            // 		filter->SetOutputDirection(outDirection);
            filter->SetSize(outputSize);
            filter->SetNumberOfThreads(numOfThreads);
            filter->SetInput(inputSliceImage);
            filter->Update();
            arma::uvec dimensions(2);
            for(int i = 0; i < 2; i++)
                dimensions(i) = outputSize[i];
            Utils::createArmaFromBuffer<DataType>(&outputSlice, filter->GetOutput()->GetBufferPointer(), dimensions, true, false);
            outputCube->slice(itS) = outputSlice;
            
            filter = NULL;                                  
        }
        interpolator = NULL;
        transform = NULL;
 
        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("ResampleCubeFilter2D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        cout << "ResampleCubeFilter2D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << endl;                
    }  

//      template <typename DataType> static void ResampleCubeFilter2D(arma::Cube<DataType>* inputCube, arma::Cube<DataType>* outputCube, ResampleTypes resampleType, mitk::Vector2D inSpacing, mitk::Vector2D outSpacing, mitk::Vector2D inOrigin, itk::ThreadProcessIdType numOfThreads = 1)
//      {
//          typedef itk::Image< DataType, 2 > ITKImageType;
//          typename ITKImageType::Pointer inputSliceImage;
//          
//          typedef itk::ResampleImageFilter<ITKImageType, ITKImageType>  FilterType;
//          typedef itk::AffineTransform< ScalarType, 2 >  TransformType;
//          
//          auto begin = chrono::high_resolution_clock::now();
//          
//          TransformType::Pointer transform = TransformType::New();
//          transform->SetIdentity();
//          
//          arma::Mat<DataType> outputSlice;
//          
//          arma::uvec size = Utils::armaSize<DataType>(&(inputCube->slice(0)));
//          unsigned int newWidth = (unsigned int) round((double) size[0] * inSpacing[0] / outSpacing[0]);
//          unsigned int newHeight = (unsigned int) round((double) size[1] * inSpacing[1] / outSpacing[1]);
//          itk::Size<2> outputSize = {{newWidth, newHeight}};
//          outputCube->set_size(newWidth, newHeight, inputCube->n_slices);
//          
//          if(resampleType == LINEAR) {
//              typedef itk::LinearInterpolateImageFunction<ITKImageType, double>  InterpolatorType;
//              typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
//              
//              for(unsigned int itS = 0; itS < inputCube->n_slices; itS++) {	    		
//                  inputSliceImage = Utils::armaToITKImage<DataType, 2>(inputCube->slice(itS).memptr(), size);
//                  inputSliceImage->SetSpacing(inSpacing);
//                  inputSliceImage->SetOrigin(inOrigin);
//                  // 		inputSliceImage->SetDirection(inDirection);
//                  typename FilterType::Pointer filter = FilterType::New();
//                  filter->SetTransform(transform);
//                  filter->SetInterpolator(interpolator);
//                  filter->SetOutputOrigin(inOrigin);
//                  filter->SetOutputSpacing(outSpacing);
//                  // 		filter->SetOutputDirection(outDirection);
//                  filter->SetSize(outputSize);
//                  filter->SetNumberOfThreads(numOfThreads);
//                  filter->SetInput(inputSliceImage);
//                  filter->Update();
//                  arma::uvec dimensions(2);
//                  for(int i = 0; i < 2; i++)
//                      dimensions(i) = outputSize[i];
//                  Utils::createArmaFromBuffer<DataType>(&outputSlice, filter->GetOutput()->GetBufferPointer(), dimensions, true, false);
//                  outputCube->slice(itS) = outputSlice;
//                  
//                  filter = NULL;                                  
//              }
//              interpolator = NULL;
//          }
//          else if(resampleType == NEAREST) {
//              typedef itk::NearestNeighborInterpolateImageFunction<ITKImageType, double>  InterpolatorType;
//              typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
//              
//              for(unsigned int itS = 0; itS < inputCube->n_slices; itS++) {	    		
//                  inputSliceImage = Utils::armaToITKImage<DataType, 2>(inputCube->slice(itS).memptr(), size);
//                  inputSliceImage->SetSpacing(inSpacing);
//                  inputSliceImage->SetOrigin(inOrigin);
//                  // 		inputSliceImage->SetDirection(inDirection);		
//                  typename FilterType::Pointer filter = FilterType::New();
//                  filter->SetTransform(transform);
//                  filter->SetInterpolator(interpolator);
//                  filter->SetOutputOrigin(inOrigin);
//                  filter->SetOutputSpacing(outSpacing);
//                  // 		filter->SetOutputDirection(outDirection);
//                  filter->SetSize(outputSize);
//                  filter->SetNumberOfThreads(numOfThreads);
//                  filter->SetInput(inputSliceImage);
//                  filter->Update();
//                  arma::uvec dimensions(2);
//                  for(int i = 0; i < 2; i++)
//                      dimensions(i) = outputSize[i];
//                  Utils::createArmaFromBuffer<DataType>(&outputSlice, filter->GetOutput()->GetBufferPointer(), dimensions, true, false);
//                  outputCube->slice(itS) = outputSlice;
//                  
//                  filter = NULL;                 
//              }	
//              interpolator = NULL;
//          }
//          
//          auto end = chrono::high_resolution_clock::now();
//          if(Utils::showTime && logSender)
//              logSender->logText("ResampleCubeFilter " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
//          cout << "ResampleCubeFilter " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << endl;
//          
//          transform = NULL;
//      }       
     
    template <typename DataType> static void ResampleCubeFilter3D(arma::Cube<DataType>* inputCube, arma::Cube<DataType>* outputCube, ResampleTypes resampleType, mitk::Vector3D inSpacing, mitk::Vector3D outSpacing, mitk::Vector3D inOrigin = mitk::Vector3D(0.0), mitk::Vector3D outOrigin = mitk::Vector3D(0.0), itk::ThreadProcessIdType numOfThreads = 1)
    {
        typedef itk::Image< DataType, 3 > ITKImageType;
        typename ITKImageType::Pointer inputImage;
        
        typedef itk::ResampleImageFilter<ITKImageType, ITKImageType>  FilterType;
        typedef itk::AffineTransform< ScalarType, 3 >  TransformType;
        
        auto begin = chrono::high_resolution_clock::now();
        
        TransformType::Pointer transform = TransformType::New();
        transform->SetIdentity();
        
        arma::Mat<DataType> outputSlice;
        
        arma::uvec size = Utils::armaSize<DataType>(inputCube);
        unsigned int newWidth = (unsigned int) round((double) size[0] * inSpacing[0] / outSpacing[0]);
        unsigned int newHeight = (unsigned int) round((double) size[1] * inSpacing[1] / outSpacing[1]);
        unsigned int newDeep = (unsigned int) round((double) size[2] * inSpacing[2] / outSpacing[2]);
        itk::Size<3> outputSize = {{newWidth, newHeight, newDeep}};
        outputCube->set_size(newWidth, newHeight, newDeep);
        
        typedef itk::InterpolateImageFunction<ITKImageType, double>  InterpolatorBaseType;
        typename InterpolatorBaseType::Pointer interpolator;
        if(resampleType == LINEAR) {
            typedef itk::LinearInterpolateImageFunction<ITKImageType, double>  InterpolatorType;
            interpolator = InterpolatorType::New();
        }
        else if(resampleType == NEAREST) {
            typedef itk::NearestNeighborInterpolateImageFunction<ITKImageType, double>  InterpolatorType;
            interpolator = InterpolatorType::New();
        }
        
        inputImage = Utils::armaToITKImage<DataType, 3>(inputCube->memptr(), size);
        inputImage->SetSpacing(inSpacing);
        inputImage->SetOrigin(inOrigin);
        // 		inputSliceImage->SetDirection(inDirection);
        typename FilterType::Pointer filter = FilterType::New();
        filter->SetTransform(transform);
        filter->SetInterpolator(interpolator);
        filter->SetOutputOrigin(outOrigin);
        filter->SetOutputSpacing(outSpacing);
        // 		filter->SetOutputDirection(outDirection);
        filter->SetSize(outputSize);
        filter->SetNumberOfThreads(numOfThreads);
        filter->SetInput(inputImage);
        filter->Update();
        arma::uvec dimensions(3);
        for(int i = 0; i < 3; i++)
            dimensions(i) = outputSize[i];
        Utils::createArmaFromBuffer<DataType>(outputCube, filter->GetOutput()->GetBufferPointer(), dimensions, true, false);
        
        filter = NULL;                                  
        interpolator = NULL;
        transform = NULL;

        auto end = chrono::high_resolution_clock::now();
        if(Utils::showTime && logSender)
            logSender->logText("ResampleCubeFilter3D " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
        cout << "ResampleCubeFilter3D " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << endl;                
    }     
     
     static void setLogSender(LogSender* senderLogs) { logSender = senderLogs ;} 
     
private:
    static LogSender* logSender; 
};

#endif // IMAGEFILTERS_H
