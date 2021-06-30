#include "contrastprocesscase.h"

#include "amositkimageio.h"

#include "prompt/error.h"
#include "prompt/parser.h"
#include "prompt/edison.h"

#include <QTimer>
#include <QTextStream>

ContrastProcessCase::ContrastProcessCase(bool modeAutomatic, bool imagesPreviouslyLoaded, bool mode3D) : InternalProcessCase(modeAutomatic, imagesPreviouslyLoaded, mode3D)
{
    saveGradientMap = false;
    saveEdgesMap = false;
    saveConfidenceMap = false;
//     connect(this, SIGNAL(startExecution()), this, SLOT(run()));
}

ContrastProcessCase::~ContrastProcessCase()
{
    cout << QString::number(numCase).toStdString()  + " ContrastProcessCase deleted" << endl;
}

void ContrastProcessCase::clearMemory() {
    InternalProcessCase::clearMemory();
    
    FL3Dcube.reset();
    FL3DROIcube.reset();  
    LB3Dcube.reset();
    enhancedFlairCube.reset();
    orgFL3Dcube.reset();
    flairLB.reset();
    LBbinary.reset();
    pgmDataCube.reset();
    gradientMapData.reset();
    edgesMapCube.reset();
    
    orgFL3D = NULL;   
    orgLB3D = NULL;
    orgMASK3D = NULL;    
    
    FL3D = NULL;
}

void ContrastProcessCase::mainLoop2D(int /*startSlice*/, int /*endSlice*/, int /*threadIndex*/)
{

}

bool ContrastProcessCase::verifySizes()
{
    unsigned int dimFL = orgFL3D->GetDimension();
    unsigned int dimLB = orgLB3D->GetDimension();
    unsigned int dimMASK = orgMASK3D ? orgMASK3D->GetDimension() : 3;
    if(dimFL != 3 ||  dimLB != 3 || dimMASK != 3) {
        emit sendLog(QString::fromLatin1("Images are not 3D"));
        //         cout  << "Images are not 3D" << endl;
        return false;
    }
    
    unsigned int sizesFL[3];
    unsigned int sizesLB[3];
    unsigned int sizesMASK[3];
    dimImg3D = (unsigned int*) malloc(sizeof(unsigned int) * 3);
    
    for(int i = 0; i < 3; i++) {
        sizesFL[i] = orgFL3D->GetDimension(i);
        sizesLB[i] = orgLB3D->GetDimension(i);
        sizesMASK[i] = orgMASK3D ? orgMASK3D->GetDimension(i) : orgFL3D->GetDimension(i);
        if(sizesFL[i] != sizesLB[i] || sizesFL[i] != sizesMASK[i]) {
            emit sendLog(QString::fromLatin1("Images have not the same sizes"));
            //             cout  << "Images have not the same sizes" << endl;
            return false;
        }
        dimImg3D[i] = sizesFL[i];
    }
    
    return true;
}

bool ContrastProcessCase::loadImages(QList< Image::Pointer > imagesMITK)
{
    orgFL3D = imagesMITK.at(0);
    orgLB3D = imagesMITK.at(1);
    orgMASK3D = imagesMITK.at(2);
    if(orgMASK3D)
        convertProgressRange++;
        // MASK not included in listTypes because it is not processed in convertImages()
        // it is processed directly in convertImageMaskAndLB()    
    return true;
}

void ContrastProcessCase::computeIROI3D()
{

}

void ContrastProcessCase::startMainLoop3D()
{
    
}

void ContrastProcessCase::preMainLoop()
{

}

/*
void ContrastProcessCase::nonLoopExecute()
{   

    caseProgress = 1;
    progressType = tr("Starting Edison");
    progressRange = 3;
    changeProgres();    
            
    writePGMData(SLICE_INI_LOOP, SLICE_END_LOOP); 
    int heigth = FL3Dcube.n_rows;
    int width = FL3Dcube.n_cols;
    
    EDISON edison;
    edison.SetParametersNew(parameters);
    edisons.append(edison);
    edisonNum = 0;
    // width and height have to be changed
    gradientMapData.set_size(PGMheigth * PGMwidth);
    
    caseProgress = 2;
    progressType = tr("Computing gradient");
    changeProgres();
//         QTimer::singleShot(0, this, SLOT(changeProgres()));    
    
    for(int i = startSlice; i <= endSlice; i++) {
        cout << "Slice " << i << endl;
        // width and height have to be changed
        edison.setInputImage(pgmDataCube.slice(i).memptr(), width, heigth, 1);
        edison.EdgeDetectNew(&gradientMapDataPart[i * heigth * width]);
    }


    caseProgress = 3;
    progressType = tr("Enhancing contrast");
    changeProgres();
//     QTimer::singleShot(0, this, SLOT(changeProgres())); 
    
    enhanceContrast();

}
*/

