#ifndef RECONSTRUCTIONINPUTINFO_H
#define RECONSTRUCTIONINPUTINFO_H

#include "xmlwritable.h"
#include <QString>

class ReconstructionInputInfo : public XMLWritable
{
public:
    ReconstructionInputInfo();
    ReconstructionInputInfo(QString source, QString imageType, QString imageName,
                            QString imageUuid, QString reconstructionUuid);
    ReconstructionInputInfo(const ReconstructionInputInfo &other);
    virtual ~ReconstructionInputInfo() {}


    bool operator == (const ReconstructionInputInfo & inputInfo);
    
    QString getSource() const;   
    QString getImageType() const;   
    QString getImageName() const;
    QString getImageUuid() const;
    QString getReconstructionUuid() const;

    bool isValid() const;
    
    virtual void writeToXml(QDomDocument& doc, QDomElement& e);
    virtual void readFromXML(QDomElement& e);
    QString generateHtml(int prevIndent) const;

    void setSource(const QString &source);

protected:
    void write_constructor(QDomElement& /*e*/) {}
    
protected:
    QString source;
    QString imageType;
    QString imageName;
    QString imageUuid;
    QString reconstructionUuid;
};

Q_DECLARE_TYPEINFO(ReconstructionInputInfo, Q_MOVABLE_TYPE);

#endif // RECONSTRUCTIONINPUTINFO_H
