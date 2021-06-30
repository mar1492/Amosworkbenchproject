#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QFileDialog>
#include <QIcon>

class FileDialog : public QFileDialog
{
    Q_OBJECT
public:
    FileDialog(QWidget *parent, Qt::WindowFlags flags);
    FileDialog(QWidget *parent = Q_NULLPTR, const QString &caption = QString(),
               const QString &directory = QString(), const QString &filter = QString());
    ~FileDialog();

    void setTopDir(QString topDir);

protected slots:
    void restrictDir(QString dir);

protected:
    void init();

protected:
    QString topDir;
};

#endif // FILEDIALOG_H
