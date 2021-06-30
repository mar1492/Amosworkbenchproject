#include "tests.hpp"

void Tests::testIndexOfPixels2D()
{
    const int Dimension = 2;
    arma::Mat<unsigned int> A;    
    A.zeros(9, 8);
    arma::Mat<unsigned int> pixels(A.n_elem, Dimension);
    unsigned int iP = -1;
        
    for(unsigned int i = 0; i < A.n_rows; i++) {
	for(unsigned int j = 0; j < A.n_cols; j++) {
	    if(i % 3 == 0 && j % 2 == 0) {
		iP++;
		A(i, j) = 1;
		arma::Row<unsigned int> pixel;
		pixel << i << j;
		pixels.row(iP) = pixel;
		cout << pixel;
	    }	
	}	
    }
    cout << endl;
    cout << A;
    cout << endl;
    pixels.resize(++iP, Dimension);

    testIndexOfPixels< arma::Mat<unsigned int>, unsigned int, Dimension >(&A, pixels);  
}

void Tests::testIndexOfPixels3D()
{
    const int Dimension = 3;    
    arma::Cube<unsigned int> A;    
    A.zeros(9, 8, 7);
    arma::Mat<unsigned int> pixels(A.n_elem, Dimension);
    unsigned int iP = -1;
    for(unsigned int i = 0; i < A.n_rows; i++) {
	for(unsigned int j = 0; j < A.n_cols; j++) {
	    for(unsigned int k = 0; k < A.n_slices; k++) {
		if(i % 4 == 0 && j % 3 == 0 && k % 2 == 0) {
		    iP++;
		    A(i, j, k) = 1;
		    arma::Row<unsigned int> pixel;
		    pixel << i << j << k;
		    pixels.row(iP) = pixel;
		    cout << pixel;		    
		}
	    }	
	}	
    }
    cout << endl;
    pixels.resize(++iP, Dimension);

    testIndexOfPixels< arma::Cube<unsigned int>, unsigned int, Dimension >(&A, pixels);
      
}