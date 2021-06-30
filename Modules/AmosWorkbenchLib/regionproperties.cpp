#include "regionproperties.h"
#include "imagefilters.hpp"

#include <vtkConvexHull2D.h>
#include <vtkMath.h>
#include <itkLineIterator.h>

RegionProperties::RegionProperties()
{
    pointsVTK = vtkSmartPointer< vtkPoints >::New();
    numConvexHullPoints = 0;
    convexHullSize = 0;
    convexHullContourSize = 0;
}

RegionProperties::~RegionProperties()
{

}

void RegionProperties::setInputMatrix(Mat<unsigned char>* inputMatrix)
{
    this->inputMatrix = inputMatrix;
    Mat<unsigned char> inputMatrixContour;    
//     ImageFilters::BinaryContourMatrixFilter2D(inputMatrix, &inputMatrixContour);
    ImageFilters::BinaryContourArmaFilter<Mat<unsigned char>, unsigned char, 2>(inputMatrix, &inputMatrixContour);
//     inputMatrixContour.save("IdilateContourAmos.mat", arma_ascii); 
//     umat points = Utils::indexesTo3DSLice(find(inputMatrixContour == 1), inputMatrix->n_rows);
//     pointsVTK->SetNumberOfPoints(points.n_rows);
//     for(unsigned int i = 0; i < points.n_rows; i++) {
//         pointsVTK->SetPoint(i, points(i, 0), points(i, 1), points(i, 2));
//     }
    
    umat points = ind2sub(size(inputMatrixContour), find(inputMatrixContour == 1));
    pointsVTK->SetNumberOfPoints(points.n_cols);
    for(long long unsigned int i = 0; i < points.n_cols; i++) {
        pointsVTK->SetPoint(i, points(0, i), points(1, i), 0);
    }
    
}

void RegionProperties::execute()
{    
    convexHullSize = 0;
    convexHullContourSize = 0;
    
    vtkSmartPointer<vtkPoints> convexHullPoints = vtkSmartPointer<vtkPoints>::New();
    vtkConvexHull2D::CalculateConvexHull(pointsVTK, convexHullPoints);
//     convexHullPoints->Print(cout);
    double* bounds = convexHullPoints->GetBounds();    
//     boundsMat << (int)round(bounds[0]) << (int)round(bounds[2]) << (int)round(bounds[4]) << endr << (int)round(bounds[1]) << (int)round(bounds[3]) << (int)round(bounds[5]) << endr;
    boundsMat << (int)bounds[0] << (int)bounds[2] << (int)bounds[4] << endr << (int)bounds[1] << (int)bounds[3] << (int)bounds[5] << endr;
//     cout << boundsMat;
    
    numConvexHullPoints = convexHullPoints->GetNumberOfPoints();
    convexHullBinaryMat = zeros< Mat<unsigned char> >(inputMatrix->n_rows, inputMatrix->n_cols);
    convexHullPixelsMat.set_size(numConvexHullPoints, 2);
    
    double x0, y0, /*z0,*/ x1, y1/*, z1*/;
    for(int i = 0; i < numConvexHullPoints; i++) {
        x0 = convexHullPoints->GetPoint(i)[0];
        y0 = convexHullPoints->GetPoint(i)[1];
    // 	z0 = convexHullPoints->GetPoint(i)[2];
        
//         convexHullPixelsMat(i, 0) = (int) round(x0);
//         convexHullPixelsMat(i, 1) = (int) round(y0);
        convexHullPixelsMat(i, 0) = (int) floor(x0);
        convexHullPixelsMat(i, 1) = (int) floor(y0);        
        convexHullBinaryMat(convexHullPixelsMat(i, 0), convexHullPixelsMat(i, 1)) = 1;
        
        int j = (i + 1) % numConvexHullPoints;
        x1 = convexHullPoints->GetPoint(j)[0];
        y1 = convexHullPoints->GetPoint(j)[1];
    // 	z1 = convexHullPoints->GetPoint(j)[2];		    
        convexHullContourSize += sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
    }
//     cout << endl;
//     convexHullBinaryMat.save("ConvexHull.mat", arma_ascii);
    convexHullOutline();
    convexImage();
	
}

