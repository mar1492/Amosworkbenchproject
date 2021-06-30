#ifndef SCANS_H
#define SCANS_H

#include "amosimage.h"

class Scans : public XMLWritable
{
public:
    Scans();

    void writeToXml(QDomDocument& doc, QDomElement& e);
    void readFromXML(QDomElement& e);
    QString generateHtml(int prevIndent) const;

    AmosImage *getFlair() const;
    void setFlair(AmosImage *flair);

    AmosImage *getT1() const;
    void setT1(AmosImage *T1);

    QString getPath() const;
    void setPath(const QString &path);

    QString getScansUuid() const;

protected:
    void write_constructor(QDomElement& /*e*/) {}

protected:
    AmosImage* flair;
    AmosImage* T1;
    QString path;
    QUuid scansUuid;
};

#endif // SCANS_H
