#ifndef REGIONPROPERTIES_H
#define REGIONPROPERTIES_H

#define ARMA_64BIT_WORD
// #define ARMA_NO_DEBUG
// #define ARMA_DONT_USE_WRAPPER
#include <armadillo>
using namespace arma;

#include <vtkSmartPointer.h>
#include <vtkPoints.h>

class RegionProperties
{
public:
    RegionProperties();
    ~RegionProperties();
    
    void setInputMatrix(Mat<unsigned char>* inputMatrix);
    void execute();
    Mat<unsigned char> getConvexImage() { return convexHullImageBinaryMat; }
    Mat<unsigned char>  getConvexOutlineHullBinaryMat() { return convexHullOutlineBinaryMat; }
    Mat<int> getBoundingBox() { return boundsMat; }
    double getConvexHullContourSize() { return convexHullContourSize; }
    long long int getConvexHullSize() { return convexHullSize; }
    
protected:
    void convexHullOutline();  
    void convexImage();
    
private:
    Mat<unsigned char>* inputMatrix;
//     Mat<unsigned char> inputMatrixContour;
    vtkSmartPointer<vtkPoints> pointsVTK;
    Mat<int> boundsMat;
    Mat<unsigned char> convexHullBinaryMat;
    Mat<int> convexHullPixelsMat;
    long long int numConvexHullPoints;
    long long int convexHullSize;
    double convexHullContourSize;
    Mat<unsigned char> convexHullOutlineBinaryMat;
    Mat<unsigned char> convexHullImageBinaryMat;

};

#endif // REGIONPROPERTIES_H