void RegionProperties::convexHullOutline()
{
/*    const unsigned int Dimension = 3;
    typedef double CoordType;
    typedef itk::Point< CoordType, Dimension > PointType; */   
    
    convexHullOutlineBinaryMat = convexHullBinaryMat;
    uvec size = Utils::armaSize<unsigned char>(&convexHullOutlineBinaryMat);
    Utils::ITKBinaryImageType2D::Pointer convexHullOutlineBinaryITK = Utils::armaToITKImage<unsigned char, 2>(convexHullOutlineBinaryMat.memptr(), size);

    Utils::ITKBinaryImageType2D::IndexType pixelIndex1;
    Utils::ITKBinaryImageType2D::IndexType pixelIndex2;
    
    for(int i = 0; i < numConvexHullPoints; i++) {
        int j = (i + 1) % numConvexHullPoints;
        pixelIndex1[0] = convexHullPixelsMat(i, 0);
        pixelIndex1[1] = convexHullPixelsMat(i, 1);
        pixelIndex2[0] = convexHullPixelsMat(j, 0);
        pixelIndex2[1] = convexHullPixelsMat(j, 1);
        
        itk::LineIterator<Utils::ITKBinaryImageType2D> it(convexHullOutlineBinaryITK, pixelIndex1, pixelIndex2);
        it.GoToBegin();
        while (!it.IsAtEnd()) {	    
            it.Set(1);
            ++it;	    
        }
    }
    
//     convexHullOutlineBinaryMat.save("ConvexHullOutline.mat", arma_ascii);
}

// void RegionProperties::convexImage()
// {    
//     convexHullImageBinaryMat = zeros< Mat<unsigned char> >(inputMatrix->n_rows, inputMatrix->n_cols);
// //     cout<< endl << outlinePoints;
//     if(numConvexHullPoints > 0) {
//         umat outlinePoints = Utils::indexesToRowsCols(find(convexHullOutlineBinaryMat == 1), convexHullOutlineBinaryMat.n_rows);
//         umat outlinePoints2 = ind2sub(size(convexHullOutlineBinaryMat), find(convexHullOutlineBinaryMat == 1));
//         
//         outlinePoints.save("outlinePoints.mat", arma_ascii); 
//         umat outlinePoints2t = outlinePoints2.t();
//         outlinePoints2t.save("outlinePoints2t.mat", arma_ascii);
//         
//         int colMin = min(outlinePoints.col(1));
//         int colMax = max(outlinePoints.col(1));
//         for(int i = colMin; i <= colMax; i++) {
//             uvec linePoints = find(outlinePoints.col(1) == i);
//         // 	cout << linePoints.t();
//             int rowMin = outlinePoints(min(linePoints), 0);
//             int rowMax = outlinePoints(max(linePoints), 0);
//             for(int j = rowMin; j <= rowMax; j++) 
//                 convexHullImageBinaryMat(j, i) = 1;
//             convexHullSize += rowMax - rowMin + 1;
//         }
//     }
//      convexHullImageBinaryMat.save("convexHullImageBinaryMat-old.mat", arma_ascii);
// }

void RegionProperties::convexImage()
{    
    convexHullImageBinaryMat = zeros< Mat<unsigned char> >(inputMatrix->n_rows, inputMatrix->n_cols);
//     cout<< endl << outlinePoints;
    if(numConvexHullPoints > 0) {
        umat outlinePoints = ind2sub(size(convexHullOutlineBinaryMat), find(convexHullOutlineBinaryMat == 1));
        int colMin = min(outlinePoints.row(1));
        int colMax = max(outlinePoints.row(1));
        for(int i = colMin; i <= colMax; i++) {
            uvec linePoints = find(outlinePoints.row(1) == i);
        // 	cout << linePoints.t();
            int rowMin = outlinePoints(0, min(linePoints));
            int rowMax = outlinePoints(0, max(linePoints));
            for(int j = rowMin; j <= rowMax; j++) 
                convexHullImageBinaryMat(j, i) = 1;
            convexHullSize += rowMax - rowMin + 1;
        }
    }
//     convexHullImageBinaryMat.save("convexHullImageBinaryMat.mat", arma_ascii);    
}
