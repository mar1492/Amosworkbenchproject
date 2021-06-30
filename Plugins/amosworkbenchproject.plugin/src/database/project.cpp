#include "project.h"
#include "reconstruction.h"

#include <QObject>

Project::Project(bool subjectsAsMainDirs) : subjectsAsMainDirs(subjectsAsMainDirs)
{
    scans = 0;
    expertImage = new AmosImage;
    expertImage->setType(AmosImage::EXPERT);
    maskImage = new AmosImage;
    maskImage->setType(AmosImage::MASK);
    annotatedUuid = QUuid::createUuid();
    externUuid = QUuid::createUuid();
}

Project::~Project()
{
    clearLists();
    delete expertImage;
    delete maskImage;
}


void Project::clearLists()
{
    for(int i = 0; i < subjects.size(); i++)
        delete subjects.at(i);
    subjects.clear();

    for(int i = 0; i < reconstructions.size(); i++)
        delete reconstructions.at(i);
    reconstructions.clear();

    for(int i = 0; i < segmentations.size(); i++)
        delete segmentations.at(i);
    segmentations.clear();

    for(int i = 0; i < externImages.size(); i++)
        delete externImages.at(i);
    externImages.clear();
}

QDateTime Project::getStartTime() const
{
    return startTime;
}

void Project::setStartTime(const QDateTime &startTime)
{
    this->startTime = startTime;
}

QDateTime Project::getEndTime() const
{
    return endTime;
}

void Project::setEndTime(const QDateTime &endTime)
{
    this->endTime = endTime;
}

QString Project::getDescription() const
{
    return description;
}

void Project::setDescription(const QString &description)
{
    this->description = description;
}

QString Project::getComments() const
{
    return comments;
}

void Project::setComments(const QString &comments)
{
    this->comments = comments;
}

QString Project::saveXML()
{
    QDomDocument doc(XMLDocName());
    doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\""));
    QDomElement root = doc.createElement("Project");
    writeToXml(doc, root);
    return doc.toString(4);
}

void Project::writeToXml(QDomDocument &doc, QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    doc.appendChild(e);
    e.appendChild(xml_operator->createTextElement(doc,
                  "project_id", id));
    e.appendChild(xml_operator->createTextElement(doc, "subjectsAsMainDirs", subjectsAsMainDirs ? "true" : "false"));
    e.appendChild(xml_operator->createTextElement(doc, "output_path", outputPath));
    e.appendChild(xml_operator->createMultiLineTextElement(doc, "project_description", description));

    QDomElement tag_scans = doc.createElement("Scans");
    e.appendChild(tag_scans);
    scans->writeToXml(doc, tag_scans);

    QDomElement tag_subjects = doc.createElement("Subjects");
    e.appendChild(tag_subjects);

    for(int i = 0; i < subjects.size(); i++) {
        Subject* subject = subjects.at(i);
        subject->writeToXml(doc, tag_subjects);
    }

    QDomElement tag_annot = doc.createElement("Annotated");
    e.appendChild(tag_annot);
    tag_annot.appendChild(xml_operator->createTextElement(doc, "annotated_uuid", annotatedUuid.toString()));
    QDomElement tag_images = doc.createElement("Images");
    tag_annot.appendChild(tag_images);
    expertImage->writeToXml(doc, tag_images);
    maskImage->writeToXml(doc, tag_images);

    QDomElement tag_recons = doc.createElement("Reconstructions");
    e.appendChild(tag_recons);
    for(int i = 0; i < reconstructions.size(); i++) {
        QDomElement tag_rec = doc.createElement("Reconstruction");
        tag_recons.appendChild(tag_rec);
        reconstructions.at(i)->writeToXml(doc, tag_rec);
    }

    QDomElement tag_segments = doc.createElement("Segmentations");
    e.appendChild(tag_segments);
    for(int i = 0; i < segmentations.size(); i++) {
        QDomElement tag_seg = doc.createElement("Segmentation");
        tag_segments.appendChild(tag_seg);
        segmentations.at(i)->writeToXml(doc, tag_seg);
    }

    QDomElement tag_extern = doc.createElement("Extern_images");
    e.appendChild(tag_extern);
    tag_extern.appendChild(xml_operator->createTextElement(doc, "extern_uuid", externUuid.toString()));

    for(int i = 0; i < externImages.size(); i++) {
        AmosImage* image = externImages.at(i);
        image->writeToXml(doc, tag_extern);
    }
}

