#include "reconstructioninputinfo.h"

#include <QObject>

ReconstructionInputInfo::ReconstructionInputInfo()
{
    
}

ReconstructionInputInfo::ReconstructionInputInfo(QString source, QString imageType,
                         QString imageName, QString imageUuid, QString reconstructionUuid) : source(source),
                         imageType(imageType), imageName(imageName),
                         imageUuid(imageUuid), reconstructionUuid(reconstructionUuid)
{

}

ReconstructionInputInfo::ReconstructionInputInfo(const ReconstructionInputInfo &other) :
    source(other.getSource()), imageType(other.getImageType()), imageName(other.getImageName()),
    imageUuid(other.getImageUuid()), reconstructionUuid(other.getReconstructionUuid())
{

}

bool ReconstructionInputInfo::operator == (const ReconstructionInputInfo &inputInfo)
{
    bool ret = false;

    if(inputInfo.getImageUuid() == imageUuid)
        ret = true;
    return ret;
}

QString ReconstructionInputInfo::getSource() const
{
    return source;
}

QString ReconstructionInputInfo::getImageType() const
{
    return imageType;
}

QString ReconstructionInputInfo::getImageName() const
{
    return imageName;
}

QString ReconstructionInputInfo::getImageUuid() const
{
    return imageUuid;
}

QString ReconstructionInputInfo::getReconstructionUuid() const
{
  return reconstructionUuid;
}

bool ReconstructionInputInfo::isValid() const
{
    if(source.isEmpty() || imageType.isEmpty() || imageName.isEmpty()
                 || imageUuid.isEmpty() || reconstructionUuid.isEmpty())
        return false;
    else
        return true;
}

void ReconstructionInputInfo::writeToXml(QDomDocument &doc, QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();

    QDomElement tag_input = doc.createElement("Reconstruction_Input");
    e.appendChild(tag_input);
    tag_input.appendChild(xml_operator->createTextElement(doc,
                  "source", source));
    tag_input.appendChild(xml_operator->createTextElement(doc,
                  "image_type", imageType));
    tag_input.appendChild(xml_operator->createTextElement(doc,
                  "image_name", imageName));
    tag_input.appendChild(xml_operator->createTextElement(doc,
                  "image_uuid", imageUuid));
    tag_input.appendChild(xml_operator->createTextElement(doc,
                  "reconstruction_uuid", reconstructionUuid));
}

void ReconstructionInputInfo::readFromXML(QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    
    QDomElement e_source = xml_operator->findTag("source", e);
    if(!e_source.isNull()) 
        source = e_source.text();
    
    QDomElement e_type = xml_operator->findTag("image_type", e);
    if(!e_type.isNull()) 
        imageType = e_type.text(); 
    
    QDomElement e_name = xml_operator->findTag("image_name", e);
    if(!e_name.isNull()) 
        imageName = e_name.text();

    QDomElement e_imguuid = xml_operator->findTag("image_uuid", e);
    if(!e_imguuid.isNull())
        imageUuid = e_imguuid.text();

    QDomElement e_reconsuuid = xml_operator->findTag("reconstruction_uuid", e);
    if(!e_reconsuuid.isNull())
        reconstructionUuid = e_reconsuuid.text();
}

QString ReconstructionInputInfo::generateHtml(int prevIndent) const
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QStringList data;
    data << source << imageType << imageName;
    QString html = xml_operator->tableRow(data, prevIndent);

//    html += xml_operator->toParagraph(fileName, prevIndent + 2, QObject::tr("Name"));
//    html += xml_operator->toParagraph("", prevIndent + 2,
//                      QObject::tr("Reconstructions and segmentations using this image"));
//    html += xml_operator->addList(reconstructionsUsingIds, prevIndent + 4);
//    html += xml_operator->toParagraph(description, prevIndent + 2,
//                                      QObject::tr("Description"));
    return html;
}

void ReconstructionInputInfo::setSource(const QString &source)
{
    this->source = source;
}
