#include "resamplingslicerlauncher.h"
#include "resamplingslicerwrapper.h"

ResamplingSlicerLauncher::ResamplingSlicerLauncher(QObject* parent) : ExternalProcessLauncher(parent)
{
    amosProcessRange = ResamplingSlicerWrapper::getAmosProcessRange(); // Flair_BiasCorrection T1_BiasCorrection Fitting T1_Resampling LB_Resampling
    numImageFilesIn = 3;
    numImageFilesOut = 3;
    doBiasCorrection = true;
    LBResampling = true;
}

ResamplingSlicerLauncher::~ResamplingSlicerLauncher()
{    
}

ExternalProcessWrapper* ResamplingSlicerLauncher::newProcess()
{
    return new ResamplingSlicerWrapper();
}

void ResamplingSlicerLauncher::setNumThreads(int numThreads)
{
    this->numThreads = numThreads;
    useParallel = false;
}

void ResamplingSlicerLauncher::setNumSimultaneousSubjects(int /*numSimultaneousSubjects*/)
{
    this->numSimultaneousSubjects = 1;
}

bool ResamplingSlicerLauncher::setParameters(QMap<QString, QString> parameters)
{
/*    QString programPath = parameters.at(0);
    QFileInfo slicerExe(programPath, "Slicer");
    m_program = slicerExe.absoluteFilePath();  */
    if(!parameters.contains("do bias correction") || !parameters.contains("LB resampling"))
        return false;
    doBiasCorrection = parameters["do bias correction"] == "true" ? true : false;
    if(!doBiasCorrection) {
        amosProcessRange = amosProcessRange - 2;
        numImageFilesOut = numImageFilesOut - 1;        
    }
    LBResampling = parameters["LB resampling"] == "true" ? true : false; 
    if(!LBResampling) {
        --amosProcessRange;
        --numImageFilesIn;        
        --numImageFilesOut;
    }    
    this->parameters["do bias correction"] = doBiasCorrection ? "true" : "false";
    this->parameters["LB resampling"] = LBResampling ? "true" : "false";
    this->parameters["amosProcessRange"] = QString::number(amosProcessRange);
    
    return true;
}

void ResamplingSlicerLauncher::setProgram(QString programPath)
{
    QFileInfo slicerExe(programPath, "Slicer");
    m_program = slicerExe.absoluteFilePath();    
}

bool ResamplingSlicerLauncher::setAmosImagesData(QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListIn, QString outputParentPath, QString outputPath, QList< QPair<Utils::ImageTypes, Utils::AmosImageData> > amosImagesListOut)
{
    this->amosImagesListIn = amosImagesListIn;
    this->amosImagesListOut = amosImagesListOut;
    if(subjectsAsMainDirs)
        OutParentDirectory = QDir(outputParentPath);
    else {
        QFileInfo fileInfo(outputParentPath, outputPath);
        OutParentDirectory = QDir(fileInfo.absoluteFilePath());
    }
    this->outputPath = outputPath;    

    if(!OutParentDirectory.exists()) {
        if(!OutParentDirectory.mkdir(OutParentDirectory.dirName())) {
            emit sendLog(tr("Can not create output directory ") + OutParentDirectory.dirName());
            // 	    cout << "Can not create output directory " << OutParentDirectory.dirName().toStdString() << endl;
            return false;
        }
    }
     
    if(amosImagesListIn.size() < numImageFilesIn) {
        emit sendLog(tr("Not enough input images"));
        return false;
    }     
    if(amosImagesListOut.size() < numImageFilesOut) {
        emit sendLog(tr("Not enough output images"));
        return false;
    }

    FlairDirectory = QDir(amosImagesListIn.at(0).second.parentPath); 
    T1Directory = QDir(amosImagesListIn.at(1).second.parentPath); 
    if(LBResampling)
        LBDirectory = QDir(amosImagesListIn.at(2).second.parentPath);
    else
        LBDirectory = QDir::current(); // always exists

    if(!FlairDirectory.exists() ||!T1Directory.exists() || !LBDirectory.exists()) {
        // emit sendLog(QString::fromLatin1("FlairDirectory or MaskDirectory do not exist"));
        // 	    cout << "FlairDirectory or MaskDirectory do not exist" << endl;
        return false;
    }    
    
    for(int i = 0; i < subjects.size(); i++) {
        if(!subjectsAsMainDirs)
            subdirsFL.append(QFileInfo(FlairDirectory, subjects.at(i)));
        else {
            QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(0).second.path;
            subdirsFL.append(QFileInfo(FlairDirectory, subdir));
        }
    }
    numValidCases = subdirsFL.size(); 
    
    for (int i = 0; i < numValidCases; ++i) {
//         cout << subdirsFlair.at(i).absoluteFilePath().toStdString() << endl;
        if(!subdirsFL.at(i).isDir()) {
            subdirsFL.removeAt(i);
            emit sendMessageToUser(subjects.at(i) + " " + tr("does not contains valid or enough images"));
            subjects.removeAt(i);
            i--;
            numValidCases--;
        }
    }    
    
    for(int i = 0; i < subjects.size(); i++) {
        if(!subjectsAsMainDirs)
            subdirsT1.append(QFileInfo(T1Directory, subjects.at(i)));
        else {
            QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(1).second.path;
            subdirsT1.append(QFileInfo(T1Directory, subdir));
        }
    }
    numValidCases = subdirsT1.size();    
    
    for (int i = 0; i < numValidCases; ++i) {
//         cout << subdirsFlair.at(i).absoluteFilePath().toStdString() << endl;
        if(!subdirsT1.at(i).isDir()) {
            subdirsFL.removeAt(i);
            subdirsT1.removeAt(i);
            emit sendMessageToUser(subjects.at(i) + " " + tr("does not contains valid or enough images"));
            subjects.removeAt(i);
            i--;
            numValidCases--;
        }
    }
    
    if(LBResampling) {
        for(int i = 0; i < subjects.size(); i++) {
            if(!subjectsAsMainDirs)
                subdirsLB.append(QFileInfo(LBDirectory, subjects.at(i)));
            else {
                QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(2).second.path;
                subdirsLB.append(QFileInfo(LBDirectory, subdir));
            }
        } 
    
        for (int i = 0; i < numValidCases; ++i) {
//          cout << subdirsT1.at(i).absoluteFilePath().toStdString() << endl;
            if(!subdirsLB.at(i).isDir()) {
                subdirsFL.removeAt(i);
                subdirsT1.removeAt(i);
                subdirsLB.removeAt(i);
                emit sendMessageToUser(subjects.at(i) + " " + tr("does not contains valid or enough images"));
                subjects.removeAt(i);
                i--;
                numValidCases--;
            }
        }
    }
    
    if(numValidCases == 0) {
        emit sendLog(tr("No valid cases."));
        return false;
    }
    return true;
    
}

