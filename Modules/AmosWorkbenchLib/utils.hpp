#ifndef UTILS_H
#define UTILS_H

#include <AmosWorkbenchLibExports.h>

#include "logsender.h"
#include "dataorientation.h"

#include <mitkImage.h>
#include <itkImage.h>
#include <itkImageIOBase.h>
#include <itkMatrix.h>
#include <mitkImageCast.h>
#include <mitkIOUtil.h>
#include <mitkITKImageImport.h>
#include <mitkImageWriteAccessor.h>
#include <mitkImageReadAccessor.h>
using namespace mitk;

#include <itkImportImageFilter.h>

#include <iostream>
#include <typeinfo>
#include <string>
#include <chrono>

using namespace std;

#define ARMA_64BIT_WORD
// #define ARMA_NO_DEBUG
// #define ARMA_DONT_USE_WRAPPER
#include <armadillo>
//using namespace arma;

#include <QList>

class QTableWidgetItem;

class AmosWorkbenchLib_EXPORT Utils{
public: 
    
    
    // Code for testing limitsData3D(), in Matlab ind has to be ind + 1 and results will be D + 1
    //     Cube<int> A;
    //     A.zeros(9, 8, 7);
    //     uvec ind;
    //     ind << 245 << 247 << 255 << 256 << 264 << 265 << 272 << 274 << 317 << 320 << 326 << 328 << 336 << 338 << 344 << 347 << 379 << 391 << 398 << 408 << 409 << 417;
    //     A.elem(ind).ones();
    //     Mat<int> D = limitsData3D(A);
    //     cout << D << endl;
    
    static bool showTime;
    static ScalarType epsQform;
    
    enum Segmentation_Type {Non_Amos, Amos_AC, Amos_BC};
    enum ImageTypes {FLAIR, T1, LABEL, MASK, EXPERT, ALL};
    
    struct AmosImageData {
        QString parentPath;
        QString path;
        QString name;
        AmosImageData() : parentPath(""), path(""), name("") {}
    };
    
    // used for implementation of MatLab [row, col] find()
    inline static arma::umat indexesToRowsCols(arma::uvec indexes, arma::uword num_rows)
    {
        arma::umat RowsCols(indexes.n_elem, 2);
        indexes = indexes + 1;
        RowsCols.col(1) = floor(indexes / num_rows); // cols
        arma::uvec ooo = indexes - num_rows * RowsCols.col(1);
        arma::uvec ooo1 = find(ooo == 0);
        arma::uvec rc1 = RowsCols.col(1);
        rc1.elem(ooo1) -= 1;
        RowsCols.col(1) = rc1;
        RowsCols.col(0) = indexes - num_rows * RowsCols.col(1) - 1; // rows
        return RowsCols;
    }
    
    inline static arma::umat indexesTo3DSLice(arma::uvec indexes, arma::uword num_rows, arma::uword slice = 0)
    {
        arma::umat points(indexes.n_elem, 3);
        indexes = indexes + 1;
        points.col(1) = floor(indexes / num_rows); // cols
        points.col(0) = indexes - num_rows * points.col(1) - 1; // rows
        points.col(2) = arma::ones<arma::uvec>(indexes.n_elem) * slice;
        return points;
    }    
    
    typedef itk::Image< float, 3> ITKFloatImageType3D;
    typedef itk::Image< unsigned char,  3 > ITKBinaryImageType3D;
    typedef itk::Image<float, 2 > ITKFloatImageType2D;
    typedef itk::Image< unsigned char,  2 > ITKBinaryImageType2D;
    
    template <typename ItkOutputImageType> static void saveITKImage(ItkOutputImageType* itkimage, string outputFilename)
    {
        Image::Pointer mitkImage = Image::New();
        mitkImage = GrabItkImageMemory(itkimage);	
        IOUtil::Save(mitkImage, outputFilename);    
    }
    
