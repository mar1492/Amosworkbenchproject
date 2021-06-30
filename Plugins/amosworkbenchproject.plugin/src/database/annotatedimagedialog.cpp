#include "annotatedimagedialog.h"
#include "ui_twoimagesdialog.h"
#include "imagedialog.h"
#include "amosimage.h"
#include "reconstructioninputlist.h"
#include "qvariantptr.h"

AnnotatedImageDialog::AnnotatedImageDialog(QWidget *parent) :
    TwoImagesDialog(parent)
{
    firstTitle = tr("Expert image");
    secondTitle = tr("Mask image");

    saveActionText = tr("Update annotated images");
    saveActionToolTip = tr("Update annotated images information");

    restoreActionText = tr("Restore annotated images");
    restoreActionToolTip = tr("Restore annotated images information\nto previously saved");

}

AnnotatedImageDialog::~AnnotatedImageDialog()
{
    qDebug("Deleting AnnotatedImageDialog");
}

bool AnnotatedImageDialog::setUpDialog()
{
    bool ret = TwoImagesDialog::setUpDialog();
    showPathWidget(false);
//    firstDialog->setAnyFile(false);
//    secondDialog->setAnyFile(false);
    return ret;
}

void AnnotatedImageDialog::changeEvent(QEvent *e)
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

void AnnotatedImageDialog::saveThis()
{
    qDebug("Saving AnnotatedImageDialog");
    TwoImagesDialog::saveThis();
}
