#ifndef TESTS_H
#define TESTS_H

#include "utils.hpp"
// using namespace arma;

using namespace std;

class Tests
{
public:
    static void testIndexOfPixels2D();
    static void testIndexOfPixels3D();
    template <typename ArmaType, typename DataType, int Dimension> static void testIndexOfPixels(ArmaType* armaType, arma::Mat<unsigned int> pixels)
    {
	arma::uvec/*::fixed<Dimension>*/ imgDimensions = Utils::armaSize<DataType>(armaType);
	arma::uvec indexOfPixels;
	indexOfPixels.zeros(pixels.n_rows);
	arma::uvec index = find(*armaType);

	for(long unsigned int i = 0; i < pixels.n_rows; i++) {
	    
	    for(int j = 0; j < Dimension; j++) {		   
		if(j == 0)
		    indexOfPixels(i) += pixels(i, j);
		else if(j == 1)
		    indexOfPixels(i) += pixels(i, j) * imgDimensions(j - 1);
		else if(j == 2)
		    indexOfPixels(i) += pixels(i, j) * imgDimensions(j - 1) * imgDimensions(j - 2);		
	    }
	}
	
	
	cout << sort(indexOfPixels).t();
	cout << sort(find(*armaType)).t();
	arma::uvec result = find(sort(indexOfPixels) - sort(find(*armaType)));
	cout << "testIndexOfPixels" << Dimension << "D " << result.n_elem << endl;
    }
};

#endif // TESTS_H
