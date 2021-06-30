#include "reconstruction.h"

#include <QObject>

QHash<Reconstruction::ExecStatus, QString> Reconstruction::initStatusNames()
{
    QHash<ExecStatus, QString> hash;

    hash.insert(Reconstruction::UNSET, "Unset");
    hash.insert(Reconstruction::SET, "Set");
    hash.insert(Reconstruction::RUNNING, "Running");
    hash.insert(Reconstruction::PARTIAL_FINISH, "Partially finished");
    hash.insert(Reconstruction::FINISH, "Finished");

    return hash;
}

QHash<Reconstruction::ExecStatus, QString> Reconstruction::statusNames = initStatusNames();

Reconstruction::Reconstruction()
{
    id = "";
    programPath = "";
    description = "";
    execStatus = UNSET;
    internalExec = false;
    hasPrefixes = false;
    reconstructionUuid = QUuid::createUuid();
}

Reconstruction::~Reconstruction()
{
    for(int i = 0; i < imagesOut.size(); i++)
        delete imagesOut.at(i);
    imagesOut.clear();
}

void Reconstruction::writeToXml(QDomDocument &doc, QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();

    e.appendChild(xml_operator->createTextElement(doc,
                  "reconstruction_id", id));    
    e.appendChild(xml_operator->createTextElement(doc,
                  "reconstruction_uuid", reconstructionUuid.toString()));
    e.appendChild(xml_operator->createMultiLineTextElement(doc,
                  "reconstruction_description", description));

    QDomElement tag_input = doc.createElement("Input_images");
    e.appendChild(tag_input);
    for(int i = 0; i < inputInfoImages.size(); i++) {
        inputInfoImages[i].writeToXml(doc, tag_input);
    }

    QDomElement tag_output = doc.createElement("Output_images");
    e.appendChild(tag_output);
    tag_output.appendChild(xml_operator->createTextElement(doc,
                  "output_path", outputPath));
    for(int i = 0; i < imagesOut.size(); i++) {
        imagesOut[i]->writeToXml(doc, tag_output);
    }

    QDomElement tag_program = doc.createElement("Program");
    e.appendChild(tag_program);
    tag_program.appendChild(xml_operator->createTextElement(doc,
                  "program_id", programId));
    tag_program.appendChild(xml_operator->createTextElement(doc,
                  "program_path", programPath));
    tag_program.appendChild(xml_operator->createTextElement(doc,
                  "internal_exec", internalExec ? "1" : "0"));
    tag_program.appendChild(xml_operator->createTextElement(doc,
                  "has_prefixes", hasPrefixes ? "1" : "0"));
    QString status = QString::number(execStatus);
    tag_program.appendChild(xml_operator->createTextElement(doc,
                  "exec_status", status));
    QDomElement tag_parameters = doc.createElement("Parameters");
    tag_program.appendChild(tag_parameters);
    QMapIterator<QString, Parameter> itp(parameters);
    while(itp.hasNext()) {
        itp.next();
        QDomElement tag_parameter = doc.createElement("parameter");
        tag_parameters.appendChild(tag_parameter);
        Parameter param = itp.value();

        tag_parameter.appendChild(xml_operator->createTextElement(doc,
                      "name", itp.key()));
        tag_parameter.appendChild(xml_operator->createTextElement(doc,
                      "prefix", param.prefix));
        tag_parameter.appendChild(xml_operator->createTextElement(doc,
                      "value", param.value));
    }
    
    QDomElement tag_processed = doc.createElement("ProcessedSubjects");
    e.appendChild(tag_processed);
    for(int i = 0; i < processedSubjects.size(); i++) {
        tag_processed.appendChild(xml_operator->createTextElement(doc,
                      "proc_subject", processedSubjects.at(i)));
    }    

    QDomElement tag_subjects = doc.createElement("SubjectsFileLists");
    e.appendChild(tag_subjects);
    for(int i = 0; i < subjectsFileList.size(); i++) {
        tag_subjects.appendChild(xml_operator->createTextElement(doc,
                      "file", subjectsFileList.at(i)));
    }

}