void ContrastProcessCase::saveResults()
{
//     caseProgress = 4;
//     progressType = tr("Saving results");
//     progressRange = 1;
//     QTimer::singleShot(0, this, SLOT(changeProgres()));    

    QFileInfo fileE(outputDirectory, enhancedName);           
    enhancedFlair_Image->SetVolume(enhancedFlairCube.memptr()); 
    
    emit sendSaveImage(enhancedFlair_Image, fileE);
    if(saveEdgesMap) {
        QFileInfo fileED(outputDirectory, edisonFiles.edgeMap);                        
        edgesMapImage = Image::New();
        edgesMapImage->Initialize(mitk::MakeScalarPixelType<uchar>(), *geometryFL);
        edgesMapImage->SetVolume(edgesMapCube.memptr());
        emit sendSaveImage(edgesMapImage, fileED);
    }
    if(saveGradientMap) {
        QFileInfo fileED(outputDirectory, edisonFiles.gradientMap);                        
        CmCWriteMFile(fileED.absoluteFilePath().toLatin1().data(), gradientMapDataPart, dimImg3D[0] * dimImg3D[2], dimImg3D[1], 1);
    }        
    if(saveConfidenceMap) {
        QFileInfo fileED(outputDirectory, edisonFiles.confidenceMap);                        
        CmCWriteMFile(fileED.absoluteFilePath().toLatin1().data(), confidenceMapDataPart, dimImg3D[0] * dimImg3D[2], dimImg3D[1], 1);
    }        
}

bool ContrastProcessCase::setParameters(QMap< QString, QString > parameters)
{
    this->parameters = parameters;
    
    edisonParams.GradientWindowRadius = parameters["GradientWindowRadius"].toInt();
    edisonParams.MinimumLength = parameters["MinimumLength"].toInt();
    edisonParams.NmxRank = parameters["NmxRank"].toDouble();
    edisonParams.NmxConf = parameters["NmxConf"].toDouble();
//     edisonParams.NmxType = parameters["NmxType"];
    edisonParams.NmxType = parameters["NmxType"].toInt();
    edisonParams.HysterisisHighRank = parameters["HysterisisHighRank"].toDouble();
    edisonParams.HysterisisHighConf = parameters["HysterisisHighConf"].toDouble();
//     edisonParams.HysterisisHighType = parameters["HysterisisHighType"];
    edisonParams.HysterisisHighType = parameters["HysterisisHighType"].toInt();
    edisonParams.CustomCurveHystHigh[0][0] = parameters["CustCurveHysterisisHigh_00"].toDouble();
    edisonParams.CustomCurveHystHigh[0][1] = parameters["CustCurveHysterisisHigh_01"].toDouble();
    edisonParams.CustomCurveHystHigh[1][0] = parameters["CustCurveHysterisisHigh_10"].toDouble();    
    edisonParams.CustomCurveHystHigh[1][1] = parameters["CustCurveHysterisisHigh_11"].toDouble();    
    edisonParams.HysterisisLowRank = parameters["HysterisisLowRank"].toDouble();
    edisonParams.HysterisisLowConf = parameters["HysterisisLowConf"].toDouble();
//     edisonParams.HysterisisLowType = parameters["HysterisisLowType"];
    edisonParams.HysterisisLowType = parameters["HysterisisLowType"].toInt();
    edisonParams.CustomCurveHystLow[0][0] = parameters["CustCurveHysterisisLow_00"].toDouble();
    edisonParams.CustomCurveHystLow[0][1] = parameters["CustCurveHysterisisLow_01"].toDouble();
    edisonParams.CustomCurveHystLow[1][0] = parameters["CustCurveHysterisisLow_10"].toDouble();    
    edisonParams.CustomCurveHystLow[1][1] = parameters["CustCurveHysterisisLow_11"].toDouble();
    saveEdgesMap = parameters["saveEdgesMap"].toInt();
    saveConfidenceMap = parameters["saveConfidenceMap"].toInt(); 
    saveGradientMap = parameters["saveGradientMap"].toInt();
    return true;
}

