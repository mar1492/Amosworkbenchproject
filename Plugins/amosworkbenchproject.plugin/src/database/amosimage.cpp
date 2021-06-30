#include "amosimage.h"

#include <QObject>

QHash<AmosImage::ImageType, QString> AmosImage::initTypeNames()
{
    QHash<ImageType, QString> hash;

    hash.insert(AmosImage::NONE,"");
    hash.insert(AmosImage::FLAIR,"Flair_original");
    hash.insert(AmosImage::T1,"T1_original");
    hash.insert(AmosImage::MASK,"Mask");
    hash.insert(AmosImage::EXPERT,"Expert_annotated");
    hash.insert(AmosImage::LABEL,"Labeled_original");
    hash.insert(AmosImage::FLAIR_RECONS,"Flair_reconstructed");
    hash.insert(AmosImage::T1_RECONS,"T1_reconstructed");
    hash.insert(AmosImage::LABEL_RECONS,"Labeled_reconstructed");
    hash.insert(AmosImage::PROB_MAP,"Probabilistic_map");
    hash.insert(AmosImage::AMOS_AC,"Amos_AC");
    hash.insert(AmosImage::AMOS_BC,"Amos_BC");
    hash.insert(AmosImage::TypesCount,"");

    return hash;
}

QHash<AmosImage::ImageType, QString> AmosImage::typeNames = initTypeNames();

AmosImage::AmosImage()
{
    parentPath = "";
    path = "";
    subjectsAsMainDirs = false;
    fileName = "";
    description = "";
    type = NONE;
    imageUuid = QUuid::createUuid();
    writePathToHtml = true;
    cntReconstructionsState = 0;
    enableEdit = true;
}

AmosImage::AmosImage(const AmosImage &other) :
    parentPath(other.getParentPath()), path(other.getPath()), subjectsAsMainDirs(other.getSubjectsAsMainDirs()),
    fileName(other.getFileName()), description(other.getDescription()),
    imageUuid(other.getImageUuid()), type(other.getType()),
    writePathToHtml(other.getWritePathToHtml()),
    reconstructionsUsing(other.getReconstructionsUsing()),
    reconstructionsUsingIds(other.getReconstructionsUsingIds()),
    blockedReconstructionsId(other.copyBlockedReconstructionsId()),
    enableEdit(other.getEnableEdit())
{
}

void AmosImage::writeToXml(QDomDocument &doc, QDomElement &e)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    if(getType() == NONE)
        return;
    if(fileName.isEmpty() || path.isEmpty() || parentPath.isEmpty() || imageUuid.isNull())
        return;

    QDomElement tag_image = doc.createElement(getTypeName());
    e.appendChild(tag_image);

    tag_image.appendChild(xml_operator->createTextElement(doc,
                  "file_name", fileName));
    tag_image.appendChild(xml_operator->createTextElement(doc,
                  "parent_path", parentPath));
    tag_image.appendChild(xml_operator->createTextElement(doc,
                  "path", path));
    tag_image.appendChild(xml_operator->createTextElement(doc,
                  "uuid", imageUuid.toString()));
    tag_image.appendChild(xml_operator->createMultiLineTextElement(doc,
                  "image_description", description));
    QDomElement tag_recons = doc.createElement("Reconstructions");
    tag_image.appendChild(tag_recons);
    for(int i = 0; i < reconstructionsUsing.size(); i++) {
        QDomElement tag_rec = doc.createElement("Reconstruction");
        tag_recons.appendChild(tag_rec);
        tag_rec.appendChild(xml_operator->createTextElement(doc,
                      "recons_uuid", reconstructionsUsing.at(i)));
        tag_rec.appendChild(xml_operator->createTextElement(doc,
                      "recons_id", reconstructionsUsingIds.at(i)));
    }

}

void AmosImage::readFromXML(QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    type = AmosImage::getTypeFromName(e.tagName());
    QDomElement e_name = xml_operator->findTag("file_name", e);
    if(!e_name.isNull()) {
        fileName = e_name.text();
    }
    QDomElement e_parent_path = xml_operator->findTag("parent_path", e);
    if(!e_parent_path.isNull()) {
        parentPath = e_parent_path.text();
    }
    QDomElement e_path = xml_operator->findTag("path", e);
    if(!e_path.isNull()) {
        path = e_path.text();
    }
    QDomElement e_uuid = xml_operator->findTag("uuid", e);
    if(!e_uuid.isNull()) {
        imageUuid = QUuid(e_uuid.text());
    }

    QDomElement e_descript = xml_operator->findTag("image_description", e);
    if(!e_descript.isNull())
        description = xml_operator->readMultiLineTextElement(e_descript);

    QDomElement e_recons = xml_operator->findTag("Reconstructions", e);
    if(!e_recons.isNull()) {
        QDomNodeList n_recons = e_recons.childNodes();
        int cnt = n_recons.count();
        QDomElement e_rec;
        for(int i = 0; i < cnt; ++i) {
            QDomNode n = n_recons.item(i);
            e_rec = n.toElement();
            if(!e_rec.isNull()) {
                QDomElement e_recuuid = xml_operator->findTag("recons_uuid", e_rec);
                if(!e_recuuid.isNull())
                    reconstructionsUsing.append(e_recuuid.text());
                QDomElement e_recid = xml_operator->findTag("recons_id", e_rec);
                if(!e_recid.isNull())
                    reconstructionsUsingIds.append(e_recid.text());
            }
        }
    }
}

