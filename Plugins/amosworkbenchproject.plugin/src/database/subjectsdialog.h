#ifndef SUBJECTSDIALOG_H
#define SUBJECTSDIALOG_H

#include <qewextensibledialog.h>
#include "rightclickmenu.h"

class ScansDialog;

class Subject;

namespace Ui {
class SubjectsDialog;
}

class SubjectsDialog : public QewExtensibleDialog
{
    Q_OBJECT

public:
    explicit SubjectsDialog(QWidget *parent = 0);
    ~SubjectsDialog();

    void generateSubjects(bool askToUser = true);
    void removeSubjects();

    void setScansDialog(const ScansDialog *scansDialog);
    void setSubjects(QMap<QString, Subject *> *subjects);

    void setMenuActions(const QMap<RightCLickMenu::ActionTypes, QAction*> &menuActions);
    QList<QAction*> setMenuActionTexts();
    
    void setSubjectsAsMainDirs(bool subjectsAsMainDirs);
    void setRootPath(const QString& rootPath);

public slots:
    void getExternVal(const QString& var_name, const QVariant& var_val);

protected:
    void changeEvent(QEvent *e);
    void insertRow(Subject* subject, int row);
    void removeSubjects(QStringList subjectsId);
    void populateSubjectsDialog();

protected slots:
    void cellSelected(int row, int col);
    void saveText(const QString& text);
    void onDescriptionModified();

signals:
    void emitBadCases(QStringList);
    void emitSubjectIds(QStringList);
    void emitUpdateSubjects();
    void emitRestoreSubjects();

protected:
    QMap<QString, Subject *>* subjects;
    Subject* currentSubject;
    int selectedRow;
    int rowHeight;
    const ScansDialog* scansDialog;
    QMap<RightCLickMenu::ActionTypes, QAction*> menuActions;
    bool subjectsAsMainDirs;
    QString rootPath;

private:
    Ui::SubjectsDialog *ui;
};

#endif // SUBJECTSDIALOG_H
