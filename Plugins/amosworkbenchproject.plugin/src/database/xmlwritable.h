#ifndef XMLWRITABLE_H
#define XMLWRITABLE_H

#include "xmloperator.h"

class XMLWritable
{
public:
    XMLWritable();

    virtual void writeToXml(QDomDocument& doc, QDomElement& e) = 0;    
    void setXMLElement(QDomElement& e);
    void restoreOriginal();

    virtual QString generateHtml(int prevIndent) const = 0 ;

protected:
    virtual void readFromXML(QDomElement& e) = 0;
    virtual void write_constructor(QDomElement& e) = 0;

private:
    QDomElement element;
};

#endif // XMLWRITABLE_H
