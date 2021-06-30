#include "amosprocessdirectory.h"
#include "amosprocesscasethreaded.h"
#include "amosprocesscaseparallel.h"

#include <iostream>

using namespace std;

AmosProcessDirectory::AmosProcessDirectory()
{
    numImagesAmos = 3;

    amosProcessRange = 5;
    numCaseProgress = 4;
    numFeaturesSVM = 6; 
    numMITKOutputImages = 2;

}

AmosProcessDirectory::~AmosProcessDirectory()
{
//     for(int i = 0; i < amosThreads.size(); i++) {
//         if(!amosThreads.at(i).isNull())
//             delete amosThreads[i];
//     }
//     amosThreads.clear();
//     avaliableAmosThreads.clear();
    cout << "AmosProcessDirectory deleted" << endl;
}

bool AmosProcessDirectory::processSubDirs()
{
    FlairDirectory = QDir(amosImagesListIn.at(0).second.parentPath); 
    T1Directory = QDir(amosImagesListIn.at(1).second.parentPath); 
    LBDirectory = QDir(amosImagesListIn.at(2).second.parentPath); 
    
//     if(amosImagesListIn.size() >= numImagesAmos) {
    if(amosImagesListIn.size() > numImagesAmos) {
        if(amosImagesListIn.at(numImagesAmos).first == Utils::MASK && !amosImagesListIn.at(numImagesAmos).second.name.isEmpty()) {
            hasMask = true;
            MaskDirectory = QDir(amosImagesListIn.at(numImagesAmos).second.parentPath);
        }
    }  
    if(!modeAutomatic) {
        if(amosImagesListIn.at(amosImagesListIn.size() - 1).first == Utils::EXPERT)
            ExpertDirectory = QDir(amosImagesListIn.at(amosImagesListIn.size() - 1).second.parentPath);
    }    
    
    if(!modeAutomatic) {
        if(!FlairDirectory.exists() || !T1Directory.exists() || !LBDirectory.exists()|| (!MaskDirectory.exists() && hasMask)) {
            // emit sendLog(QString::fromLatin1("FlairDirectory or MaskDirectory do not exist"));
            // 	    cout << "FlairDirectory or MaskDirectory do not exist" << endl;
            return false;
        }
    }
    else {
        if(!FlairDirectory.exists() || !T1Directory.exists() || !LBDirectory.exists() || (!MaskDirectory.exists()  && hasMask)|| !ExpertDirectory.exists()) {
            // emit sendLog(QString::fromLatin1("FlairDirectory or MaskDirectory or ExpertDirectory do not exist"));
            // 	    cout << "FlairDirectory or MaskDirectory or ExpertDirectory do not exist" << endl;
            return false;
        }	
        
    }    
    
    for(int i = 0; i < subjects.size(); i++) {
        if(!subjectsAsMainDirs)
            subdirsFlair.append(QFileInfo(FlairDirectory, subjects.at(i)));
        else {
            QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(0).second.path;
            subdirsFlair.append(QFileInfo(FlairDirectory, subdir));
        }
    }
    numValidCases = subdirsFlair.size();    
    
    for (int i = 0; i < numValidCases; ++i) {
        // cout << subdirsFlair.at(i).absoluteFilePath().toStdString() << endl;
        if(!subdirsFlair.at(i).isDir()) {		
            subdirsFlair.removeAt(i);
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
    
    for (int i = 0; i < numValidCases; ++i) {
//         cout << subdirsT1.at(i).absoluteFilePath().toStdString() << endl;
        if(!subdirsT1.at(i).isDir()) {
            subdirsFlair.removeAt(i);
            subdirsT1.removeAt(i);
            emit sendMessageToUser(subjects.at(i) + " " + tr("does not contains valid or enough images"));
            subjects.removeAt(i);
            i--;
            numValidCases--;
        }
    } 
    
    for(int i = 0; i < subjects.size(); i++) {
        if(!subjectsAsMainDirs)
            subdirsLB.append(QFileInfo(LBDirectory, subjects.at(i)));
        else {
            QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(2).second.path;
            subdirsLB.append(QFileInfo(LBDirectory, subdir));
        }
    }   
    
    for (int i = 0; i < numValidCases; ++i) {
        if(!subdirsLB.at(i).isDir()) {
            subdirsFlair.removeAt(i);
            subdirsT1.removeAt(i);            
            subdirsLB.removeAt(i);
            emit sendMessageToUser(subjects.at(i) + " " + tr("does not contains valid or enough images"));
            subjects.removeAt(i);
            i--;
            numValidCases--;
        }
    }     

    if(hasMask) { 
        for (int i = 0; i < numValidCases; ++i) {
            if(!subjectsAsMainDirs)
                subdirsMask.append(QFileInfo(MaskDirectory, subjects.at(i)));
            else {
                QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(numImagesAmos).second.path;
                subdirsMask.append(QFileInfo(MaskDirectory, subdir));
            }
        }
    }   
    
    if(!modeAutomatic) {
        for (int i = 0; i < numValidCases; ++i) {
            QFileInfo fileInfo;
            if(!subjectsAsMainDirs)
                fileInfo.setFile(ExpertDirectory, subjects.at(i));
            else {
                QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(amosImagesListIn.size() - 1).second.path;
                fileInfo.setFile(ExpertDirectory, subdir);
            }
            if(fileInfo.exists())
                subdirsExpert.append(fileInfo);
            else {
                subdirsFlair.removeAt(i);
                subdirsT1.removeAt(i);            
                subdirsLB.removeAt(i);                
                if(hasMask)
                    subdirsMask.removeAt(i);
                emit sendMessageToUser(subjects.at(i) + " " + tr("does not contains valid or enough images"));
                subjects.removeAt(i);
                i--;
                numValidCases--;                
            }
        }        	    
    } 
    return true;
}

InternalProcessCase* AmosProcessDirectory::createProcess(int threadsPerProccess)
{
    InternalProcessCase* amos;
    if(useParallel) {
        amos = new AmosProcessCaseParallel(modeAutomatic, imagesPreviouslyLoaded, mode3D);
        amos->setNumThreads(threadsPerProccess);
    }
    else
        amos = new AmosProcessCaseThreaded(modeAutomatic, imagesPreviouslyLoaded, mode3D); 
    
    return amos;
}

void AmosProcessDirectory::loadExternalFiles(InternalProcessCase *amos)
{
    
    if(modeAutomatic)
        dynamic_cast<AmosProcessCase *>(amos)->setSVMFiles(svmModelFileName, svmScaleFileName);
    else
        dynamic_cast<AmosProcessCase *>(amos)->setSVMFiles("", "", numFeaturesSVM);     
}

bool AmosProcessDirectory::processNewSubDirs(bool rightDir, int numCase)
{
    QFileInfo fileInfoF(subdirsFlair.at(numCase).filePath(), amosImagesListIn.at(0).second.name);	
    if(fileInfoF.exists()) {
        imageFiles.append(fileInfoF);
    }

    QFileInfo fileInfoT(subdirsT1.at(numCase).filePath(), amosImagesListIn.at(1).second.name);	
    if(fileInfoT.exists()) 
        imageFiles.append(fileInfoT);         

    QFileInfo fileInfoL(subdirsLB.at(numCase).filePath(), amosImagesListIn.at(2).second.name);	
    if(fileInfoL.exists()) 
        imageFiles.append(fileInfoL);            
    
    if(imageFiles.size() < numImagesAmos)
        rightDir = false;
    if(!rightDir) {
        emit sendMessageToUser(subjectName + " " + tr("does not contains valid or enough images"));            
        badCase(numCase);
        return false;
    }
    
    // TODO
    if(!hasMask || amosImagesListIn.at(numImagesAmos).second.name.isEmpty()) // "Mask"
        imageFiles.append(QFileInfo());
    else {
        QFileInfo maskInfo(subdirsMask.at(numCase).filePath(), amosImagesListIn.at(numImagesAmos).second.name);
        imageFiles.append(maskInfo);
    }
    
    if(!modeAutomatic) {
        if(amosImagesListIn.at(amosImagesListIn.count() - 1).first == Utils::EXPERT) {
            QFileInfo expertInfo(subdirsExpert.at(numCase).filePath(), amosImagesListIn.at(amosImagesListIn.count() - 1).second.name);
            if(expertInfo.exists()) 		    
                imageFiles.append(expertInfo);
            else {
                imageFiles.clear();
                rightDir = false;
                emit sendMessageToUser(subjectName + " " + tr("does not contains valid or enough images"));                     
                badCase(numCase);
                return false;
            }
        }
        else {
            imageFiles.clear();
            rightDir = false;
            emit sendMessageToUser(subjectName + " " + tr("does not contain expert annotated image"));
            badCase(numCase);
            return false;
        }
    }  
    return true;
}

// void AmosProcessDirectory::setSvmFiles(QString svmModelFileName, QString svmScaleFileName)
// {
//     this->svmModelFileName = svmModelFileName;
//     this->svmScaleFileName = svmScaleFileName;
// }

void AmosProcessDirectory::setExternalFiles(QStringList files)
{
    this->svmModelFileName = files.at(0);
    this->svmScaleFileName = files.at(1);
}
