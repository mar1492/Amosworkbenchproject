#ifndef SUBJECT_H
#define SUBJECT_H

#include "scans.h"
#include "reconstruction.h"
#include <QString>

class Subject : public XMLWritable
{
public:
    Subject();
    virtual ~Subject();

    void writeToXml(QDomDocument& doc, QDomElement& e);
    void readFromXML(QDomElement& e);
    QString generateHtml(int prevIndent) const;

    QString getDescription() const;
    void setDescription(const QString &description);

    QString getId() const;
    void setId(const QString &id);

protected:
    void write_constructor(QDomElement& /*e*/) {}

protected:
    QString id;
    QString description;
};

#endif // SUBJECT_H
