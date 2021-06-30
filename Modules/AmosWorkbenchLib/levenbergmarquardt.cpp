#include "levenbergmarquardt.hpp"
#include <math.h>
#include <float.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

LevenbergMarquardt::LevenbergMarquardt()
{
    this->nu = 2;
    this->tau = 0.001;
    this->epsilon1 = 1.0e-12;
    this->epsilon2 = 1.0e-12;
    this->maxIter = 100;
    this->iter = 0;
}

LevenbergMarquardt::LevenbergMarquardt(double nu, double tau, double epsilon1, double epsilon2, int maxIter)
{
    this->nu = nu;
    this->tau = tau;
    this->epsilon1 = epsilon1;
    this->epsilon2 = epsilon2;
    this->maxIter = maxIter;
}

LevenbergMarquardt::~LevenbergMarquardt()
{
}

void LevenbergMarquardt::getResults(double *a, double *mu, double *sigma, double *r)
{
    *a = this->a;
    *mu = this->mu;
    *sigma = this->sigma;
    *r = this->r;
}

void LevenbergMarquardt::getMeanAndStd(double *mu, double *sigma)
{
    *mu = this->mu;
    *sigma = this->sigma;
}

void LevenbergMarquardt::getInitials(double *a0, double *mu0, double *sigma0)
{
    *a0 = this->a0;
    *mu0 = this->mu0;
    *sigma0 = this->sigma0;
}

void LevenbergMarquardt::execute(double *sampleX, double *sampleY, int sampleSize)
{
    this->sampleSize = sampleSize;
    
    sampleX = (double*) malloc(sizeof(double) * sampleSize);
    sampleY = (double*) malloc(sizeof(double) * sampleSize);

    for(int i = 0; i < sampleSize; i++) {
        this->sampleX[i] = sampleX[i];
        this->sampleY[i] = sampleY[i];
    }
    
    execute();
}

void LevenbergMarquardt::execute()
{
    reserveMem();

    initGuess();
    a = a0;
    mu = mu0;
    sigma = sigma0;

    iter = 0;
    errors(a, mu, sigma, err);
    jacobian();
    product(err, J, 1, sampleSize, 3, g);
    hessian();
    lambda = tau * maxAbs(diagH, 3);
    double n1 = maxAbs(g[0], 3);
    bool stop = n1 <= epsilon1;
    while(!stop && iter < maxIter) {
        ++iter;
        do {
            inverse();
            product(g, invH, 1, 3, 3, delta);
            double n2 = norm2(delta[0], 3);
            double n3 = epsilon2 * sqrt(a * a + mu * mu + sigma * sigma);
            stop = n2 <= n3;
            if(stop)
                break;

            aNew = a + delta[0][0];
            muNew = mu + delta[0][1];
            sigmaNew = sigma + delta[0][2];

            errors(aNew, muNew, sigmaNew, errNew);
            double err2 = norm2(err[0], sampleSize) * norm2(err[0], sampleSize);
            double errNew2 = norm2(errNew[0], sampleSize) * norm2(errNew[0], sampleSize);
            transpose(delta, 1, 3, deltat);
            product(delta, deltat, 1, 3, 1, delta2);
            transpose(g, 1, 3, gt);
            product(delta, gt, 1, 3, 1, deltaG);
            rho = (err2 - errNew2) / (lambda * delta2[0][0] + deltaG[0][0]);
            if(rho >= 0) {
                ssr = errNew2;
                a = aNew;
                mu = muNew;
                sigma = sigmaNew;

                for(int i = 0; i < sampleSize; i++)
                    err[0][i] = errNew[0][i];
                jacobian();
                product(err, J, 1, sampleSize, 3, g);
                hessian();

                n1 = maxAbs(g[0], 3);
                stop = n1 <= epsilon1;
                if(stop)
                    break;
                lambda *= fmax(1.0/3.0, 1 - pow(2 * rho - 1, 3));
                nu = 2;
            }
            else {
                ssr = err2;
                lambda *= nu;
                nu *= 2;
            }
        } while (rho < 0 /*|| !stop*/);
    }

    calculateCorrelCoeff();

    clearMem();
}

