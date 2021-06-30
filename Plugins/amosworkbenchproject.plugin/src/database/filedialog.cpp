#include "filedialog.h"

#include <QFileIconProvider>
#include <QMessageBox>

FileDialog::FileDialog(QWidget *parent, Qt::WindowFlags flags) : QFileDialog(parent, flags)
{
    init();
}

FileDialog::FileDialog(QWidget *parent, const QString &caption,
                       const QString &directory, const QString &filter) :
                                QFileDialog(parent, caption, directory, filter)
{
    init();
}

FileDialog::~FileDialog()
{
}

void FileDialog::init()
{
#if defined(Q_OS_LINUX) // due to in KDE signal to restrictDir() does not work
    setOption(QFileDialog::DontUseNativeDialog);
#endif    
}

void FileDialog::setTopDir(QString topDir)
{
    this->topDir = topDir;
    if(!topDir.isEmpty())
//        connect(this, SIGNAL(directoryEntered(QString)), this, SLOT(restrictDir(QString)));
        connect(this, SIGNAL(currentChanged(QString)), this, SLOT(restrictDir(QString)));
}

void FileDialog::restrictDir(QString dir)
{
    if(dir.trimmed().isEmpty())
        return;
    QFileInfo fileInfo(dir);
    if(fileInfo.isFile())
        dir = fileInfo.absolutePath();
    if(!topDir.isEmpty() && dir.left(topDir.size()) != topDir) {
        setDirectory(topDir);
        QMessageBox::warning(this, tr("Top directory set"), tr("Cannot go out ") + topDir);
    }
//     else
//         setDirectory(dir);

}
