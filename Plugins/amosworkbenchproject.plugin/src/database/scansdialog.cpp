#include "scansdialog.h"
#include "ui_twoimagesdialog.h"
#include "imagedialog.h"
#include "scans.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

ScansDialog::ScansDialog(QWidget *parent) : TwoImagesDialog(parent)
{
    scans = 0;
    subjectsGenerated = false;
    subjectsGenAgainMaybe = false;

    firstTitle = tr("Flair image");
    secondTitle = tr("T1 image");

    saveActionText = tr("Update scans images");
    saveActionToolTip = tr("Update scans images information");

    restoreActionText = tr("Restore scans images");
    restoreActionToolTip = tr("Restore scans images information\nto previously saved");
}

ScansDialog::~ScansDialog()
{
    qDebug("Deleting ScansDialog");
}

bool ScansDialog::setUpDialog()
{
    bool ret = TwoImagesDialog::setUpDialog();
    showPathWidget(true);
    firstDialog->setAnyFile(false);
    secondDialog->setAnyFile(false);
    firstDialog->hideCleanWidget();
    secondDialog->hideCleanWidget();
    return ret;
}

void ScansDialog::changeEvent(QEvent *e)
{
    TwoImagesDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ScansDialog::saveThis()
{
    TwoImagesDialog::saveThis();
    scans->setPath(path);
    qDebug("Saving ScansDialog");
}

void ScansDialog::setSubjectsGenerated(bool subjectsGenerated)
{
    this->subjectsGenerated = subjectsGenerated;
}

void ScansDialog::preDialogActivated(bool activated, QewExtensibleDialog::QewDialogInactivate *from)
{
    if(subjectsGenerated && !activated
            && *from != QewExtensibleDialog::FromRestore) {
        QString title(tr("Subjects are already generated"));
        QString text(tr("Subjects are already generated from these scans.") + "\n" +
                    tr("If you save scans information subjects will be generated again."));
        QMessageBox::warning(0, title, text);

        *from = QewExtensibleDialog::FromSave;
        subjectsGenAgainMaybe = true;
    }
}

void ScansDialog::postDialogActivated(bool response)
{
    if(subjectsGenAgainMaybe && response)
        emit generateSubjectsAgain();
    subjectsGenAgainMaybe = false;
}

Scans *ScansDialog::getScans() const
{
    return scans;
}

void ScansDialog::setScans(Scans *scans)
{
    this->scans = scans;
    path = scans->getPath();
    firstImage = scans->getFlair();
    secondImage = scans->getT1();
    firstDialog->setImage(firstImage);
    secondDialog->setImage(secondImage);
    fillUi();

    setDataChangedThis(false);
}

QString ScansDialog::getT1Name() const
{
    return secondImage->getFileName();
}

QString ScansDialog::getFlairName() const
{
    return firstImage->getFileName();
}