void ContrastProcessCase::setOutputNames(QList< QString > outputNames)
{
    enhancedName = outputNames.at(0); 
    gradientName = outputNames.at(1); 
    QFileInfo gradInfo(QDir(outputDirectory), gradientName);
    edisonFiles.gradientMap = gradInfo.absoluteFilePath();
    edisonFiles.edgeMap = outputNames.at(2);
    QFileInfo edgesInfo(QDir(outputDirectory), edisonFiles.edgeMap);
    edisonFiles.edgeMap = edgesInfo.absoluteFilePath();
    edisonFiles.confidenceMap = outputNames.at(3);
    QFileInfo confInfo(QDir(outputDirectory), edisonFiles.confidenceMap);
    edisonFiles.confidenceMap = confInfo.absoluteFilePath();    
}

void ContrastProcessCase::initHeap()
{

}

bool ContrastProcessCase::calculateOrientation()
{
    geometryFL = orgFL3D->GetGeometry();
    // TODO call Initialize dependig pixelType template see AmosItkImageIO::Write()
    //const mitk::PixelType pixelType = orgFL3D->GetPixelType();
    if(modeAutomatic && !imagesPreviouslyLoaded) {
        enhancedFlair_Image = Image::New();
        enhancedFlair_Image->Initialize(mitk::MakeScalarPixelType<float>(), *geometryFL);
    }
    AffineTransform3D* transform = geometryFL->GetIndexToWorldTransform();
    itk::Matrix<ScalarType> qformFL = transform->GetMatrix();
    
    BaseGeometry* geometry = orgLB3D->GetGeometry();
    transform = geometry->GetIndexToWorldTransform();
    itk::Matrix<ScalarType> qformLB = transform->GetMatrix();
    
    itk::Matrix<ScalarType> qformMASK;
    if(orgMASK3D) {
        geometry = orgMASK3D->GetGeometry();
        transform = geometry->GetIndexToWorldTransform();
        qformMASK = transform->GetMatrix();
    }
    else
        qformMASK = qformFL;
    
    if(!(Utils::aproxEqualITKMatrix(qformFL, qformLB, Utils::epsQform) && Utils::aproxEqualITKMatrix(qformFL, qformMASK, Utils::epsQform))) {
        // 	// emit sendLog(subjectName + " images qforms are different");
        // emit sendLog(subjectName + " " + tr("images qforms are different, skipping"));	
        emit sendDifferentQforms(subjectName);			
        return false;
    }
  
//     Utils::changeGeometry(geometryFL, &icod, &jcod, &kcod, &reorderI, &reorderJ, &reorderK);
    geometryFLChanged = true;
    BoundingBox::BoundsArrayType bounds = geometryFL->GetBounds();
    for(int i = 0; i < 3; i++) 
        dimImg3D[i] = bounds[2*i + 1] - bounds[2*i];
    
    if(modeAutomatic && imagesPreviouslyLoaded) { // after geometry has changed
        enhancedFlair_Image = Image::New();
        enhancedFlair_Image->Initialize(mitk::MakeScalarPixelType<float>(), *geometryFL);
    }    
   
    return true;
}

void ContrastProcessCase::createCubes()
{
//     caseProgress = 1;
//     progressType = tr("Converting");
//     progressRange = convertProgressRange;
//     QTimer::singleShot(0, this, SLOT(changeProgres()));         
//     convertCounter = 0;
    
    orgFL3Dcube = Utils::createCubeFromMITKImage<float>(orgFL3D, false);
    emit sendProgress(1);
    LB3Dcube = Utils::createCubeFromMITKImage<unsigned short>(orgLB3D, false);
    emit sendProgress(2);
    
    bool create = true;
    Utils::binarizeArma< Cube<unsigned short>, Cube<unsigned char> >(&LB3Dcube, &LBbinary, 1, -1.f, create);
    FL3Dcube = Utils::createCubeFromMITKImage<float>(orgFL3D, false);


    flairLB = FL3Dcube % LBbinary; 
    Utils::normalizeArma(&flairLB);
    pgmDataCube.set_size(arma::size(flairLB));
    arma::fcube flairLB255 = flairLB*255;
    Utils::roundArma(&flairLB255, &pgmDataCube);

//     Utils::normalizeArma(&FL3Dcube);
//     flairLB = FL3Dcube % LBbinary;     
//     Utils::roundArma(&flairLB, &pgmDataCube);    
//     pgmDataCube = pgmDataCube * 255;
    
//     flairLB.save("flairLB.cub", arma_ascii);
    
    gradientMapData.set_size(pgmDataCube.n_elem);
    gradientMapDataPart = gradientMapData.memptr();  
    if(saveConfidenceMap) {
        confidenceMapData.set_size(pgmDataCube.n_elem);
        confidenceMapDataPart = confidenceMapData.memptr();         
    }
    else
        confidenceMapDataPart = nullptr;
    
    enhancedFlairCube = zeros<fcube>(dimImg3D[0], dimImg3D[1], dimImg3D[2]);
    if(saveEdgesMap) {
        edgesMapCube = zeros< Cube<uchar> >(dimImg3D[0], dimImg3D[1], dimImg3D[2]);
        edgesMapDataPart = edgesMapCube.memptr();
    }
    else
        edgesMapDataPart = nullptr;
}