void LevenbergMarquardt::reserveMem()
{
//     sampleX = (double*) malloc(sizeof(double) * sampleSize);
//     sampleY = (double*) malloc(sizeof(double) * sampleSize);

    J = (double**) malloc(sizeof(double*) * sampleSize);
    for(int i = 0; i < sampleSize; i++)
        J[i] = (double*) malloc(sizeof(double) * 3);

    Jt = (double**) malloc(sizeof(double*) * 3);
    for(int i = 0; i < 3; i++)
        Jt[i] = (double*) malloc(sizeof(double) * sampleSize);

    H = (double**) malloc(sizeof(double*) * 3);
    for(int i = 0; i < 3; i++)
        H[i] = (double*) malloc(sizeof(double) * 3);

    diagH = (double*) malloc(sizeof(double) * 3);

    invH = (double**) malloc(sizeof(double*) * 3);
    for(int i = 0; i < 3; i++)
        invH[i] = (double*) malloc(sizeof(double) * 3);

    cofactor = (double**) malloc(sizeof(double *) * 2);
    for (int i = 0; i < 2; i++)
        cofactor[i] = (double*) malloc(sizeof(double) * 2);

    g = (double**) malloc(sizeof(double*));
    g[0] = (double*) malloc(sizeof(double) * 3);

    gt = (double**) malloc(sizeof(double*) * 3);
    for (int i = 0; i < 3; i++)
        gt[i] = (double*) malloc(sizeof(double));

    delta = (double**) malloc(sizeof(double*));
    delta[0] = (double*) malloc(sizeof(double) * 3);

    deltat = (double**) malloc(sizeof(double*) * 3);
    for (int i = 0; i < 3; i++)
        deltat[i] = (double*) malloc(sizeof(double));

    delta2 = (double**) malloc(sizeof(double*));
    delta2[0] = (double*) malloc(sizeof(double));

    deltaG = (double**) malloc(sizeof(double*));
    deltaG[0] = (double*) malloc(sizeof(double));

    err = (double**) malloc(sizeof(double*));
    err[0] = (double*) malloc(sizeof(double) * sampleSize);

    errNew = (double**) malloc(sizeof(double*));
    errNew[0] = (double*) malloc(sizeof(double) * sampleSize);

}

void LevenbergMarquardt::clearMem()
{
    free(sampleX);
    free(sampleY);

    for (int i = 0; i < sampleSize; i++)
       free(J[i]);
    free(J);

    for (int i = 0; i < 3; i++)
       free(Jt[i]);
    free(Jt);

    for (int i = 0; i < 3; i++)
       free(H[i]);
    free(H);

    free(diagH);

    for (int i = 0; i < 3; i++)
       free(invH[i]);
    free(invH);

    for (int i = 0; i < 2; i++)
       free(cofactor[i]);
    free(cofactor);

    free(g[0]);
    free(g);

    for (int i = 0; i < 3; i++)
       free(gt[i]);
    free(gt);

    free(delta[0]);
    free(delta);

    for (int i = 0; i < 3; i++)
       free(deltat[i]);
    free(deltat);

    free(delta2[0]);
    free(delta2);

    free(deltaG[0]);
    free(deltaG);

    free(err[0]);
    free(err);

    free(errNew[0]);
    free(errNew);
}

void LevenbergMarquardt::jacobian() {
    // calculate Jacobian    
    double expo;

    for(int i = 0; i < sampleSize; i++) {
        expo = exp(-(sampleX[i] - mu)*(sampleX[i] - mu)/(2 * sigma*sigma));
        J[i][0] = expo;
        J[i][1] = expo * a * (sampleX[i] - mu) / (sigma*sigma);
        J[i][2] = expo * a * (sampleX[i] - mu)*(sampleX[i] - mu)/(sigma*sigma*sigma);
    }

//    for(int i = 0; i < sampleSize; i++)
//        cout << "J["  << i << "] = (" << J[i][0] << ", " << J[i][1] << ", " << J[i][2] <<")"<< endl;
//    cout << endl;

}


void LevenbergMarquardt::product(double **A, double **B, int rows1, int cols1_rows2, int cols2, double **prod)
{
    for(int k = 0; k < rows1; k++) {
        for(int j = 0; j < cols2; j++) {
            prod[k][j] = 0;
            for(int i = 0; i < cols1_rows2; i++) {
                prod[k][j] += A[k][i] * B[i][j];
            }
        }
    }
}

void LevenbergMarquardt::transpose(double **A, int rows, int cols, double **At) {

   for(int i = 0; i < cols; i++) {
      for(int j = 0; j < rows; j++)
          At[i][j] = A[j][i];
   }
}

