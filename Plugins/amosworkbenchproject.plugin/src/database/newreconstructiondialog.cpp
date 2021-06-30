#include "newreconstructiondialog.h"
#include "ui_newreconstructiondialog.h"

NewReconstructionDialog::NewReconstructionDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    ui = new Ui::NewReconstructionDialog;
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    
    connect(ui->programIDComboBox, SIGNAL(activated(const QString& )), this, SLOT(programIDChanged(const QString&)));
}

NewReconstructionDialog::~NewReconstructionDialog()
{
    delete ui;
}

QString NewReconstructionDialog::getID()
{
    return ui->idLineEdit->text().trimmed();
}

QString NewReconstructionDialog::getProgramID()
{
    return ui->programIDComboBox->currentText();
}

bool NewReconstructionDialog::getInternalExec()
{
    return ui->internalExecutionCheckBox->isChecked();
}

void NewReconstructionDialog::setInternalExec(bool internal)
{    
    ui->internalExecutionCheckBox->setChecked(internal);
    ui->internalExecutionCheckBox->setEnabled(false);
    
}

bool NewReconstructionDialog::getHasPrefixes()
{
    return ui->hasPrefixesCheckBox->isChecked();
}

void NewReconstructionDialog::setHasPrefixes(bool hasPrefixes)
{
    ui->hasPrefixesCheckBox->setChecked(hasPrefixes);
    ui->hasPrefixesCheckBox->setEnabled(false);
}

void NewReconstructionDialog::setProgramIDAndInternals(QMap<QString, QPair<QString, QString> > programIDAndInt)
{
    programIDAndInternals = programIDAndInt;
    QMapIterator<QString, QPair<QString, QString> > it(programIDAndInternals);
    int i = 0;
    while (it.hasNext()) {
        it.next();
        ui->programIDComboBox->insertItem(i, it.key());
        i++;
    }
    ui->programIDComboBox->insertItem(i, tr("Custom"));
    programIDAndInternals[tr("Custom")].first = "false";
    programIDAndInternals[tr("Custom")].second = "false";
    ui->programIDComboBox->setCurrentIndex(0);
    programIDChanged(ui->programIDComboBox->currentText());

}

void NewReconstructionDialog::programIDChanged(const QString& programID)
{
    if(programID != tr("Custom")) {
        if(programIDAndInternals.value(programID).first.toLower() == "true")
            setInternalExec(true);
        else if(programIDAndInternals.value(programID).first.toLower() == "false")
            setInternalExec(false);
        if(programIDAndInternals.value(programID).second.toLower() == "true")
            setHasPrefixes(true);
        else if(programIDAndInternals.value(programID).second.toLower() == "false")
            setHasPrefixes(false);       
    }
    else {// Custom
        ui->internalExecutionCheckBox->setEnabled(true);  
        ui->hasPrefixesCheckBox->setEnabled(true);
    }
}

void NewReconstructionDialog::hideCustomType()
{
    ui->programIDComboBox->removeItem(ui->programIDComboBox->count() - 1);
}