void ContrastProcessCase::nonLoopExecute()
{
    caseProgress = 1;
    progressType = tr("Starting Edison");
    progressRange = 1;
    changeProgres();    
    
/*    createCubes(); */    
    
    SLICE_INI_LOOP = 0;
    SLICE_END_LOOP = FL3Dcube.n_slices - 1;
}

void ContrastProcessCase::execute()
{
    
    if(stop) {
    //     	stop = false;
        clearMemory();
        return;
    }
  
/*    caseProgress = 1;
    progressType = tr("Converting");
    progressRange = convertProgressRange;
    changeProgres();*/  
  
    if(!calculateOrientation()) {
        clearMemory();
//         executionFinished(false);
        finishedOk = false;
//         quit();
        stop = true;
        return;
    }
 
    createCubes();        
    
    SLICE_INI_LOOP = 0;
    SLICE_END_LOOP = FL3Dcube.n_slices - 1;
    
    if(stop) {
        clearMemory();
        return;
    } 
    
    if(!mode3D) { 
        caseProgress = 2;
        progressType = tr("Computing gradient");
        progressRange = SLICE_END_LOOP - SLICE_INI_LOOP + 1;
        changeProgres();        
        computeGradient(SLICE_INI_LOOP, SLICE_END_LOOP); 
        enhanceContrast();       
    }    
    else {
    }    
}

void ContrastProcessCase::computeGradient(int startSlice, int endSlice)
{
//     auto begin = chrono::high_resolution_clock::now();
    int heigth = FL3Dcube.n_rows;
    int width = FL3Dcube.n_cols;
        
    /*EDISON* edison = edisons[edisonNum++];*/    
    EDISON edison = edisons[edisonNum++];
//     startSlice = 17;
//     endSlice = 17;

    for(int i = startSlice; i <= endSlice; i++) {
        //int ii = 17;
//         cout << "Slice " << i << endl;
        // width and height have to be changed
         edison.setInputImage(pgmDataCube.slice(i).memptr(), width, heigth, 1);
        // Testing similarity witj PGM original version
//         arma::Mat<uchar> slice = pgmDataCube.slice(i).t();
//         edison.setInputImage(slice.memptr(), heigth, width, 1);
         
        if(saveEdgesMap && saveConfidenceMap)
            edison.EdgeDetectNew(&gradientMapDataPart[i * heigth * width], &edgesMapDataPart[i * heigth * width], &confidenceMapDataPart[i * heigth * width]);
        else if(saveEdgesMap)
            edison.EdgeDetectNew(&gradientMapDataPart[i * heigth * width], &edgesMapDataPart[i * heigth * width], nullptr);
        else if(saveConfidenceMap)
            edison.EdgeDetectNew(&gradientMapDataPart[i * heigth * width], nullptr,  &confidenceMapDataPart[i * heigth * width]);
        else
            edison.EdgeDetectNew(&gradientMapDataPart[i * heigth * width], nullptr,  nullptr);
        
        emit sendProgress(++sliceCounter);
    }
//     pgmDataCube.save("pgmDataCube.cub", arma_ascii);    
//     gradientMapData.save("gradientMapData.vec", arma_ascii);
//     edgesMapCube.save("edgesMapCube.cub", arma_ascii);
    
/*    auto end = chrono::high_resolution_clock::now();
    cout << "Edison " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;*/        
    
//     caseProgress = 2;
//     progressType = tr("Computing gradient");
//     changeProgres();
/*        QTimer::singleShot(0, this, SLOT(changeProgres())); */ 
}