void Project::readFromXML(QDomElement& e)
{
    clearLists();

    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_id = xml_operator->findTag("project_id", e);
    if(!e_id.isNull())
        id = e_id.text();
    QDomElement e_subjectsAsMainDirs = xml_operator->findTag("subjectsAsMainDirs", e);
    if(!e_subjectsAsMainDirs.isNull()) 
        subjectsAsMainDirs = e_subjectsAsMainDirs.text() == "true" ? true : false;
    QDomElement e_outpath = xml_operator->findTag("output_path", e);
    if(!e_outpath.isNull())
        outputPath = e_outpath.text();
    QDomElement e_descript = xml_operator->findTag("project_description", e);
    if(!e_descript.isNull())
        description = xml_operator->readMultiLineTextElement(e_descript);
    QDomElement e_scans = xml_operator->findTag("Scans", e);
    if(!e_scans.isNull()) {
        if(!scans)
            scans = new Scans;
        scans->setXMLElement(e_scans);
    }

    QDomElement e_subjects = xml_operator->findTag("Subjects", e);
    QDomNodeList n_subjects = e_subjects.childNodes();
    int cnt = n_subjects.count();
    QDomElement e_sub;
    for(int i = 0; i < cnt; ++i) {
        QDomNode n = n_subjects.item(i);
        e_sub = n.toElement();
        if(!e_sub.isNull()) {
            Subject* subject = new Subject;
            subject->setXMLElement(e_sub);
            subjects.append(subject);
        }
    }

    QDomElement e_annot = xml_operator->findTag("Annotated", e);
    if(!e_annot.isNull()) {
        QDomElement e_uuid = xml_operator->findTag("annotated_uuid", e_annot);
        if(!e_uuid.isNull()) {
            annotatedUuid = QUuid(e_uuid.text());
        }

        QDomElement e_img = xml_operator->findTag("Images", e_annot);
        if(!e_img.isNull()) {
            QDomElement e_expert = xml_operator->findTag(AmosImage::getTypeName(AmosImage::EXPERT), e_img);
            if(!e_expert.isNull()) {
                expertImage->setXMLElement(e_expert);
            }
            QDomElement e_mask = xml_operator->findTag(AmosImage::getTypeName(AmosImage::MASK), e_img);
            if(!e_mask.isNull()) {
                maskImage->setXMLElement(e_mask);
            }
        }
    }

    QDomElement e_recons = xml_operator->findTag("Reconstructions", e);
    readReconstructions(e_recons, &reconstructions);

    QDomElement e_segments = xml_operator->findTag("Segmentations", e);
    readReconstructions(e_segments, &segmentations);

    QDomElement e_extern = xml_operator->findTag("Extern_images", e);
    if(!e_extern.isNull()) {
        QDomElement e_uuid = xml_operator->findTag("extern_uuid", e_extern);
        if(!e_uuid.isNull()) {
            externUuid = QUuid(e_uuid.text());
        }
        QDomNodeList n_extern = e_extern.childNodes();
        cnt = n_extern.count();
        QDomElement e_extimg;
        for(int i = 1; i < cnt; ++i) { // 1 uuid
            QDomNode n = n_extern.item(i);
            e_extimg = n.toElement();
            if(!e_extimg.isNull()) {
                AmosImage* image = new AmosImage;
                image->setXMLElement(e_extimg);
                externImages.append(image);
            }
        }
    }
}

void Project::readReconstructions(QDomElement &e, QList<Reconstruction*>* list)
{
    QDomNodeList n_recons  = e.childNodes();
    int cnt = n_recons .count();
    QDomElement e_rec;
    for(int i = 0; i < cnt; ++i) {
        QDomNode n = n_recons .item(i);
        e_rec = n.toElement();
        if(!e_rec.isNull()) {
            Reconstruction* reconstruction = new Reconstruction;
            reconstruction->setXMLElement(e_rec);
            QStringList ids;
            for(int i = 0; i < subjects.size(); i++)
                ids.append(subjects.at(i)->getId());
            reconstruction->setSubjectIds(ids);
            list->append(reconstruction);
        }
    }
}

bool Project::getSubjectsAsMainDirs()
{
    return subjectsAsMainDirs;
}

QString Project::getOutputPath() const
{
    return outputPath;
}

void Project::setOutputPath(const QString &outputPath)
{
    this->outputPath = outputPath;
}

void Project::setSvgContents(QString svgContents)
{
    this->svgContents = svgContents;
}

QString Project::getScansUuid() const
{
    return scans->getScansUuid();
}

QString Project::getExternUuid() const
{
    return externUuid.toString();
}

QString Project::getAnnotatedUuid() const
{
    return annotatedUuid.toString();
}

QList<AmosImage *>* Project::getExternImages()
{
    return &externImages;
}

