#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <qewextensibledialog.h>

#include "amosimage.h"

class AnnotatedImageDialog;
namespace Ui {
class ImageDialog;
}

class ImageDialog : public QewExtensibleDialog
{
    Q_OBJECT

public:
    explicit ImageDialog(QWidget *parent = 0);
    ~ImageDialog();

    void fillListTypes();
    void hideTypeSelection();
    void hidePathSelection();
    void showNameToolButton(bool show);
    void removeTypes(QList<AmosImage::ImageType> types);
    void hideTitleLabel();
    void setTitle(const QString& title);
    void setImage(AmosImage* image);
    
    bool getSubjectsAsMainDirs();
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);

    QString getPath() const;    
    void setPath(const QString& path, bool fromUser = false);
    QString getParentPath() const;    
    void setParentPath(const QString& parentPath);    
    QString getName() const;

    void setAnyFile(bool anyFile);
    void fillUi();

    bool dialogActivated(bool activated, QewDialogInactivate from = FromOutside,
                         bool noSaveChildren = false);
    void askReconstructionState();

    bool getChangedOtherThanDescription() const;

    bool getInvalidFile() const;
    void setInvalidFile(bool invalidFile);

    void enableOtherThanPath(QString pathText);

    void setTopDir(const QString &topDir);
    
    void hideCleanWidget();
    
    void setOneValidSubjectId(QString oneValidSubjectId);

public slots:
    void getExternVal(const QString& var_name, const QVariant& var_val);

protected slots:
    void choosePath();
    void chooseName();
    void setName();
    void setPath();
    void chooseType(int type);
    void onDescriptionModified();
    void eraseData();
//    void restrictDir(QString dir);

protected:
    void changeEvent(QEvent *e);
    void saveThis(void);    
    bool validateThis( void );
//    void cleanThis( void );
    void restoreThis(void);
    void enableEdition();    

protected:
    AmosImage* image;
    int type;
    QString name;
    QString description;
    bool subjectsAsMainDirs;
    QString path;
    QString parentPath;
    bool hideTypeSelect;
    bool hidePathSelect;
    QHash<QString, int> itemTypeToPos;
    bool anyFile;
    bool invalidFile;
    bool changedOtherThanDescription;
    QString topDir;
    QString oneValidSubjectId;

//    QFileDialog *fd;

private:
    Ui::ImageDialog *ui;
};

#endif // IMAGEDIALOG_H
