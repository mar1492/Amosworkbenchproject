#ifndef LOGSENDER_H
#define LOGSENDER_H

#include <QObject>
#include <AmosWorkbenchLibExports.h>

class AmosWorkbenchLib_EXPORT LogSender : public QObject
{
    Q_OBJECT

public:
    LogSender();
    ~LogSender();
    
    void logText(QString text);
    
signals:
    void sendText(QString text);

private:
};

#endif // LOGSENDER_H
