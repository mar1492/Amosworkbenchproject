#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

NewProjectDialog::NewProjectDialog(QWidget* parent, Qt::WindowFlags f) : QDialog( parent, f)
{
    ui = new Ui::NewProjectDialog;
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
}

bool NewProjectDialog::getSubjectsAsMainDirs()
{
    return ui->subjectsCheckBox->isChecked();
}