void ResamplingSlicerLauncher::appendImageFilesIn(int numCase)
{
    
    QFileInfo fileInfoF(subdirsFL.at(numCase).filePath(), amosImagesListIn.at(0).second.name);	
    // 	    cout << fileInfo.filePath().toStdString() << endl;
    if(fileInfoF.exists()) {
        QString ll = fileInfoF.absoluteFilePath();
        imageFiles.append(fileInfoF);
        // emit sendLog(fileInfo.filePath());
    }     

    QFileInfo fileInfoT(subdirsT1.at(numCase).filePath(), amosImagesListIn.at(1).second.name);	
    // 	    cout << fileInfo.filePath().toStdString() << endl;
    if(fileInfoT.exists()) {
        QString ll = fileInfoT.absoluteFilePath();
        imageFiles.append(fileInfoT);
        // emit sendLog(fileInfo.filePath());
    }           

    if(LBResampling) {
        QFileInfo fileInfoL(subdirsLB.at(numCase).filePath(), amosImagesListIn.at(2).second.name);	
        // 	    cout << fileInfo.filePath().toStdString() << endl;
        if(fileInfoL.exists()) {
            QString ll = fileInfoL.absoluteFilePath();
            imageFiles.append(fileInfoL);
            // emit sendLog(fileInfo.filePath());
        }
    }
 
}

QStringList ResamplingSlicerLauncher::setOutputPaths()
{
    QStringList outputPaths;
    
    QString TXName = "TX_FL.tfm";
    QFileInfo TXInfo(outputDirectory, TXName);
    TXPath = TXInfo.absoluteFilePath();    
    
    if(doBiasCorrection && LBResampling) {
        QFileInfo flairOutPath(outputDirectory, amosImagesListOut.at(0).second.name);
        flairBCPath = flairOutPath.absoluteFilePath();
        QFileInfo T1TempBCPath(outputDirectory, "T1_bcN4_temp.nii");
        T1BCPath = T1TempBCPath.absoluteFilePath();
        QFileInfo T1OutPath(outputDirectory, amosImagesListOut.at(1).second.name);
        T1RSFLPath = T1OutPath.absoluteFilePath();
        QFileInfo LBOutPath(outputDirectory, amosImagesListOut.at(2).second.name);
        LBRSFLPath = LBOutPath.absoluteFilePath();
        
        outputPaths << flairBCPath << T1BCPath << T1RSFLPath << LBRSFLPath << TXPath;
    }
    else if(doBiasCorrection && !LBResampling) {
        QFileInfo flairOutPath(outputDirectory, amosImagesListOut.at(0).second.name);
        flairBCPath = flairOutPath.absoluteFilePath();        
        QFileInfo T1TempBCPath(outputDirectory, "T1_bcN4_temp.nii");
        T1BCPath = T1TempBCPath.absoluteFilePath();
        QFileInfo T1OutPath(outputDirectory, amosImagesListOut.at(1).second.name);
        T1RSFLPath = T1OutPath.absoluteFilePath();
        
        outputPaths << flairBCPath << T1BCPath << T1RSFLPath << TXPath;
    } 
    else if(!doBiasCorrection && LBResampling) {       
        QFileInfo T1OutPath(outputDirectory, amosImagesListOut.at(0).second.name);
        T1RSFLPath = T1OutPath.absoluteFilePath();
        QFileInfo LBOutPath(outputDirectory, amosImagesListOut.at(1).second.name);
        LBRSFLPath = LBOutPath.absoluteFilePath();        
        
        outputPaths << T1RSFLPath << LBRSFLPath << TXPath;
    }    
    else {
        QFileInfo T1OutPath(outputDirectory, amosImagesListOut.at(0).second.name);
        T1RSFLPath = T1OutPath.absoluteFilePath();
        
        outputPaths << T1RSFLPath << TXPath;
    }    
            
    return outputPaths;
}
