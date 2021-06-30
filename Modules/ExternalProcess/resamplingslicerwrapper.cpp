#include "resamplingslicerwrapper.h"

int ResamplingSlicerWrapper::getAmosProcessRange()
{
    return 5;
}

ResamplingSlicerWrapper::ResamplingSlicerWrapper(QObject* parent) : ExternalProcessWrapper(parent)
{
    amosProcessRange = getAmosProcessRange();
    doBiasCorrection = true;
    LBResampling = true;
}

ResamplingSlicerWrapper::~ResamplingSlicerWrapper()
{
    cout << "ResamplingSlicerWrapper deleted" << endl;
}

void ResamplingSlicerWrapper::setOutputPaths(QDir outputDirectory, QStringList outputPaths)
{
    this->outputDirectory = outputDirectory;
    if(doBiasCorrection && LBResampling) {
        flairBCPath = outputPaths.at(0);
        T1BCPath = outputPaths.at(1);
        T1RSFLPath = outputPaths.at(2);
        T1RSFLPath = outputPaths.at(2);
        LBRSFLPath = outputPaths.at(3);
        TXPath = outputPaths.at(4);        
    }
    else if(doBiasCorrection && !LBResampling) {
        flairBCPath = outputPaths.at(0);
        T1BCPath = outputPaths.at(1);
        T1RSFLPath = outputPaths.at(2);
        TXPath = outputPaths.at(3);        
    }
    else if(!doBiasCorrection && LBResampling) {
        T1RSFLPath = outputPaths.at(0);
        LBRSFLPath = outputPaths.at(1);
        TXPath = outputPaths.at(2);        
    } 
    else {
        T1RSFLPath = outputPaths.at(0);
        TXPath = outputPaths.at(1);   
    }
}

QStringList ResamplingSlicerWrapper::getArguments()
{
    QStringList arguments;
    arguments << "-launch";
    int countRealProcess;
        
    if(doBiasCorrection)
        countRealProcess = countProcess;
    else {
        // starting at BRAINSFit
        countRealProcess = countProcess + 2;
        if( countProcess == 1) {
            flairBCPath = imageFiles.at(0).absoluteFilePath();
            T1BCPath = imageFiles.at(1).absoluteFilePath();
        }
    }
    
    if(countRealProcess == 1 || countRealProcess == 2) {
        QString N4Path("N4ITKBiasFieldCorrection");
        arguments << N4Path;
        if(countRealProcess == 1) {
            arguments << imageFiles.at(0).absoluteFilePath() << flairBCPath;
            emit sendProgressType(tr("Flair Bias Corr."));
        }
        else if(countRealProcess == 2) {
            arguments << imageFiles.at(1).absoluteFilePath() << T1BCPath;
            emit sendProgressType(tr("T1 Bias Corr."));
        }
    } 
    else if(countRealProcess == 3) {
        QString BRAINSFit("BRAINSFit");
        arguments << BRAINSFit;

        arguments << "--fixedVolume" << flairBCPath;
        arguments << "--movingVolume" << T1BCPath;
        arguments << "--linearTransform" << TXPath;
        arguments << "--samplingPercentage" << QString::number(0.8);
        if(LBResampling) {
            arguments << "--maskProcessingMode" << "ROI";
            arguments << "--movingBinaryVolume" << imageFiles.at(2).absoluteFilePath();
        }
        else
            arguments << "--maskProcessingMode" << "NOMASK";        
        arguments << "--transformType" << "ScaleVersor3D";
        arguments << "--interpolationMode" << "BSpline";
        arguments << "--outputVolumePixelType" << "short";
        arguments << "--numberOfThreads" << QString::number(numThreads);

        emit sendProgressType(tr("Fitting"));
    }
    else if(countRealProcess == 4 || countRealProcess == 5) {
        QString BRAINSResample("BRAINSResample");
        arguments << BRAINSResample;

        if(doBiasCorrection)
            arguments << "--referenceVolume" << flairBCPath;
        else
            arguments << "--referenceVolume" << imageFiles.at(0).absoluteFilePath();        
        arguments << "--warpTransform" << TXPath;
        if(countRealProcess == 4) {
            arguments << "--inputVolume" << T1BCPath;
            arguments << "--outputVolume" << T1RSFLPath;
            arguments << "--interpolationMode" << "BSpline";
            emit sendProgressType(tr("T1 Resampling"));
        }
        else if(countRealProcess == 5) {
            arguments << "--inputVolume" << imageFiles.at(2).absoluteFilePath();
            arguments << "--outputVolume" << LBRSFLPath;
            arguments << "--interpolationMode" << "NearestNeighbor";
            emit sendProgressType(tr("LB Resampling"));
        }
        arguments << "--pixelType" << "short";        
        arguments << "--defaultValue" << QString::number(0);
        arguments << "--numberOfThreads" << QString::number(numThreads);
    }
//     QString arg = arguments.join(" ");
//     cout << arg.toStdString() << endl;
    return arguments;
}

void ResamplingSlicerWrapper::cleanAfterFinished()
{
    QFile file;
    if(doBiasCorrection) {
        file.setFileName(T1BCPath);
        file.remove();
    }
    file.setFileName(TXPath);
    file.remove();
    QFileInfo TX(TXPath);
    QString inverseName = TX.baseName();
    inverseName = inverseName  + "_Inverse.h5";
    QFileInfo TXinverse(outputDirectory, inverseName);
    file.setFileName(TXinverse.absoluteFilePath());
    file.remove();     
}

bool ResamplingSlicerWrapper::setParameters(QMap<QString, QString> parameters)
{
    if(parameters.contains("do bias correction"))
        doBiasCorrection = parameters["do bias correction"] == "true" ? true : false;    
//      if(!doBiasCorrection)
//          amosProcessRange = 3;
    if(parameters.contains("LB resampling"))
        LBResampling = parameters["LB resampling"] == "true" ? true : false; 
    if(parameters.contains("amosProcessRange"))    
        amosProcessRange = parameters["amosProcessRange"].toInt();    
    return true;
}