    template <typename DataType> static arma::Cube<DataType> createCubeFromMITKImage(Image::Pointer image, bool writeAccess = true, bool copy_aux_mem = false, bool strict = true, bool create = true)
    {
        arma::Cube<DataType> cube;
        mitk::PixelType chPixelType = image->GetPixelType(0);
        bool sameType = false;
        bool round = false;
        if(chPixelType.GetNumberOfComponents() > 1) {
            cout << "createCubeFromMITKImage image has to contain only one component" << endl;
        }
        int pixelType = chPixelType.GetComponentType();
        string type = chPixelType.GetComponentTypeAsString();
        switch(pixelType) {
            case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE : 
                break;
            case itk::ImageIOBase::UCHAR : 
                sameType = is_same<uchar, DataType>::value;
                if(!sameType) {
                    arma::Cube<uchar> inCube;
                    inCube = createCubeFromImageAccesor<uchar>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		    
                break;
            case itk::ImageIOBase::CHAR : 
                sameType = is_same<char, DataType>::value;
                if(!sameType) {
                    arma::Cube<char> inCube;
                    inCube = createCubeFromImageAccesor<char>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;
            case itk::ImageIOBase::USHORT : 
                sameType = is_same<ushort, DataType>::value;
                if(!sameType) {
                    arma::Cube<ushort> inCube;
                    inCube = createCubeFromImageAccesor<ushort>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;
            case itk::ImageIOBase::SHORT : 
                sameType = is_same<short, DataType>::value;
                if(!sameType) {
                    arma::Cube<short> inCube;
                    inCube = createCubeFromImageAccesor<short>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;
            case itk::ImageIOBase::UINT : 
                sameType = is_same<uint, DataType>::value;
                if(!sameType) {
                    arma::Cube<uint> inCube;
                    inCube = createCubeFromImageAccesor<uint>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;
            case itk::ImageIOBase::INT : 
                sameType = is_same<int, DataType>::value;
                if(!sameType) {
                    arma::Cube<int> inCube;
                    inCube = createCubeFromImageAccesor<int>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;
            case itk::ImageIOBase::ULONG : 
                sameType = is_same<ulong, DataType>::value;
                if(!sameType) {
                    arma::Cube<ulong> inCube;
                    inCube = createCubeFromImageAccesor<ulong>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;
            case itk::ImageIOBase::LONG : 
                sameType = is_same<long, DataType>::value;
                if(!sameType) {
                    arma::Cube<long> inCube;
                    inCube = createCubeFromImageAccesor<long>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;
            case itk::ImageIOBase::FLOAT : 
                sameType = is_same<float, DataType>::value;
                round = is_integral<DataType>::value;
                if(!sameType) {
                    arma::Cube<float> inCube;
                    inCube = createCubeFromImageAccesor<float>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;
            case itk::ImageIOBase:: DOUBLE : 
                sameType = is_same<double, DataType>::value;
                round = is_integral<DataType>::value;
                if(!sameType) {
                    arma::Cube<double> inCube;
                    inCube = createCubeFromImageAccesor<double>(image, writeAccess, copy_aux_mem, strict);
                    castCube2NumType(&inCube, &cube, round, create);
                    return cube;
                }		
                break;		
        }
        
        if(sameType) {
            cube = createCubeFromImageAccesor<DataType>(image, writeAccess, copy_aux_mem, strict);
            return cube;
        }
        return cube;
    }
    
    template <typename InType, typename OutType> static void castCube2NumType(arma::Cube<InType>* cube, arma::Cube<OutType>* outCube, bool round = false,  bool create = true)
    {
        try {
            if(round)
                roundArma(cube, outCube, create);
            else
                copyCastingArma(cube, outCube, create);
            
        }
        catch (mitk::Exception& e){
            cout << e << endl; 
        }      
    }    
    
    template <typename ArmaTypeIn, typename ArmaTypeOut> static void roundArma(ArmaTypeIn* armaIn, ArmaTypeOut* roundedArmaIn, bool create = true)
    {
        if(create)
            roundedArmaIn->copy_size(*armaIn);
        typename ArmaTypeIn::const_iterator a1 = armaIn->begin();
        typename ArmaTypeIn::const_iterator b1 = armaIn->end();
        typename ArmaTypeOut::iterator a2 = roundedArmaIn->begin();
        //typename ArmaTypeOut::const_iterator b2 = roundedArmaIn->end();    
        typename ArmaTypeIn::const_iterator i = a1;
        typename ArmaTypeOut::iterator j = a2;
        
        for( ;i != b1; ++i, ++j) {
            *j = round(*i);
        }
    }    
    
    template <typename DataType> static Image::Pointer cubeToMITKImage(arma::Cube<DataType>* cube)
    {
        Image::Pointer image = Image::New();
        unsigned int dim[3];
        dim[0] = cube->n_rows;
        dim[1] = cube->n_cols;
        dim[2] = cube->n_slices;
        image->Initialize(mitk::MakeScalarPixelType<DataType>(), 3, dim);
        image->SetVolume(cube->memptr());
        return image;	
    }
    
    template <typename DataType> static arma::uvec armaSize(arma::Cube<DataType>* cube) {
        arma::uvec dimensions;
        dimensions << cube->n_rows << cube->n_cols << cube->n_slices;
        return dimensions;
    }
    
    template <typename DataType> static arma::uvec armaSize(arma::Mat<DataType>* matrix) {
        arma::uvec dimensions;
        dimensions << matrix->n_rows << matrix->n_cols;
        return dimensions;
    } 
    
    template <typename ArmaType> static bool isNotZero(ArmaType* armaType) {
        typename ArmaType::const_iterator itt = armaType->begin();
        typename ArmaType::const_iterator ite = armaType->end();
        bool notZero = false;
        while(itt != ite && !notZero) {
            if(*itt != 0)
                notZero = true;
            itt++;
        }
        
        return notZero;
    }
    
    template <typename DataType> inline static void createArmaFromBuffer(arma::Cube<DataType>* cube, DataType* buffer, arma::uvec dimensions, bool copy_aux_mem = false, bool strict = true) {
        *cube = arma::Cube<DataType>(buffer, dimensions(0), dimensions(1), dimensions(2), copy_aux_mem, strict);
    }
    
    template <typename DataType> inline static void createArmaFromBuffer(arma::Mat<DataType>* matrix, DataType* buffer, arma::uvec dimensions, bool copy_aux_mem = false, bool strict = true) {
        *matrix = arma::Mat<DataType>(buffer, dimensions(0), dimensions(1), copy_aux_mem, strict);
    }
    
    inline static arma::Mat<unsigned char> inverseArma(arma::Mat<unsigned char> matrix)
    {
        arma::Mat<unsigned char>Ones(matrix.n_rows, matrix.n_cols);
        arma::Mat<unsigned char> inverseMatrix = Ones.ones() - matrix;
        return inverseMatrix;
    }
    
    inline static arma::Cube<unsigned char> inverseArma(arma::Cube<unsigned char> cube)
    {
        arma::Cube<unsigned char>Ones(cube.n_rows, cube.n_cols, cube.n_slices);
        arma::Cube<unsigned char> inverseCube = Ones.ones() - cube;
        return inverseCube;
    } 
    
    inline static arma::Mat<unsigned char> orArma(arma::Mat<unsigned char> matrix1, arma::Mat<unsigned char> matrix2)
    {
        arma::Mat<unsigned char> result = matrix1 + matrix2;
        Utils::logicalArma<unsigned char>(&result);
        return result;
    }
    
    inline static arma::Cube<unsigned char> orArma(arma::Cube<unsigned char> cube1, arma::Cube<unsigned char> cube2)
    {
        arma::Cube<unsigned char> result = cube1 + cube2;
        Utils::logicalArma<unsigned char>(&result);
        return result;
    } 
    
    template <typename DataType> inline static void logicalArma(arma::Mat<DataType> *inputMatrix, arma::Mat<unsigned char> *outputMatrix = 0)
    {
        arma::uvec ind = find(*inputMatrix);
        if(outputMatrix) {
            outputMatrix->copy_size(*inputMatrix);
            outputMatrix->zeros();
            outputMatrix->elem(ind).ones();
        }
        else
            inputMatrix->elem(ind).ones();
    }
    
    template <typename DataType> inline static void logicalArma(arma::Cube<DataType> *inputCube, arma::Cube<unsigned char> *outputCube = 0)
    {
//         arma::uvec ind = find(*inputCube > 1);
        arma::uvec ind = find(*inputCube);
        if(outputCube) {
            outputCube->copy_size(*inputCube);
            outputCube->zeros();
            outputCube->elem(ind).ones();
        }
        else	
            inputCube->elem(ind).ones();
    }    
    
    template <typename DataType, int Dimension> static typename itk::Image<DataType, Dimension >::Pointer armaToITKImage(DataType* memPointer, arma::uvec dimensions)
    {
        typedef typename itk::Image<  DataType, Dimension >::Pointer ImagePointer;
        typedef typename itk::ImportImageFilter< DataType, Dimension > ImportFilterType;
        typedef typename ImportFilterType::Pointer ImportFilterTypePtr;
        ImportFilterTypePtr importFilter = ImportFilterType::New();
        typedef typename ImportFilterType::SizeType  sizeFilter;
        sizeFilter size;
        for(int i = 0; i < Dimension; i++)
            size[i] = dimensions(i);
        
        typedef typename ImportFilterType::IndexType startFilter;
        startFilter start;
        start.Fill(0);
        typedef typename ImportFilterType::RegionType regionFilter;
        regionFilter region;
        region.SetIndex(start);
        region.SetSize(size);
        importFilter->SetRegion(region);
        itk::SpacePrecisionType origin[Dimension];
        for(int i = 0; i < Dimension; i++)
            origin[i] = 0.0;		
        importFilter->SetOrigin(origin);
        itk::SpacePrecisionType spacing[Dimension];
        for(int i = 0; i < Dimension; i++)
            spacing[i] = 1.0;	
        importFilter->SetSpacing(spacing);
        const bool importImageFilterWillOwnTheBuffer = false;
        int imageSize = 1;
        for(int i = 0; i < Dimension; i++)
            imageSize *= size[i];	
        importFilter->SetImportPointer(memPointer, imageSize, importImageFilterWillOwnTheBuffer);
        importFilter->Update();
        ImagePointer outputImage = importFilter->GetOutput();
        importFilter = NULL;
        return outputImage;
    }
    
    template <typename ArmaTypeIn, typename ArmaTypeOut> static void binarizeArma(ArmaTypeIn* armaIn, ArmaTypeOut* binaryArmaIn, float lowerThreshold, float upperThreshold = -1, bool create = false)
    {
        auto begin = chrono::high_resolution_clock::now();
        
        if(create)
            binaryArmaIn->copy_size(*armaIn);
        typename ArmaTypeIn::const_iterator a1 = armaIn->begin();
        typename ArmaTypeIn::const_iterator b1 = armaIn->end();
        typename ArmaTypeOut::iterator a2 = binaryArmaIn->begin();
        //typename ArmaTypeOut::const_iterator b2 = binaryArmaIn->end();    
        typename ArmaTypeIn::const_iterator i = a1;
        typename ArmaTypeOut::iterator j = a2;
        
        if(upperThreshold < 0) {
            for( ;i != b1; ++i, ++j) {
                if(*i >= lowerThreshold)
                    *j = 1;
                else
                    *j = 0;
            }            
        }
        else {
            for( ;i != b1; ++i, ++j) {
                if(*i >= lowerThreshold && *i <= upperThreshold)
                    *j = 1;
                else
                    *j = 0;
            }            
        }
        
        auto end = chrono::high_resolution_clock::now();
        if(showTime && logSender) {
            logSender->logText("binarizeArma " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
            // 	    cout << "binarizeArma " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
        }
    }
    
    static Segmentation_Type AmosSegmentationType(const DataNode::Pointer node);
    static Segmentation_Type AmosSegmentationType(const DataNode* node);    
    
    template <typename ArmaTypeIn, typename DataType = float> static void normalizeArma(ArmaTypeIn* armaIn)
    {
        auto begin = chrono::high_resolution_clock::now();
        
        // 	typename ArmaTypeIn::const_iterator a1 = armaIn->begin();
        typename ArmaTypeIn::const_iterator b1 = armaIn->end();  
        typename ArmaTypeIn::iterator i = armaIn->begin();
        
        DataType maxIn = -arma::datum::inf;
        DataType minIn = arma::datum::inf;
        
        for( ;i != b1; ++i) {
            maxIn = *i > maxIn ? *i : maxIn;
            minIn = *i < minIn ? *i : minIn;
        }
        
        DataType range = maxIn - minIn;
        if(range <= vnl_math::eps)
            return;
        
        i = armaIn->begin();
        for( ;i != b1; ++i)
            *i = (*i - minIn) / range;
        
        auto end = chrono::high_resolution_clock::now();
        if(showTime && logSender) {
            logSender->logText("normalizeArma " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
            // 	    cout << "normalizeArma " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
        }
    }    
    
    template <typename DataType> static void transpose(arma::Cube<DataType> *cubeIn, arma::Cube<DataType> *cubeOut, int i, int j, int k, bool copy_aux_mem = true, bool showTime = false)
    {
        auto begin = chrono::high_resolution_clock::now();
        
        bool validIdxs = true;
        int r = cubeIn->n_rows;
        int c = cubeIn->n_cols;
        int s = cubeIn->n_slices;
        
        if(i == 1 && j == 2 && k == 3)
            //        *cubeOut = *cubeIn;
            *cubeOut = arma::Cube<DataType>(cubeIn->memptr(), r, c, s, copy_aux_mem);
        else if(i == 1 && j == 3 && k == 2) {
            cubeOut->set_size(r, s, c);
            for(int n = 0; n < s; n++)
                cubeOut->tube(arma::span(0, r - 1), arma::span(n)) = cubeIn->slice(n);  // 1 3 2
        }
        else if(i == 2 && j == 1 && k == 3) {
            cubeOut->set_size(c, r, s);
            for(int n = 0; n < s; n++)
                cubeOut->slice(n) = cubeIn->slice(n).t();  // 2 1 3
        }
        else if(i == 2 && j == 3 && k == 1) {
            cubeOut->set_size(c, s, r);
            for(int n = 0; n < s; n++)
                cubeOut->tube(arma::span(0, c - 1), arma::span(n)) = cubeIn->slice(n).t(); // 2 3 1
        }
        else if(i == 3 && j == 1 && k == 2) {
            cubeOut->set_size(s, r, c);
            for(int n = 0; n < s; n++)
                cubeOut->tube(arma::span(n), arma::span(0, r - 1)) = cubeIn->slice(n); // 3 1 2
        }
        else if(i == 3 && j == 2 && k == 1) {
            cubeOut->set_size(s, c, r);
            for(int n = 0; n < s; n++)
                cubeOut->tube(arma::span(n), arma::span(0, c- 1)) = cubeIn->slice(n).t(); // 3 2 1
        }
        else {
            if(logSender)
                logSender->logText( "Transpose invalid indexes " + QString::number(i) + " " + QString::number(j) + " " + QString::number(k) + " )");
            validIdxs = false;
        }
        
        if(validIdxs && showTime && logSender) {
            auto end = chrono::high_resolution_clock::now();
            logSender->logText( "Transpose ( " + QString::number(i) + " " + QString::number(j) + " " + QString::number(k) + " ) " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end - begin).count()/1000000.0) + "ms");
        }
    }    
    
    
    template <typename DataType> static void reorder(arma::Cube<DataType> *cubeIn, arma::Cube<DataType> *cubeOut, int* dims, bool showTime = false)
    {
        auto begin = chrono::high_resolution_clock::now();
        
        int r = cubeIn->n_rows;
        int c = cubeIn->n_cols;
        int s = cubeIn->n_slices;
        
        cubeOut->set_size(r, c, s);
        if(dims[0] < 0 || dims[0] > 1 || dims[1] < 0 || dims[1] > 1 || dims[2] < 0 || dims[2] > 1) {
            cout << "Reorder invalid dimensions, values has to be 0 or 1" << endl;
            if(logSender)
                logSender->logText( "Reorder invalid dimensions, values has to be 0 or 1");	    
        }
        
        int sumDims = dims[0] + dims[1] + dims[2];
        if(!sumDims)
            *cubeOut = *cubeIn;
        bool copy_aux_mem = (sumDims > 1);
        arma::Cube<DataType> tmpCube(cubeIn->memptr(), r, c, s, copy_aux_mem);
        
        int cnt = 0;
        for (int i = 0; i < 3; i++) {
            if(dims[i] == 0)
                continue;
            else {
                if(cnt > 0)
                    tmpCube = *cubeOut;
                cnt++;
            }
            
            if(i == 0) {
                arma::Cube<DataType> tmpCube2;
                tmpCube2.set_size(r, c, s);
                //        for(int n = 0; n < r; n++) {
                //            (*cubeOut)(arma::span(n), arma::span(0, c - 1), arma::span(0, s - 1)) = (*cubeIn)(arma::span(r - 1 - n), arma::span(0, c - 1), arma::span(0, s - 1)); // approx. 3.5 times slower
                transpose(&tmpCube, cubeOut, 2 , 1, 3);
                int dim[] = {0, 1, 0};
                reorder(cubeOut, &tmpCube2, dim);
                transpose(&tmpCube2, cubeOut, 2 , 1, 3);
                //        }
            }
            else if(i == 1) {
                for(int n = 0; n < c; n++)
                    (*cubeOut)(arma::span(0, r - 1), arma::span(n), arma::span(0, s - 1)) = tmpCube(arma::span(0, r - 1), arma::span(c - 1 - n), arma::span(0, s - 1));
            }
            else if(i == 2) {
                for(int n = 0; n < s; n++)
                    cubeOut->slice(n) = tmpCube.slice(s - 1 - n);
            }
        }
        
        if(showTime && logSender) {
            auto end = chrono::high_resolution_clock::now();
            logSender->logText( "Reorder " + QString::number(dims[0]) + " " + QString::number(dims[1]) + " " + QString::number(dims[2]) + " " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end - begin).count()/1000000.0) + "ms");
        }
    }    
    
    template <typename ArmaTypeIn, typename ArmaTypeOut> static void copyCastingArma(ArmaTypeIn* armaIn, ArmaTypeOut* changedArmaIn, bool create = true)
    {
        if(create)
            changedArmaIn->copy_size(*armaIn);
        typename ArmaTypeIn::const_iterator a1 = armaIn->begin();
        typename ArmaTypeIn::const_iterator b1 = armaIn->end();
        typename ArmaTypeOut::iterator a2 = changedArmaIn->begin();
        //typename ArmaTypeOut::const_iterator b2 = changedArmaIn->end();    
        typename ArmaTypeIn::const_iterator i = a1;
        typename ArmaTypeOut::iterator j = a2;
        
        for( ;i != b1; ++i, ++j) {
            *j = *i;
        }
    }    
    
    enum NiftyOrientation {NIFTI_L2R = 1, NIFTI_R2L, NIFTI_P2A, NIFTI_A2P, NIFTI_I2S, NIFTI_S2I};
    static void nifti_mat_to_orientation(itk::Matrix<ScalarType> R, int *icod, int *jcod, int *kcod);
    static char const *nifti_orientation_string( int ii );
    static ScalarType det(itk::Matrix<ScalarType> R);
    
    static void setLogSender(LogSender* senderLogs) { logSender = senderLogs ;} 
    
    template< typename T = ScalarType, unsigned int NRows = 3, unsigned int NColumns = 3 > static bool aproxEqualITKMatrix(itk::Matrix<T, NRows, NColumns> m1, itk::Matrix<T, NRows, NColumns> m2, T maxeps)
    {
        bool equal = true;
        if(m1.ColumnDimensions != m2.ColumnDimensions || m1.RowDimensions != m2.RowDimensions)
            return false;
        
        for (unsigned int r = 0; r < NRows; r++) {
            for (unsigned int c = 0; c < NColumns; c++) {
                if (abs(m1(r, c) - m2(r, c)) > maxeps) {
                    cout << setprecision(9) << "images qforms difference " << abs(m1(r, c) - m2(r, c)) << endl;
                    equal = false;
                    break;
                }
            }
        }
        return equal;
    }
    
    static void changeGeometry(BaseGeometry* geometry, int* icod, int* jcod, int* kcod, int* reorderI, int* reorderJ, int* reorderK);
    static void revertGeometry(BaseGeometry* geometry, int iq, int jq, int kq, int reorderI, int reorderJ, int reorderK);
    
    template <typename DataType> static void transposeReorderImage(Image::Pointer imageIn, Image::Pointer imageOut, DataOrientation::OrientationData oData)
    {
        arma::Cube<DataType> inCube;
        arma::Cube<DataType> tmpCubeTransposed;		
        arma::Cube<DataType> tmpCubeReordered;
        mitk::BaseGeometry *geometry = imageIn->GetGeometry();
        
        inCube = createCubeFromImageAccesor<DataType>(imageIn, true, false, true);
        Utils::transpose(&inCube, &tmpCubeTransposed, oData.icod, oData.jcod, oData.kcod);	
        int dims[] = {oData.reorderI, oData.reorderJ, oData.reorderK};
        Utils::reorder(&tmpCubeTransposed, &tmpCubeReordered, dims);
        
        imageOut->Initialize(mitk::MakeScalarPixelType<DataType>(), *geometry);
        imageOut->SetVolume(tmpCubeReordered.memptr());	
    }
    
    template <typename DataType> static void revertCubeOrientation(arma::Cube<DataType>* inCube, arma::Cube<DataType>* outCube, int dims[], arma::Col<int> indexes)
    {
        arma::Cube<DataType> tmpCubeReordered;
//         reorder(inCube, &tmpCubeReordered, dims);
//         Utils::transpose(&tmpCubeReordered, outCube, (*indexes)(0), (*indexes)(1), (*indexes)(2));
//         arma::uvec uveci = find(*indexes == 1);
//         arma::uvec uvecj = find(*indexes == 2);
//         arma::uvec uveck = find(*indexes == 3);
//         int i_cod = uveci(0) + 1;
//         int j_cod = uvecj(0) + 1;
//         int k_cod = uveck(0) + 1;
//         indexes->reset();
//         *indexes << i_cod << j_cod << k_cod;
        int i_cod = indexes(0);
        int j_cod = indexes(1);
        int k_cod = indexes(2);        
//         Utils::transpose(&tmpCubeReordered, outCube, i_cod, j_cod, k_cod);
        Utils::transpose(inCube, &tmpCubeReordered, i_cod, j_cod, k_cod);
        reorder(&tmpCubeReordered, outCube, dims);
    }
    
    template <typename DataType> static void revertImageOrientation(Image::Pointer imageIn, Image::Pointer imageOut, DataOrientation::OrientationData oData) 
    {
        DataOrientation::OrientationData invData = invertOData(oData);
        arma::Cube<DataType> inCube;
        arma::Cube<DataType> outCube;		
        mitk::BaseGeometry *geometry = imageIn->GetGeometry();
        
        inCube = createCubeFromImageAccesor<DataType>(imageIn, true, false, true);

        arma::Col<int> indexes;
//         indexes << oData.icod << oData.jcod << oData.kcod;
        indexes << invData.icod << invData.jcod << invData.kcod;
        
//         int* dimsPtr[] = {&(oData.reorderI), &(oData.reorderJ), &(oData.reorderK)};
//         arma::uvec uveci = find(indexes == 1);
//         arma::uvec uvecj = find(indexes == 2);
//         int i = uveci(0);
//         int j = uvecj(0);
//         *(dimsPtr[i]) = *(dimsPtr[i]) == 0 ? 1 : 0;
//         *(dimsPtr[j]) = *(dimsPtr[j]) == 0 ? 1 : 0;
        
//         oData.reorderI = oData.reorderI == 0 ? 1 : 0;
//         oData.reorderJ = oData.reorderJ == 0 ? 1 : 0;
        /*int dims[] = {oData.reorderI, oData.reorderJ, oData.reorderK}; */
        int dims[] = {invData.reorderI, invData.reorderJ, invData.reorderK}; 
        
        revertCubeOrientation(&inCube, &outCube, dims, indexes);
        
//         revertGeometry(geometry, indexes[0], indexes[1], indexes[2], oData.reorderI, oData.reorderJ, oData.reorderK);
        revertGeometry(geometry, invData.icod, invData.jcod, invData.kcod, invData.reorderI, invData.reorderJ, invData.reorderK);
        imageOut->Initialize(mitk::MakeScalarPixelType<DataType>(), *geometry);
        imageOut->SetVolume(outCube.memptr());
    }
    
    static void cubePointerToSegmentation(Image::Pointer image, arma::Cube<unsigned char>* cube);
    
    static QList<int> getRowsForItems(QList<QTableWidgetItem*> items);

    static DataOrientation::OrientationData invertOData(DataOrientation::OrientationData oData);
    
    static size_t getTotalSizeOfFreeRam();
    
private:
    
    template <typename DataType> static arma::Cube<DataType> createCubeFromImageAccesor(Image::Pointer image, bool writeAccess = true, bool copy_aux_mem = false, bool strict = true)
    {
        arma::Cube<DataType> cube;
        try {
            if(writeAccess) {
                ImageWriteAccessor accessor(image);
                void* vPointer = accessor.GetData();       
                DataType *mPointer = (DataType *) vPointer;
                cube = arma::Cube<DataType>(mPointer, image->GetDimension(0), image->GetDimension(1), image->GetDimension(2), copy_aux_mem, strict);			
            }
            else {
                ImageReadAccessor accessor(image);
                const void* vPointer = accessor.GetData();       
                DataType *mPointer = (DataType *) vPointer;
                cube = arma::Cube<DataType>(mPointer, image->GetDimension(0), image->GetDimension(1), image->GetDimension(2), copy_aux_mem, strict);			
            }
        }
        catch (mitk::Exception& e) {
            cout << e << endl;
            return cube;
        }
        return cube;	
    }
    
    static LogSender* logSender;
};

#endif // UTILS_H