QString AmosImage::generateHtml(int prevIndent) const
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString html = xml_operator->toHeading(getTypeName(), prevIndent + 1);
    if(writePathToHtml)
        html += xml_operator->toParagraph(path, prevIndent + 2, QObject::tr("Path"));
    html += xml_operator->toParagraph(fileName, prevIndent + 2, QObject::tr("Name"));

    if(reconstructionsUsingIds.size() > 0) {
        QStringList ids;
        for(int i = 0; i < reconstructionsUsingIds.size(); i++) {
            if(blockedReconstructionsId.contains(reconstructionsUsingIds.at(i)))
                ids.append(reconstructionsUsingIds.at(i) + xml_operator->superScript("b"));
            else
                ids.append(reconstructionsUsingIds.at(i));
        }

        html += xml_operator->toParagraph("", prevIndent + 2,
                          QObject::tr("Reconstructions and segmentations using this image"));
        html += xml_operator->addList(ids, prevIndent + 4);
    }

    html += xml_operator->toParagraph(description, prevIndent + 2,
                                      QObject::tr("Description"));
    return html;
}

QString AmosImage::getDescription() const
{
    return description;
}

void AmosImage::setDescription(const QString &description)
{
    this->description = description;
}

AmosImage::ImageType AmosImage::getType() const
{
    return type;
}

void AmosImage::setType(const ImageType &type)
{
    this->type = type;
}

QString AmosImage::getTypeName() const
{
    return AmosImage::getTypeName(type);
}

QString AmosImage::getTypeName(const AmosImage::ImageType &type)
{
    QString name = "";
    name = AmosImage::typeNames.value(type);
    return name;
}

AmosImage::ImageType AmosImage::getTypeFromName(const QString &typeName)
{
    AmosImage::ImageType type = NONE;
    type = AmosImage::typeNames.key(typeName);
    return type;
}

QString AmosImage::getParentPath() const
{
    return parentPath;
}

void AmosImage::setParentPath(const QString &path)
{
    parentPath = path;
}

bool AmosImage::getSubjectsAsMainDirs() const
{
    return subjectsAsMainDirs;
}

void AmosImage::setSubjectsAsMainDirs(bool subjectsAsMainDirs)
{
    this->subjectsAsMainDirs = subjectsAsMainDirs;
}

QString AmosImage::getFileName() const
{
    return fileName;
}

void AmosImage::setFileName(const QString &name)
{
    fileName = name;
}

QString AmosImage::getPath() const
{
    return path;
}

void AmosImage::setPath(const QString &path)
{
    this->path = path;
}

QUuid AmosImage::getImageUuid() const
{
    return imageUuid;
}

bool AmosImage::isValid()
{
    if(type == NONE || imageUuid.isNull() || fileName.isEmpty() ||
            path.isEmpty() || parentPath.isEmpty())
        return false;
    else
        return true;
}

void AmosImage::setWritePathToHtml(bool writePathToHtml)
{
    this->writePathToHtml = writePathToHtml;
}

bool AmosImage::getWritePathToHtml() const
{
    return writePathToHtml;
}

void AmosImage::addReconstruction(QString uuid, QString source)
{
    int index = reconstructionsUsing.indexOf(uuid);
    if(index == -1) {
        reconstructionsUsing.append(uuid);
        reconstructionsUsingIds.append(source);
    }
    else
       reconstructionsUsingIds[index] = source;
}

void AmosImage::removeReconstruction(QString uuid)
{
    int index = reconstructionsUsing.indexOf(uuid);
    if(index > -1) {
        reconstructionsUsing.removeAt(index);
        reconstructionsUsingIds.removeAt(index);
    }
}

QStringList AmosImage::getReconstructionsUsing() const
{
    return reconstructionsUsing;
}

QStringList AmosImage::getReconstructionsUsingIds() const
{
    return reconstructionsUsingIds;
}

int AmosImage::getCntReconstructionsState() const
{
    return cntReconstructionsState;
}

void AmosImage::setCntReconstructionsState(int cntReconstructionsState)
{
    this->cntReconstructionsState = cntReconstructionsState;
}

QStringList &AmosImage::getBlockedReconstructionsId()
{
    return blockedReconstructionsId;
}

QStringList AmosImage::copyBlockedReconstructionsId() const
{
    return blockedReconstructionsId;
}

bool AmosImage::getEnableEdit() const
{
    return enableEdit;
}

void AmosImage::setEnableEdit(bool enableEdit)
{
    this->enableEdit = enableEdit;
}