void LevenbergMarquardt::hessian() {
    // calculate Hessian

    transpose(J, sampleSize, 3, Jt);
    product(Jt, J, 3, sampleSize, 3, H);
    for(int i = 0; i < 3; i++)
        diagH[i] = H[i][i];

//    for(int i = 0; i < 3; i++)
//        cout << "H["  << i << "] = (" << H[i][0] << ", " << H[i][1] << ", " << H[i][2] <<")"<< endl;
//    cout << endl;

}

double LevenbergMarquardt::determinant(double **mat, int n) {
    double det = 0;
    if(n == 2)
        det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    else if(n == 3)
        det = mat[0][0] * mat[1][1] * mat[2][2] + mat[1][0] * mat[2][1] * mat[0][2] +
                    mat[0][1] * mat[1][2] * mat[2][0] - mat[0][2] * mat[1][1] * mat[2][0] -
                    mat[0][1] * mat[1][0] * mat[2][2] - mat[2][1] * mat[1][2] * mat[0][0];
    return det;
}

void LevenbergMarquardt::inverse() {
    for(int i = 0; i < 3; i++)
        H[i][i] += lambda;
    double detH = determinant(H, 3);
//    if(detH == 0)
//        return invH;

    int i, i1,j1;
    double det;

    for (int j = 0; j < 3; j++) {
       for (i = 0; i < 3; i++) {

          /* Form the adjoint a_ij */
          i1 = 0;
          for (int ii = 0;ii < 3; ii++) {
             if (ii == i)
                continue;
             j1 = 0;
             for (int jj = 0; jj < 3; jj++) {
                if (jj == j)
                   continue;
                cofactor[i1][j1] = H[ii][jj];
                j1++;
             }
             i1++;
          }

          /* Calculate the determinant */
          det = determinant(cofactor, 2);

          /* Fill in the elements of the cofactor */
          invH[j][i] = pow(-1.0,i+j+2.0) * det / detH;
       }
    }
}

double LevenbergMarquardt::mean(double *sampleX, int sampleSize) {
    double sum = 0;
    for(int i = 0; i < sampleSize; i++)
        sum += sampleX[i];
    return sum / sampleSize;
}

double LevenbergMarquardt::maxAbs(double *sampleX, int sampleSize) {
   double max = 0;
   double absVal;
   for(int i = 0; i < sampleSize; i++) {
       absVal = fabs(sampleX[i]);
       max = max < absVal ? absVal : max;
   }
   return max;
}


void LevenbergMarquardt::errors(double a, double mu, double sigma, double **e)
{
    for(int i = 0; i < sampleSize; i++)
        e[0][i] = sampleY[i] - (a*exp(-(sampleX[i] - mu)*(sampleX[i] - mu)/(2 * sigma*sigma)));
}

double LevenbergMarquardt::norm2(double *vect, int size) {
    double sum = 0;
    for(int i = 0; i < size; i++)
        sum += vect[i] * vect[i];

    return sqrt(sum);
}

void LevenbergMarquardt::initGuess() {
    a0 = maxAbs(sampleY, sampleSize);
    double *dotProdXY = (double*) malloc(sizeof(double) * sampleSize);
    for(int i = 0; i < sampleSize; i++)
        dotProdXY[i] = sampleX[i] * sampleY[i];
    meanY = mean(sampleY, sampleSize);
    double meanXY = mean(dotProdXY, sampleSize);
    if(meanY <= DBL_EPSILON || meanXY <= DBL_EPSILON)
        mu0 = 1.0;
    else
        mu0 = meanXY / meanY;

    double *dotProdX2Y = (double*) malloc(sizeof(double) * sampleSize);
    for(int i = 0; i < sampleSize; i++)
        dotProdX2Y[i] = (sampleX[i] - mu0) * (sampleX[i] - mu0) * sampleY[i];
    double meanX2Y = mean(dotProdX2Y, sampleSize);
    if(meanY <= DBL_EPSILON || meanX2Y <= DBL_EPSILON)
        sigma0 = 1.0;
    else
        sigma0 = sqrt(meanX2Y / meanY);

    free(dotProdXY);
    free(dotProdX2Y);
}

void LevenbergMarquardt::calculateCorrelCoeff()
{
    double sse = 0;
    for(int i = 0; i < sampleSize; i++) {
        double y = a*exp(-(sampleX[i] - mu)*(sampleX[i] - mu)/(2 * sigma*sigma)) - meanY;
        sse += y * y;
    }
    r = sqrt(sse / (sse + ssr));
}
