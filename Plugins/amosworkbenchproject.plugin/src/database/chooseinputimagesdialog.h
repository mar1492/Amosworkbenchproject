#ifndef CHOOSEINPUTIMAGESDIALOG_H
#define CHOOSEINPUTIMAGESDIALOG_H

#include <qewextensibledialog.h>
#include "reconstructioninputlist.h"

class AmosImage;

namespace Ui {
class ChooseInputImagesDialog;
}

class ChooseInputImagesDialog : public QewExtensibleDialog
{
    Q_OBJECT

public:
    explicit ChooseInputImagesDialog(QWidget *parent = 0);
    ~ChooseInputImagesDialog();

    void setSelectableImages(QList<ReconstructionInputInfo> list);
    QList<ReconstructionInputInfo> getSelectedImages();
    void hideSource();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ChooseInputImagesDialog *ui;
    QList<ReconstructionInputInfo> listInputInfo;
};

#endif // CHOOSEINPUTIMAGESDIALOG_H
