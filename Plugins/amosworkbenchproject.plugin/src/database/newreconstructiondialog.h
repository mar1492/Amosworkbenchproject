#ifndef NEWRECONSTRUCTIONDIALOG_H
#define NEWRECONSTRUCTIONDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui
{
class NewReconstructionDialog;
}

class NewReconstructionDialog : public QDialog
{
    Q_OBJECT
    
public:
    
    NewReconstructionDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~NewReconstructionDialog();
    
    QString getID();
    QString getProgramID();
    bool getInternalExec();
    void setInternalExec(bool internal);
    bool getHasPrefixes();
    void setHasPrefixes(bool hasPrefixes);
    void setProgramIDAndInternals(QMap<QString, QPair<QString, QString> > programIDAndInt);
    void hideCustomType();
    
protected slots:
        void programIDChanged(const QString& programID);
    
protected:
    QMap<QString, QPair<QString, QString> > programIDAndInternals;

private:
    Ui::NewReconstructionDialog* ui;
};

#endif // NEWRECONSTRUCTIONDIALOG_H
