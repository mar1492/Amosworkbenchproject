#ifndef ANNOTATEDIMAGEDIALOG_H
#define ANNOTATEDIMAGEDIALOG_H

#include "twoimagesdialog.h"

class AnnotatedImageDialog : public TwoImagesDialog
{
    Q_OBJECT

public:
    explicit AnnotatedImageDialog(QWidget *parent = 0);
    ~AnnotatedImageDialog();

    bool setUpDialog();

protected:
    void changeEvent(QEvent *e);
    void saveThis();

};

#endif // ANNOTATEDIMAGEDIALOG_H
