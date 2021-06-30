#include "scans.h"

#include <QObject>

Scans::Scans()
{
    flair = new AmosImage();
    flair->setType(AmosImage::FLAIR);
    T1 = new AmosImage();
    T1->setType(AmosImage::T1);
    path = "";
    scansUuid = QUuid::createUuid();
//    path = QDir::home().absolutePath();
}

void Scans::writeToXml(QDomDocument &doc, QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    e.appendChild(xml_operator->createTextElement(doc,
                  "scans_uuid", scansUuid.toString()));
    e.appendChild(xml_operator->createTextElement(doc,
                  "scans_path", path));
    QDomElement tag_images = doc.createElement("Images");
    e.appendChild(tag_images);
    flair->writeToXml(doc, tag_images);
    T1->writeToXml(doc, tag_images);
}

void Scans::readFromXML(QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_uuid = xml_operator->findTag("scans_uuid", e);
    if(!e_uuid.isNull()) {
        scansUuid = QUuid(e_uuid.text());
    }
    QDomElement e_path = xml_operator->findTag("scans_path", e);
    if(!e_path.isNull()) {
        path = e_path.text();
    }
    QDomElement e_images = xml_operator->findTag("Images", e);
    if(!e_images.isNull()) {
        QDomElement e_flair = xml_operator->findTag(flair->getTypeName(), e_images);
        flair->setXMLElement(e_flair);
        QDomElement e_T1 = xml_operator->findTag(T1->getTypeName(), e_images);
        T1->setXMLElement(e_T1);
    }
}

QString Scans::generateHtml(int prevIndent) const
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString html = xml_operator->toParagraph(path, prevIndent + 1, QObject::tr("Scans path"));

    html += xml_operator->toHeading(QObject::tr("Images"), prevIndent + 1);
    flair->setWritePathToHtml(false);
    html += flair->generateHtml(prevIndent + 1);
    T1->setWritePathToHtml(false);
    html += T1->generateHtml(prevIndent + 1);

    return html;
}

AmosImage *Scans::getFlair() const
{
    return flair;
}

void Scans::setFlair(AmosImage *flair)
{
    this->flair = flair;
}

AmosImage *Scans::getT1() const
{
    return T1;
}

void Scans::setT1(AmosImage *T1)
{
    this->T1 = T1;
}

QString Scans::getPath() const
{
    return path;
}

void Scans::setPath(const QString &path)
{
    this->path = path;
}

QString Scans::getScansUuid() const
{
    return scansUuid.toString();
}
