#include "contrastprocessdirectory.h"
// #include "contrastprocesscase.h"
#include "contrastprocesscaseparallel.h"
#include "contrastprocesscasethreaded.h"

ContrastProcessDirectory::ContrastProcessDirectory()
{
    numImagesAmos = 2;
    numMITKOutputImages = 1;
    amosProcessRange = 4;
    numCaseProgress = 4;
}

ContrastProcessDirectory::~ContrastProcessDirectory()
{
    cout << "ContrastProcessDirectory deleted" << endl;
}


bool ContrastProcessDirectory::processSubDirs()
{
    FlairDirectory = QDir(amosImagesListIn.at(0).second.parentPath); 
    LBDirectory = QDir(amosImagesListIn.at(1).second.parentPath); 
    
//     if(amosImagesListIn.size() >= numImagesAmos) {
    if(amosImagesListIn.size() > numImagesAmos) {
        if(amosImagesListIn.at(numImagesAmos).first == Utils::MASK && !amosImagesListIn.at(numImagesAmos).second.name.isEmpty()) {
            hasMask = true;
            MaskDirectory = QDir(amosImagesListIn.at(numImagesAmos).second.parentPath);
        }
    }      
    

    if(!FlairDirectory.exists() || !LBDirectory.exists()|| (!MaskDirectory.exists() && hasMask)) {
        // emit sendLog(QString::fromLatin1("FlairDirectory or MaskDirectory do not exist"));
        // 	    cout << "FlairDirectory or MaskDirectory do not exist" << endl;
        return false;
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
            subdirsLB.append(QFileInfo(LBDirectory, subjects.at(i)));
        else {
            QString subdir = subjects.at(i) + QDir::separator() + amosImagesListIn.at(1).second.path;
            subdirsLB.append(QFileInfo(LBDirectory, subdir));
        }
    }   
    
    for (int i = 0; i < numValidCases; ++i) {
        if(!subdirsLB.at(i).isDir()) {
            subdirsFlair.removeAt(i);            
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

    return true;
}

InternalProcessCase* ContrastProcessDirectory::createProcess(int threadsPerProccess)
{
/*    InternalProcessCase* icase = new ContrastProcessCase(modeAutomatic, imagesPreviouslyLoaded, mode3D);
    icase->setNumThreads(threadsPerProccess);
    return icase*;*/
    
    InternalProcessCase* icase;
    if(useParallel) {
        icase = new ContrastProcessCaseParallel(modeAutomatic, imagesPreviouslyLoaded, mode3D);
        icase->setNumThreads(threadsPerProccess);
    }
    else 
        icase = new ContrastProcessCaseThreaded(modeAutomatic, imagesPreviouslyLoaded, mode3D);        
    
    return icase;    
}

void ContrastProcessDirectory::loadExternalFiles(InternalProcessCase* /*amos*/)
{

}

bool ContrastProcessDirectory::processNewSubDirs(bool rightDir, int numCase)
{
    QFileInfo fileInfoF(subdirsFlair.at(numCase).filePath(), amosImagesListIn.at(0).second.name);	
    if(fileInfoF.exists()) {
        imageFiles.append(fileInfoF);
    }        

    QFileInfo fileInfoL(subdirsLB.at(numCase).filePath(), amosImagesListIn.at(1).second.name);	
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
  
    return true;
}

void ContrastProcessDirectory::setExternalFiles(QStringList /*files*/)
{
}
