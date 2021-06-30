#ifndef NEWPR0JECTDIALOG_H
#define NEWPR0JECTDIALOG_H

#include <QDialog>

namespace Ui
{
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT
    
public:
    NewProjectDialog(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    bool getSubjectsAsMainDirs();

private:
    Ui::NewProjectDialog* ui;
};

#endif // NEWPR0JECTDIALOG_H
