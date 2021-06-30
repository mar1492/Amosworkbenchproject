#ifndef RECONSTRUCTION_H
#define RECONSTRUCTION_H

#include "amosimage.h"
#include "reconstructioninputinfo.h"

#include <QList>
#include <QDateTime>

class Reconstruction : public XMLWritable
{

public:
    struct Parameter {
        QString name;
        QString prefix;
        QString value;
    };

public:
    Reconstruction();
    virtual ~Reconstruction();

    enum ExecStatus {UNSET, SET, RUNNING, PARTIAL_FINISH, FINISH};

    void writeToXml(QDomDocument& doc, QDomElement& e);
    void readFromXML(QDomElement& e);
    QString generateHtml(int prevIndent) const;

    QString getId() const;
    void setId(const QString &id);    

    void addImageOut(AmosImage* imageOut);
    QList<AmosImage *>& getImagesOut();
    void setImagesOut(const QList<AmosImage *> &imagesOut);

    QString getProgramId() const;
    void setProgramId(const QString &programId);

    QString getProgramPath() const;
    void setProgramPath(const QString &programPath);

    QMap<QString, Parameter>& getParameters();
    void setParameters(const QMap<QString, Parameter> &parameters);

    QDateTime getStartTime() const;
    void setStartTime(const QDateTime &startTime);

    QDateTime getEndTime() const;
    void setEndTime(const QDateTime &endTime);

    bool getValidation() const;
    void setValidation(bool validation);

    QString getDescription() const;
    void setDescription(const QString &description);

    QString getComments() const;
    void setComments(const QString &comments);

    QList<ReconstructionInputInfo>& getInputInfoImages();
    void setInputInfoImages(const QList<ReconstructionInputInfo> &inputInfoImages);

    void setSubjectIds(const QStringList &subjectIds);
    QMap<QString, bool>& getSubjects();
    void setProcessedSubjects(const QStringList &subjectIds);
    QStringList getProcessedSubjects();

    static QString getExecStatusName(const ExecStatus &status);
    static ExecStatus getExecStatusFromName(const QString &statusName);

    bool getInternalExec() const;
    void setInternalExec(bool internalExec);
    
    bool getHasPrefixes() const;
    void setHasPrefixes(bool hasPrefixes);    

    QString getOutputPath() const;
    void setOutputPath(const QString &outputPath);

    QUuid getReconstructionUuid() const;

    ExecStatus getExecStatus() const;
    void setExecStatus(const ExecStatus &execStatus);

    QStringList getSubjectsFileList();
    void setSubjectsFileList(const QStringList &subjectsFileList);

    void changeRootOutputPath(const QString& oldRootPath, const QString &newRootPath, bool subjectsAsMainDirs);

protected:
    void write_constructor(QDomElement& /*e*/) {}

protected:
    QString id;
    QString programId;
    QList<ReconstructionInputInfo> inputInfoImages;
    QList<AmosImage*> imagesOut;
    QString outputPath;
    QString programPath;
    QMap<QString, Parameter> parameters;
    QDateTime startTime;
    QDateTime endTime;
    bool validation;
    QString description;
    QString comments;
    QMap<QString, bool> subjects;
    QStringList processedSubjects;
    bool internalExec;
    bool hasPrefixes;
    ExecStatus execStatus;
    QUuid reconstructionUuid;

    QStringList subjectsFileList;       

private:
    static QHash<ExecStatus, QString> statusNames;
    static QHash<ExecStatus, QString> initStatusNames();
};

#endif // RECONSTRUCTION_H
