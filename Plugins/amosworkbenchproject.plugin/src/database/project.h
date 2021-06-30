#ifndef PROJECT_H
#define PROJECT_H

#include "subject.h"
#include "scans.h"

class Reconstruction;

class Project : public XMLWritable
{
public:
    Project(bool subjectsAsMainDirs = false);
    virtual ~Project();

    void writeToXml(QDomDocument& doc, QDomElement& e);
    void readFromXML(QDomElement& e);
    QString generateHtml(int prevIndent) const;
//    void saveHtml();

    QString getId() const;
    void setId(const QString &id);
    
    bool getSubjectsAsMainDirs();

    void addSubject(Subject* subject);
    void removeSubject(Subject* subject);
    Subject* getSubject(QString id);
    QList<Subject *> getAllSubjects();
    void clearAllSubjects();

    QDateTime getStartTime() const;
    void setStartTime(const QDateTime &startTime);

    QDateTime getEndTime() const;
    void setEndTime(const QDateTime &endTime);

    QString getDescription() const;
    void setDescription(const QString &description);

    QString getComments() const;
    void setComments(const QString &comments);

    QString saveXML();

    Scans *getScans() const;

    void setScans(Scans *scans);

    static QString XMLDocName();

    AmosImage *getExpertImage() const;
    AmosImage *getMaskImage() const;
    QList<Reconstruction*>* getReconstructions();
    void addReconstruction(Reconstruction* reconstruction);
    QList<Reconstruction*>* getSegmentations();
    void addSegmentation(Reconstruction* reconstruction);

    QList<AmosImage *>* getExternImages();

    QString getScansUuid() const;
    QString getAnnotatedUuid() const;
    QString getExternUuid() const;

    QString getOutputPath() const;
    void setOutputPath(const QString &outputPath);

    void setSvgContents(QString svgContents);

protected:
    void clearLists();
    void write_constructor(QDomElement& /*e*/) {}
    void readReconstructions(QDomElement& e, QList<Reconstruction *> *list);

protected:
    QString id;
    bool subjectsAsMainDirs;
    QString outputPath;
    QList<Subject*> subjects;
    QDateTime startTime;
    QDateTime endTime;
    QString description;
    QString comments;
    Scans* scans;
    QList<Reconstruction*> reconstructions;
    QList<Reconstruction*> segmentations;
    AmosImage* expertImage;
    AmosImage* maskImage;
    QList<AmosImage*> externImages;
    QUuid annotatedUuid;
    QUuid externUuid;
    QString svgContents;

};

#endif // PROJECT_H