void Reconstruction::readFromXML(QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_id = xml_operator->findTag("reconstruction_id", e);
    if(!e_id.isNull())
        id = e_id.text();  

    QDomElement e_uuid = xml_operator->findTag("reconstruction_uuid", e);
    if(!e_uuid.isNull())
        reconstructionUuid = QUuid(e_uuid.text());

    QDomElement e_descript = xml_operator->findTag("reconstruction_description", e);
    if(!e_descript.isNull())
        description = xml_operator->readMultiLineTextElement(e_descript);   

    inputInfoImages.clear();
    QDomElement e_inputlist = xml_operator->findTag("Input_images", e);
    if(!e_inputlist.isNull()) {
        QDomNodeList n_inputlist = e_inputlist.childNodes();
        int cnt = n_inputlist.count();
        QDomElement e_input;
        for(int i = 0; i < cnt; ++i) {
            QDomNode n = n_inputlist.item(i);
            e_input = n.toElement();
            if(!e_input.isNull()) {
                ReconstructionInputInfo inputInfo;
                inputInfo.setXMLElement(e_input);
                inputInfoImages.append(inputInfo);
            }
        }
    }

    imagesOut.clear();
    QDomElement e_outputlist = xml_operator->findTag("Output_images", e);
    if(!e_outputlist.isNull()) {
        QDomNodeList n_outputlist = e_outputlist.childNodes();
        QDomElement e_path = xml_operator->findTag("output_path", e_outputlist);
        if(!e_path.isNull())
            outputPath = e_path.text();
        int cnt = n_outputlist.count();
        QDomElement e_output;
        for(int i = 1; i < cnt; ++i) {
            QDomNode n = n_outputlist.item(i);
            e_output = n.toElement();
            if(!e_output.isNull()) {
                AmosImage* image = new AmosImage;
                image->setXMLElement(e_output);
                imagesOut.append(image);
            }
        }
    }

    parameters.clear();
    QDomElement e_program = xml_operator->findTag("Program", e);
    if(!e_program.isNull()) {
        QDomElement e_program_id = xml_operator->findTag("program_id", e_program);
        if(!e_program_id.isNull())
            programId = e_program_id.text();
        QDomElement e_program_path = xml_operator->findTag("program_path", e_program);
        if(!e_program_path.isNull())
            programPath = e_program_path.text();
        QDomElement e_internal_exec = xml_operator->findTag("internal_exec", e_program);
        if(!e_internal_exec.isNull())
            internalExec = e_internal_exec.text() == "1" ? true : false;
        QDomElement e_has_prefixes = xml_operator->findTag("has_prefixes", e_program);
        if(!e_has_prefixes.isNull())
            hasPrefixes = e_has_prefixes.text() == "1" ? true : false;        
        QDomElement e_status = xml_operator->findTag("exec_status", e_program);
        if(!e_status.isNull()) {
            int status = e_status.text().toInt();
            execStatus = static_cast<Reconstruction::ExecStatus>(status);
        }

        QDomElement e_parameters = xml_operator->findTag("Parameters", e_program);
        if(!e_parameters.isNull()) {
            QDomNodeList n_params = e_parameters.childNodes();
            int cnt = n_params.count();
            QDomElement e_param;
            for(int i = 0; i < cnt; ++i) {
                QDomNode n = n_params.item(i);
                e_param = n.toElement();
                if(!e_param.isNull()) {
                    QDomElement e_name = xml_operator->findTag("name", e_param);
                    QDomElement e_prefix = xml_operator->findTag("prefix", e_param);
                    QDomElement e_value = xml_operator->findTag("value", e_param);
                    Parameter params;
                    if(!e_name.isNull() && !e_prefix.isNull() && !e_value.isNull()) {
                        params.name = e_name.text();
                        params.prefix = e_prefix.text();
                        params.value = e_value.text();
                        parameters.insert(params.name, params);
                    }
                }
            }
        }
    }
    
    QDomElement e_processed_list = xml_operator->findTag("ProcessedSubjects", e);
    if(!e_processed_list.isNull()) {   
        QDomNodeList n_proc = e_processed_list.childNodes();
        int cnt = n_proc.count();
        QDomElement e_processed;
        for(int i = 0; i < cnt; ++i) {
            QDomNode n = n_proc.item(i);
            e_processed = n.toElement();
            processedSubjects.append(e_processed.text());
        }
    }

    subjectsFileList.clear();
    QDomElement e_list = xml_operator->findTag("SubjectsFileLists", e);
    if(!e_list.isNull()) {
        QDomNodeList n_list = e_list.childNodes();
        int cnt = n_list.count();
        QDomElement e_file;
        for(int i = 0; i < cnt; ++i) {
            QDomNode n = n_list.item(i);
            e_file = n.toElement();
            if(!e_file.isNull())
                subjectsFileList.append(e_file.text());
        }
    }
}

