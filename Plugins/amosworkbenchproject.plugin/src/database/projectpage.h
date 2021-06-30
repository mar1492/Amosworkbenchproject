#ifndef PROJECTPAGE_H
#define QEWTREEPAGE_H

#include <QWidget>

class ProjectPage : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectPage(QWidget *parent = 0);

signals:
    void emitActivated(bool, const QString&);
public slots:

protected:
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
};

#endif // QEWTREEPAGE_H
