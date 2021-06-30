#include "svmclassifier.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


SVMClassifier::SVMClassifier()
{
    dataNode = 0;
    model = 0;
}

SVMClassifier::~SVMClassifier()
{
    if(dataNode)
        free(dataNode);
    if(model)
        free(model);
}

void SVMClassifier::setModel(const char * modelname)
{
    if((model = svm_load_model(modelname)) == 0) {
        cout << "Can't open model file " << modelname << endl;
        return;
    }
    classesNumber = model->nr_class;
}

void SVMClassifier::setData(fvec data)
{
    nr_features = data.n_elem + 1;
    dataNode = (struct svm_node *) malloc(nr_features*sizeof(struct svm_node));
    for(int i = 0; i < nr_features - 1; i++) {
        dataNode[i].index = i + 1;
        dataNode[i].value = data(i);
    }
    dataNode[nr_features - 1].index = -1;
}

vec SVMClassifier::predict()
{
    vec result;
    if(!model)
        return result;
    
    double *prob_estimates = (double *) malloc(classesNumber*sizeof(double));
    double predict_label = svm_predict_probability(model, dataNode, prob_estimates);
    
    result = zeros<vec>(classesNumber + 1);
    result(0) = predict_label;
    for(int i = 1; i <= classesNumber; i++)
        result(i) = prob_estimates[i - 1];
    
    free(prob_estimates);
    free(dataNode);
    dataNode = 0;
    return result;
}
