#include "xmlwritable.h"

XMLWritable::XMLWritable()
{

}

void XMLWritable::setXMLElement(QDomElement &e)
{
    element = e.cloneNode().toElement();
    readFromXML(element);
}

void XMLWritable::restoreOriginal()
{
    readFromXML(element);
}