QString Project::generateHtml(int prevIndent) const
{
    XMLOperator* xml_operator = XMLOperator::instance();
    xml_operator->initHtml(":/database/base.html", QObject::tr("Amosdatabase project ") + getId());

    for(int i = 0; i < reconstructions.size(); i++)
        xml_operator->insertReconstructionInNav(reconstructions.at(i)->getId());
    for(int i = 0; i < segmentations.size(); i++)
        xml_operator->insertSegmentationInNav(segmentations.at(i)->getId());

    QString html = xml_operator->getHtmlInit();
    html += xml_operator->getHtmlNav();
    html += xml_operator->initContentDiv();
    html += xml_operator->toHeading(QObject::tr("Project ") + id, prevIndent, "project");
    if(subjectsAsMainDirs)
        html += xml_operator->toParagraph(QObject::tr("XNAT"), prevIndent + 2, QObject::tr("Directories structure"));
    else
        html += xml_operator->toParagraph(QObject::tr("AMOS"), prevIndent + 2, QObject::tr("Directories structure"));
    if(subjectsAsMainDirs)
        html += xml_operator->toParagraph(outputPath, prevIndent + 2, QObject::tr("Subjects path"));
    else
        html += xml_operator->toParagraph(outputPath, prevIndent + 2, QObject::tr("Outputs path"));        
    html += xml_operator->toHeading(QObject::tr("Description"), prevIndent + 2, "projectdescript");
    html += xml_operator->toParagraph(description, prevIndent + 4);

    html += xml_operator->toHeading(QObject::tr("Graph"), prevIndent + 2, "graph");

    html += xml_operator->insertImage(svgContents);

    html += xml_operator->toHeading(QObject::tr("Scans"), prevIndent + 1, "scans");
    html += scans->generateHtml(prevIndent + 1);

    html += xml_operator->toHeading(QObject::tr("Annotated images"), prevIndent + 1, "annotated");

    html += expertImage->generateHtml(prevIndent + 2);
    html += maskImage->generateHtml(prevIndent + 2);

//    if(externImages.size() > 0) {
        html += xml_operator->toHeading(QObject::tr("External images"), prevIndent + 1, "extern");
        for(int i = 0; i < externImages.size(); i++)
            html += externImages.at(i)->generateHtml(prevIndent + 2);
//    }

    html += xml_operator->toHeading(QObject::tr("Reconstructions"), prevIndent + 1, "recons");
    for(int i = 0; i < reconstructions.size(); i++)
        html += reconstructions.at(i)->generateHtml(prevIndent + 1);

    html += xml_operator->toHeading(QObject::tr("Segmentations"), prevIndent + 1, "segments");
    for(int i = 0; i < segmentations.size(); i++)
        html += segmentations.at(i)->generateHtml(prevIndent + 1);

    html += xml_operator->toHeading(QObject::tr("Subjects"), prevIndent + 1, "subjects");
    html += xml_operator->toParagraph(QString::number(subjects.size()), prevIndent + 2,
                                      QObject::tr("Subjects number"));
    QStringList subjectsIds;
    for(int i = 0; i < subjects.size(); i++) {
        subjectsIds.append(xml_operator->wrapWithAnchor(subjects.at(i)->getId()));
    }
    html += xml_operator->toParagraph(subjectsIds.join(", "), prevIndent + 2,
                                      QObject::tr("Subjects ids"));
    for(int i = 0; i < subjects.size(); i++) {
        html += subjects.at(i)->generateHtml(prevIndent + 1);
    }

    html += xml_operator->endHtml();

    return html;
}

//void Project::saveHtml()
//{
//    XMLOperator* xml_operator = XMLOperator::instance();
//    QString sender = QObject::tr("Save project html");
//    QString filename = QDir::homePath();
//    QString type_file = "";
//    QString extensions = "html";
//    bool change_name = true;
//    QString html = generateHtml(1);
//    xml_operator->saveDocument(filename, html, type_file, extensions, sender, change_name);

//}

Scans *Project::getScans() const
{
    return scans;
}

void Project::setScans(Scans *scans)
{
    this->scans = scans;
}

QString Project::XMLDocName()
{
    return "AmosProject";
}

AmosImage *Project::getExpertImage() const
{
    return expertImage;
}

AmosImage *Project::getMaskImage() const
{
    return maskImage;
}

QList<Reconstruction *> *Project::getReconstructions()
{
    return &reconstructions;
}

void Project::addReconstruction(Reconstruction *reconstruction)
{
    reconstructions.append(reconstruction);
}

QList<Reconstruction *> *Project::getSegmentations()
{
    return &segmentations;
}

void Project::addSegmentation(Reconstruction *reconstruction)
{
    segmentations.append(reconstruction);
}

QString Project::getId() const
{
    return id;
}

void Project::setId(const QString &id)
{
    this->id = id;
}

void Project::addSubject(Subject *subject)
{
    subjects.append(subject);
}

void Project::removeSubject(Subject *subject)
{
    subjects.removeOne(subject);
}

QList<Subject *> Project::getAllSubjects()
{
    return subjects;
}

void Project::clearAllSubjects()
{
    subjects.clear();
}
