#include "labellingfreesurferlauncher.h"
#include "labellingfreesurferwrapper.h"

LabellingFreeSurferLauncher::LabellingFreeSurferLauncher(QObject* parent) : ExternalProcessLauncher(parent)
{
    amosProcessRange = LabellingFreeSurferWrapper::getAmosProcessRange(); // labelling, copy
    numImageFilesIn = 1;
    numImageFilesOut = 1;
}

LabellingFreeSurferLauncher::~LabellingFreeSurferLauncher()
{
/*    int threadsPerProccess = numThreads/numValidCases;
    if(threadsPerProccess > 1)
        useParallel = true;
    else {
        threadsPerProccess = 1;
        useParallel = false;
    }
    numThreads = min(numThreads, numValidCases);*/     
}

ExternalProcessWrapper* LabellingFreeSurferLauncher::newProcess()
{      
    ExternalProcessWrapper* process = new LabellingFreeSurferWrapper();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("FREESURFER_HOME", freesurferPath);
//     QProcess setUpFreeSurfer;
    process->setProcessEnvironment(env);
//     setUpFreeSurfer.setStandardOutputProcess(process);
//     setUpFreeSurfer.start();
//     setUpFreeSurfer.waitForFinished();
//     QFileInfo fInfo;
//     fInfo.setFile(freesurferPath, "fsfast");
//     env.insert("FSFAST_HOME", fInfo.absoluteFilePath());
//     fInfo.setFile(freesurferPath, "subjects");
//     env.insert("SUBJECTS_DIR", fInfo.absoluteFilePath());
//     fInfo.setFile(freesurferPath, "mni");
//     env.insert("MNI_DIR", fInfo.absoluteFilePath());
//     env.insert("FSF_OUTPUT_FORMAT", "nii.gz");
//     process->setProcessEnvironment(env);
    return process;
}

void LabellingFreeSurferLauncher::setNumThreads(int numThreads)
{
    this->numThreads = numThreads;
    useParallel = false;
}

void LabellingFreeSurferLauncher::setNumSimultaneousSubjects(int numSimultaneousSubjects)
{
    this->numSimultaneousSubjects = numSimultaneousSubjects;
}

void LabellingFreeSurferLauncher::setProgram(QString /*programPath*/)
{
    m_program = "nohup";   
}

bool LabellingFreeSurferLauncher::setParameters(QMap<QString, QString> parameters)
{
    if(!parameters.contains("FSPath") || !parameters.contains("path4FSOutput"))
        return false;
    freesurferPath = parameters.value("FSPath");
    
    bool insideSubjects = false;
    if(parameters.contains("FS out inside subjects"))
        insideSubjects = parameters["FS out inside subjects"] == "true" ? true : false;  
    
    QFileInfo subjectsOutputPath(parameters.value("path4FSOutput"));
    QDir mainProjectDir(subjectsOutputPath.absolutePath());
    QFileInfo FSOutput;
    if(insideSubjects)
        FSOutput.setFile(subjectsOutputPath.absoluteFilePath(), "FS");
    else
        FSOutput.setFile(subjectsOutputPath.absolutePath(), "FS");
    if(!FSOutput.exists()) {
        if(!mainProjectDir.mkdir("FS")) {
            emit sendLog(tr("Can not create FreeSurfer output directory ") + FSOutput.absoluteFilePath());
            // 	    cout << "Can not create output directory " << OutParentDirectory.dirName().toStdString() << endl;
            return false;
        }        
    }
    FSOutputPath = FSOutput.absoluteFilePath();
    
    bool onlyConvert = false;
    if(parameters.contains("only convert"))
        onlyConvert = parameters["only convert"] == "true" ? true : false;     
    
    if(parameters.contains("delay") && !onlyConvert)
        delay = parameters.value("delay").toInt(); 

    this->parameters["only convert"] = onlyConvert ? "true" : "false";    
    
    return true;
}

bool LabellingFreeSurferLauncher::setAmosImagesData(QList< QPair< Utils::ImageTypes, Utils::AmosImageData > > amosImagesListIn, QString outputParentPath, QString outputPath, QList< QPair< Utils::ImageTypes, Utils::AmosImageData > > amosImagesListOut)
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

    T1Directory = QDir(amosImagesListIn.at(0).second.parentPath);    

    if(!T1Directory.exists()) {
        // emit sendLog(QString::fromLatin1("FlairDirectory or MaskDirectory do not exist"));
        // 	    cout << "FlairDirectory or MaskDirectory do not exist" << endl;
        return false;
    }       
    
    for(int i = 0; i < subjects.size(); i++) {
        if(!subjectsAsMainDirs)
            subdirsT1.append(QFileInfo(T1Directory, subjects.at(i)));
        else {
            QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(0).second.path;
            subdirsT1.append(QFileInfo(T1Directory, subdir));
        }
    }
    numValidCases = subdirsT1.size();    
    
    for (int i = 0; i < numValidCases; ++i) {
//         cout << subdirsFlair.at(i).absoluteFilePath().toStdString() << endl;
        if(!subdirsT1.at(i).isDir()) {
            subdirsT1.removeAt(i);
            emit sendMessageToUser(subjects.at(i) + " " + tr("does not contains valid or enough images"));
            subjects.removeAt(i);
            i--;
            numValidCases--;
        }
    }

    if(numValidCases == 0) {
        emit sendLog(tr("No valid cases."));
        return false;
    }
    return true;
}

void LabellingFreeSurferLauncher::appendImageFilesIn(int numCase)
{

    QFileInfo fileInfoT(subdirsT1.at(numCase).filePath(), amosImagesListIn.at(0).second.name);	
    // 	    cout << fileInfo.filePath().toStdString() << endl;
    if(fileInfoT.exists()) {
        QString ll = fileInfoT.absoluteFilePath();
        imageFiles.append(fileInfoT);
        // emit sendLog(fileInfo.filePath());
    }
    
}

QStringList LabellingFreeSurferLauncher::setOutputPaths()
{
    QFileInfo LBOutPath(outputDirectory, amosImagesListOut.at(0).second.name);
    LBOrigPath = LBOutPath.absoluteFilePath();
    
    QStringList outputPaths;
    outputPaths << FSOutputPath;
    outputPaths << LBOrigPath;
    return outputPaths;
}