QString Reconstruction::generateHtml(int prevIndent) const
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString html = xml_operator->toHeading(id, prevIndent + 1, id);
    html += xml_operator->toParagraph(description, prevIndent + 2,
                                      QObject::tr("Description"));

    html += xml_operator->toHeading(QObject::tr("Input images"), prevIndent + 2);
    QStringList headers;
    headers << QObject::tr("From") << QObject::tr("Image type")
            << QObject::tr("Image name");
    html += xml_operator->initTable(headers, prevIndent + 3);
    for(int i = 0; i < inputInfoImages.size(); i++)
        html += inputInfoImages.at(i).generateHtml(prevIndent + 3);
    html += xml_operator->closeTable();

    html += xml_operator->toHeading(QObject::tr("Output images"), prevIndent + 2);
    html += xml_operator->toParagraph(outputPath, prevIndent + 2,
                                      QObject::tr("Output path"));
    for(int i = 0; i < imagesOut.size(); i++) {
        imagesOut.at(i)->setWritePathToHtml(false);
        html += imagesOut.at(i)->generateHtml(prevIndent + 2);
    }

    html += xml_operator->toHeading(QObject::tr("Program"), prevIndent + 2);
    html += xml_operator->toParagraph(programId, prevIndent + 2,
                                      QObject::tr("Program id"));
    html += xml_operator->toParagraph(programPath, prevIndent + 2,
                                      QObject::tr("Program path"));

    html += xml_operator->toParagraph("", prevIndent + 2,
                                      QObject::tr("Program parameters"));
    QStringList paramsList;
    paramsList << QObject::tr("Name") << QObject::tr("Prefix") << QObject::tr("Value");
    html += xml_operator->initTable(paramsList, prevIndent + 3);
    QMapIterator<QString, Parameter> it(parameters);
    while(it.hasNext()) {
        it.next();
        QStringList params;
        params << it.key() << it.value().prefix << it.value().value;
        html += xml_operator->tableRow(params, prevIndent + 3);
    }
    html += xml_operator->closeTable();
    
    html += xml_operator->toHeading(QObject::tr("Processed subjects"), prevIndent + 1, "subjects");
    html += xml_operator->toParagraph(QString::number(processedSubjects.size()), prevIndent + 2,
                                      QObject::tr("Subjects number"));
    html += xml_operator->toParagraph(processedSubjects.join(", "), prevIndent + 2,
                                      QObject::tr("Subjects ids"));    

    html += xml_operator->toHeading(QObject::tr("Subjects list files"), prevIndent + 2);
    html += xml_operator->addList(subjectsFileList, prevIndent + 3);

    return html;
}

QString Reconstruction::getId() const
{
    return id;
}

void Reconstruction::setId(const QString &id)
{
    this->id = id;
}

void Reconstruction::addImageOut(AmosImage *imageOut)
{
    imagesOut.append(imageOut);
}

QList<AmosImage *>& Reconstruction::getImagesOut()
{
    return imagesOut;
}

void Reconstruction::setImagesOut(const QList<AmosImage *> &imagesOut)
{
    this->imagesOut = imagesOut;
}

QString Reconstruction::getProgramPath() const
{
    return programPath;
}

void Reconstruction::setProgramPath(const QString &programPath)
{
    this->programPath = programPath;
}

QMap<QString, Reconstruction::Parameter> &Reconstruction::getParameters()
{
    return parameters;
}

void Reconstruction::setParameters(const QMap<QString, Parameter> &parameters)
{
    this->parameters = parameters;
}

QDateTime Reconstruction::getStartTime() const
{
    return startTime;
}

void Reconstruction::setStartTime(const QDateTime &startTime)
{
    this->startTime = startTime;
}

