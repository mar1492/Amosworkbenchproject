#ifndef SCANSDIALOG_H
#define SCANSDIALOG_H

#include "twoimagesdialog.h"

class Scans;

class ScansDialog : public TwoImagesDialog
{
    Q_OBJECT

public:
    explicit ScansDialog(QWidget *parent = 0);
    ~ScansDialog();

    bool setUpDialog();

    QString getFlairName() const;
    QString getT1Name() const;

    Scans *getScans() const;
    void setScans(Scans *scans);       

    void setSubjectsGenerated(bool subjectsGenerated);    

protected:
    void changeEvent(QEvent *e);
    void saveThis( void );
    void preDialogActivated(bool activated, QewDialogInactivate* from);
    void postDialogActivated(bool response);

signals:
    void generateSubjectsAgain();

protected:
    Scans* scans;
    bool subjectsGenerated;
    bool subjectsGenAgainMaybe;

//private:
//    Ui::ScansDialog *ui;
};

#endif // SCANSDIALOG_H
