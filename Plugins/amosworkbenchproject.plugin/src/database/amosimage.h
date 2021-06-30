#ifndef AMOSIMAGE_H
#define AMOSIMAGE_H

#include "xmlwritable.h"

#include <QFileInfo>
#include <QUuid>

class AmosImage : public XMLWritable
{
public:
    AmosImage();
    AmosImage(const AmosImage &other);
    virtual ~AmosImage() {}

    enum ImageType {NONE, FLAIR, T1, MASK, EXPERT, LABEL, FLAIR_RECONS, T1_RECONS,
                    LABEL_RECONS, PROB_MAP, AMOS_AC, AMOS_BC,  TypesCount};

    void writeToXml(QDomDocument& doc, QDomElement& e);
    void readFromXML(QDomElement& e);
    QString generateHtml(int prevIndent) const;

    QString getDescription() const;
    void setDescription(const QString &description);

    ImageType getType() const;
    void setType(const ImageType &type);
    QString getTypeName() const;
    static QString getTypeName(const ImageType &type);
    static ImageType getTypeFromName(const QString &typeName);

    QString getParentPath() const;
    void setParentPath(const QString &path);

    QString getFileName() const;
    void setFileName(const QString &name);

    QString getPath() const;
    void setPath(const QString &path);
    
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);
    bool getSubjectsAsMainDirs() const;

    QUuid getImageUuid() const;

    bool isValid();

    void setWritePathToHtml(bool writePathToHtml);
    bool getWritePathToHtml() const;

    void addReconstruction(QString uuid, QString source);
    void removeReconstruction(QString uuid);

    QStringList getReconstructionsUsing() const;
    QStringList getReconstructionsUsingIds() const;

    int getCntReconstructionsState() const;
    void setCntReconstructionsState(int cntReconstructionsState);
    QStringList& getBlockedReconstructionsId();
    QStringList copyBlockedReconstructionsId() const;

    bool getEnableEdit() const;
    void setEnableEdit(bool enableEdit);

protected:
    void write_constructor(QDomElement& /*e*/) {}

protected:
    QString parentPath;
    QString path;
    bool subjectsAsMainDirs;
    QString fileName;
    QString description;
    QUuid imageUuid;
    ImageType type;    
    bool writePathToHtml;
    QStringList reconstructionsUsing;
    QStringList reconstructionsUsingIds;
    int cntReconstructionsState;
    QStringList blockedReconstructionsId;
    bool enableEdit;

private:
    static QHash<ImageType, QString> typeNames;
    static QHash<ImageType, QString> initTypeNames();

};

#endif // AMOSIMAGE_H