QDateTime Reconstruction::getEndTime() const
{
    return endTime;
}

void Reconstruction::setEndTime(const QDateTime &endTime)
{
    this->endTime = endTime;
}

bool Reconstruction::getValidation() const
{
    return validation;
}

void Reconstruction::setValidation(bool validation)
{
    this->validation = validation;
}

QString Reconstruction::getDescription() const
{
    return description;
}

void Reconstruction::setDescription(const QString &description)
{
    this->description = description;
}

QString Reconstruction::getComments() const
{
    return comments;
}

void Reconstruction::setComments(const QString &comments)
{
    this->comments = comments;
}

QList<ReconstructionInputInfo> &Reconstruction::getInputInfoImages()
{
    return inputInfoImages;
}

void Reconstruction::setInputInfoImages(const QList<ReconstructionInputInfo> &inputInfoImages)
{
    this->inputInfoImages = inputInfoImages;
}

void Reconstruction::setSubjectIds(const QStringList &subjectIds)
{
    int cnt = subjectIds.size();
    QMutableMapIterator<QString, bool> itd(subjects);
    while(itd.hasNext()) {
        if(!subjectIds.contains(itd.next().key()))
            itd.remove();
    }
    for(int i = 0; i < cnt; i++) {
        if(!subjects.contains(subjectIds.at(i)))
            subjects.insert(subjectIds.at(i), false);
    }

//     QMapIterator<QString, bool> it(subjects);
//     while(itd.hasNext()) {
//         it.next();
//     }
}

QMap<QString, bool>& Reconstruction::getSubjects()
{
    return subjects;
}

void Reconstruction::setProcessedSubjects(const QStringList &subjectIds)
{
    processedSubjects = subjectIds;
}

QStringList Reconstruction::getProcessedSubjects()
{
    return processedSubjects;
}

QString Reconstruction::getExecStatusName(const Reconstruction::ExecStatus &status)
{
    QString name = "";
    name = Reconstruction::statusNames.value(status);
    return name;
}

Reconstruction::ExecStatus Reconstruction::getExecStatusFromName(const QString &statusName)
{
    Reconstruction::ExecStatus status = Reconstruction::statusNames.key(statusName);
    return status;
}

bool Reconstruction::getInternalExec() const
{
    return internalExec;
}

void Reconstruction::setInternalExec(bool internalExec)
{
    this->internalExec = internalExec;
}

bool Reconstruction::getHasPrefixes() const
{
    return hasPrefixes;
}

void Reconstruction::setHasPrefixes(bool hasPrefixes)
{
    this->hasPrefixes = hasPrefixes;
}

QString Reconstruction::getOutputPath() const
{
    return outputPath;
}

void Reconstruction::setOutputPath(const QString &outputPath)
{
    this->outputPath = outputPath;
}

QUuid Reconstruction::getReconstructionUuid() const
{
    return reconstructionUuid;
}

Reconstruction::ExecStatus Reconstruction::getExecStatus() const
{
    return execStatus;
}

void Reconstruction::setExecStatus(const ExecStatus &execStatus)
{
    this->execStatus = execStatus;
}

QStringList Reconstruction::getSubjectsFileList()
{
    return subjectsFileList;
}

void Reconstruction::setSubjectsFileList(const QStringList &subjectsFileList)
{
    this->subjectsFileList = subjectsFileList;
}

void Reconstruction::changeRootOutputPath(const QString &oldRootPath, const QString &newRootPath, bool subjectsAsMainDirs)
{
    if(!subjectsAsMainDirs)
        outputPath.replace(oldRootPath, newRootPath);
    for(int i = 0; i < imagesOut.size(); i++) {
        QString path = imagesOut.at(i)->getParentPath();
        if(!subjectsAsMainDirs) {
            path.replace(oldRootPath, newRootPath);
            imagesOut.at(i)->setPath(path);
            imagesOut.at(i)->setParentPath(path);
        }
        else
            imagesOut.at(i)->setParentPath(newRootPath);
    }
}

QString Reconstruction::getProgramId() const
{
    return programId;
}

void Reconstruction::setProgramId(const QString &programId)
{
    this->programId = programId;
}