bool ContrastProcessCase::setImages(int numCase, QList<Image::Pointer> imagesMITK, QString caseName, int SLICE_INI_LOOP, int SLICE_END_LOOP)
{
    bool ret = InternalProcessCase::setImages(numCase, imagesMITK, caseName, SLICE_INI_LOOP, SLICE_END_LOOP);
    convertProgressRange = 0;
    if(ret && !stop) {
        emit startExecution();
    }
    return ret;
}

void ContrastProcessCase::enhanceContrast()
{
   cout << "enhanceContrast " << endl;
   
    caseProgress = 3;
    progressRange = 1;
    progressType = tr("Enhancing contrast");
    changeProgres();   
    
    uint bins_g = 1024;
    uint bins_i = 1024;

    arma::fvec im2edisonVec = arma::vectorise(flairLB);
    //gradientMapData.zeros();
    arma::umat N = hist3(&gradientMapData, &im2edisonVec, bins_g, bins_i);    
    N.col(0) = arma::zeros<arma::uvec>(bins_g);
    arma::umat NC = cumsum(cumsum(N, 0), 1);

    float max_iwm =im2edisonVec.max();
    arma::fvec i_l = arma::linspace<arma::fvec>(0, max_iwm, bins_i);
    arma::fvec g_l = arma::linspace<arma::fvec>(0, 1, bins_g);
    arma::fvec p = arma::zeros<arma::fvec>(bins_g);
    for(arma::uword gi = 0; gi < bins_g; gi++) {
        unsigned long long sz = sum(NC.col(gi));
        if(sz == 0)
            p(gi) = 0;
        else {
            float sm = sum(NC.col(gi) % g_l);
            p(gi) = sm/static_cast<float>(sz);
        }
    }

    arma::fvec P = cumsum(p % i_l);

    for(arma::uword its = 0; its < FL3Dcube.n_slices; its++) {
        for(arma::uword i=0; i < FL3Dcube.n_rows; i++) {
            for(arma::uword j=0; j < FL3Dcube.n_cols; j ++) {
//                ulong ind = static_cast<ulong>(roundf(im2pgm(i + its * FL3Dcube->n_rows, j) * bins_g));
//                 ulong ind = static_cast<ulong>(roundf(im2edison(i + its * FL3Dcube.n_rows, j) * bins_g));
                ulong ind = static_cast<ulong>(roundf(flairLB(i, j, its) * bins_g));
                if(ind == 0)
                    enhancedFlairCube(i, j, its) = 0;
                else
                    enhancedFlairCube(i, j, its) = (P(ind - 1));
            }
        }
    }

    float maxFL = orgFL3Dcube.max();
    float minFL = orgFL3Dcube.min();
    float minEnhancedFlairCube = enhancedFlairCube.min();
    float maxEnhancedFlairCube = enhancedFlairCube.max();
    enhancedFlairCube = (enhancedFlairCube - minEnhancedFlairCube) / (maxEnhancedFlairCube - minEnhancedFlairCube)*(maxFL - minFL) + minFL;
    arma::uvec ind = find(LB3Dcube == 0);
    enhancedFlairCube(ind) = orgFL3Dcube(ind);
    
}

arma::umat ContrastProcessCase::hist3(arma::fvec *X, arma::fvec *Y, uint n_binsX, uint n_binsY)
{
    arma::umat histogram;
    if(X->n_elem != Y->n_elem) {
        std::cout << "Vectors with different number of elements" << std::endl;
        return histogram;
    }

    histogram.zeros(n_binsX, n_binsY);
    arma::uword num_elems = X->n_elem;
    float minX = X->min();
    float maxX = X->max();
    float minY = Y->min();
    float maxY = Y->max();
    float lengthX = maxX - minX;
    float lengthY = maxY - minY;
    uint indexX, indexY;
    for(arma::uword i = 0; i < num_elems; i++) {
        indexX = static_cast<uint>(truncf((X->at(i) - minX) * n_binsX / lengthX));
        indexY = static_cast<uint>(truncf((Y->at(i) - minY) * n_binsY / lengthY));
        if(indexX == n_binsX)
            --indexX;
        if(indexY == n_binsY)
            --indexY;
        histogram(indexX, indexY) += 1;
    }

    return histogram;
}
