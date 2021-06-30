#include "subject.h"

Subject::Subject()
{

}

Subject::~Subject()
{

}

QString Subject::getDescription() const
{
    return description;
}

void Subject::setDescription(const QString &description)
{
    this->description = description;
}

QString Subject::getId() const
{
    return id;
}

void Subject::setId(const QString &id)
{
    this->id = id;
}

void Subject::writeToXml(QDomDocument &doc, QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement tag_subject = doc.createElement("Subject");
    e.appendChild(tag_subject);
    tag_subject.appendChild(xml_operator->createTextElement(doc,
                  "subject_id", id));
    tag_subject.appendChild(xml_operator->createMultiLineTextElement(doc,
                                          "subject_description", description));
}

void Subject::readFromXML(QDomElement &e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_id = xml_operator->findTag("subject_id", e);
    if(!e_id.isNull())
        id = e_id.text();
    QDomElement e_descript = xml_operator->findTag("subject_description", e);
    if(!e_descript.isNull())
        description = xml_operator->readMultiLineTextElement(e_descript);
}

QString Subject::generateHtml(int prevIndent) const
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QString html = xml_operator->toHeading(id, prevIndent + 2, id);
    html += xml_operator->toParagraph(description, prevIndent + 3,
                                      QObject::tr("Description"));
    return html;
}

