#ifndef LEVENBERGMARQUARDT_H
#define LEVENBERGMARQUARDT_H

#define ARMA_64BIT_WORD
// #define ARMA_NO_DEBUG
// #define ARMA_DONT_USE_WRAPPER
#include <armadillo>
using namespace arma;

class LevenbergMarquardt
{
public:
    LevenbergMarquardt();
    LevenbergMarquardt(double nu, double tau, double epsilon1, double epsilon2, int maxIter);
    ~LevenbergMarquardt();
    
    void execute(double *sampleX, double *sampleY, int sampleSize);
    void getResults(double *a, double *mu, double *sigma, double *r = 0);
    void getMeanAndStd(double *mu, double *sigma);
    void getInitials(double *a0, double *mu0, double *sigma0);
    int getIter() { return iter; }
    float getRConf() { return rConf; }
    
    template <typename DataType> Col<DataType> fitPeak(Col<DataType> xSample, Col<DataType> ySample, unsigned int nPoints) {
        rConf  = 1.0; // Estimation confidence
        nPoints = nPoints > xSample.n_elem ? xSample.n_elem : nPoints;
        uvec yNon0 = find(ySample > 0);
        nPoints = nPoints > yNon0.n_elem ? yNon0.n_elem : nPoints;
        uvec ind = sort_index(ySample, "descend");
        // Is it really a peak? maxima are not very separated
        uvec indSel = ind(span(0, nPoints - 1));
//         ind.save("ind.mat", arma_ascii);
        uvec indSelSort = sort(indSel);
        uvec distNeighbour = diff(indSelSort);
        ind = find(distNeighbour > 4);
        if(ind.n_elem > 0) {
            rConf = 0.5;
            // MatLab code under revision
        }
        
        sampleSize = nPoints;
        if(indSel.n_elem > 0) {
            // Selects points
//             indSel.save("indSel.mat", arma_ascii);
//             xSample.save("xSample.mat", arma_ascii);
//             ySample.save("ySample.mat", arma_ascii);
            Col<DataType> xS = xSample.elem(indSel);
            Col<DataType> yS = ySample.elem(indSel);
            sampleX = (double*) malloc(sizeof(double) * sampleSize);
            sampleY = (double*) malloc(sizeof(double) * sampleSize);
            
            for(int i = 0; i < sampleSize; i++) {
                sampleX[i] = xS(i);
                sampleY[i] = yS(i);
            }
        }
        
        execute();
        
        Col<DataType> meanStd(2);
        meanStd(0) = (DataType) mu;
        meanStd(1) = (DataType) sigma;
        return meanStd;
    }
    
protected:
    void execute();
    
private:
    void reserveMem();
    void clearMem();
    
    void errors(double a, double mu, double sigma, double **e);
    
    void jacobian();
    void hessian();
    void inverse();
    
    void product(double **A, double **B, int rows1, int cols1_rows2, int cols2, double **prod);
    void transpose(double **A, int rows, int cols, double **At);
    double determinant(double **mat, int n);
    
    double mean(double *sampleX, int sampleSize);
    
    double maxAbs(double *sampleX, int sampleSize);
    double norm2(double *vect, int size);
    
    void initGuess();
    
    void calculateCorrelCoeff();
    
private:
    double nu, tau, epsilon1, epsilon2;
    int maxIter;
    
    double *sampleX, *sampleY;
    int sampleSize;
    
    double a0, mu0, sigma0, a, mu, sigma, aNew, muNew, sigmaNew;
    double lambda, rho;
    int iter;
    float rConf;
    
    double **err;
    double **errNew;
    double **J;
    double ** Jt;
    double **H;
    double *diagH;
    double **invH;
    double **cofactor;
    double **g;
    double **gt;
    double **delta;
    double **deltat;
    double **delta2;
    double **deltaG;
    
    double r;
    double ssr;
    double meanY;
    
};

#endif // LEVENBERGMARQUARDT_H
