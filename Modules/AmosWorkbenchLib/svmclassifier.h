#ifndef SVMCLASSIFIER_H
#define SVMCLASSIFIER_H

#include "libsvm/svm.h"

#define ARMA_64BIT_WORD
// #define ARMA_NO_DEBUG
// #define ARMA_DONT_USE_WRAPPER
#include <armadillo>
using namespace arma;

class SVMClassifier
{
public:
    SVMClassifier();
    ~SVMClassifier();
    
    void setModel(const char * modelname);
    void setData(fvec data);
    vec predict();
    
protected:
    struct svm_model* model;
    struct svm_node * dataNode;  // features vector container
    
    char * modelname;
    int classesNumber;
    int nr_features;    
};

#endif // SVMCLASSIFIER_H
