#include "labellingfreesurferwrapper.h"
#include <QTime>

int LabellingFreeSurferWrapper::getAmosProcessRange()
{
    return 2;
}

LabellingFreeSurferWrapper::LabellingFreeSurferWrapper(QObject* parent) : ExternalProcessWrapper(parent)
{
    amosProcessRange = getAmosProcessRange();
    onlyConvert = false;
}

LabellingFreeSurferWrapper::~LabellingFreeSurferWrapper()
{
    cout << "LabellingFreeSurferWrapper deleted" << endl;
}

void LabellingFreeSurferWrapper::setOutputPaths(QDir outputDirectory, QStringList outputPaths)
{
    this->outputDirectory = outputDirectory;
    FSOutputPath = outputPaths.at(0);
    LBOrigPath = outputPaths.at(1);   
    setAparc_AsegPath();
}

QStringList LabellingFreeSurferWrapper::getArguments()
{
    //nohup bash -c "source $FREESURFER_HOME/SetUpFreeSurfer.sh;recon-all -s E075 -i Subjects/E075/Scans/T1.nii -sd FS -all -dontrun"
    
    QStringList arguments;
    QStringList command;
    QString source;
    QProcessEnvironment environment = processEnvironment();
    if(environment.contains("FREESURFER_HOME")) {
//             QFileInfo fInfo(environment.value("FREESURFER_HOME"), "SetUpFreeSurfer.sh");
        QFileInfo fInfo(QDir(environment.value("FREESURFER_HOME")), QString("SetUpFreeSurfer.sh"));
        source = "source " + fInfo.absoluteFilePath() + ";";
        arguments << "bash";
        arguments << "-c";

    }
    if(onlyConvert) {
            countProcess = 2;
//             delay = 0;
    }
    if(countProcess == 1) {                    
//             arguments << fInfo.absoluteFilePath();
//             arguments << "--help";
//             arguments << ";";
        command << "recon-all";
        command << "-s" << subjectName;
        command << "-i" << imageFiles.at(0).absoluteFilePath();
        command << "-sd" << FSOutputPath;
        command << "-openmp" << QString::number(numThreads);
        command << "-all";
        command << "&";
        arguments.append(source + command.join(" "));
        //command << "-dontrun";
        emit sendProgressType(tr("Labelling"));
//             arguments << "\"";

//             QString arg = "";
//             for(int i = 0; i < arguments.size(); i++)
//                 arg = arg + arguments.at(i) + " ";
//             std::cout << arg.toStdString() << std::endl;
//             arguments.clear();
//             arguments << arg;

    }

//     else if(countProcess == 2) {
//         m_program = "nohup";
//         arguments << "recon-all";
//         arguments << "-s";
//         arguments << subjectName;
//         arguments << "-i";
//         arguments << imageFiles.at(0).absoluteFilePath();
//         arguments << "-sd";
//         arguments << FSOutputPath;
//         arguments << "-all";
//         arguments << "-dontrun";
// 
//         emit sendProgressType(tr("Labelling"));
//     }
    else if(countProcess == 2) {
        command << "mri_convert";
        command << aparc_AsegPath;
        command << LBOrigPath;
        command << "&";
        arguments.append(source + command.join(" "));
        emit sendProgressType(tr("Converting"));
    } 
//     else if(countProcess == 3) {
//         arguments << "/home/jose/mitk/qtprojects/build-delay-Desktop5-Release/delay";
//         QTime time = QTime::currentTime();
//         qsrand((uint)time.msec());
//         int timeInt = 30/* + qrand() % 11 - 5*/;
//         arguments << QString::number(timeInt);
//         arguments << subjectName;
//         emit sendProgressType(tr("Delaying"));
//     }
    
    return arguments;
}

bool LabellingFreeSurferWrapper::copyToOutput()
{
    bool ret = true;
    QFile LBOutput;
    LBOutput.setFileName(LBFSPath);
    ret = LBOutput.exists();
    if(!ret)
        return ret;
    LBOutput.setFileName(LBOrigPath);
    if(LBOutput.exists()) {
        ret = LBOutput.remove();
        if(!ret)
            return ret;
    }

    ret = QFile::copy(LBFSPath, LBOrigPath);
    return ret;
}

void LabellingFreeSurferWrapper::setAparc_AsegPath()
{
    QFileInfo FSSubject(FSOutputPath, subjectName);
    QFileInfo FSmri(FSSubject.absoluteFilePath(), "mri");
    QFileInfo aparc_aseg(FSmri.absoluteFilePath(), "aparc+aseg.mgz");
    aparc_AsegPath = aparc_aseg.absoluteFilePath();
//     QFileInfo LBOrig(LBOrigPath);        
//     QFileInfo LBFS(FSmri.absoluteFilePath(), LBOrig.fileName());
//     LBFSPath = LBFS.absoluteFilePath();
}

void LabellingFreeSurferWrapper::cleanAfterFinished()
{
/*    if(!stopped) {        
        emit sendProgressType(tr("Copying"));        
        copyToOutput();
    }  */  
    
    QFileInfo nohupout(QDir::homePath(), "nohup.out");
    QFile nohup_out(nohupout.absoluteFilePath());
    if(nohup_out.exists())
        nohup_out.remove();
}

bool LabellingFreeSurferWrapper::setParameters(QMap<QString, QString> parameters)
{
    if(parameters.contains("only convert"))
        onlyConvert = parameters["only convert"] == "true" ? true : false;    
    return true;
}
