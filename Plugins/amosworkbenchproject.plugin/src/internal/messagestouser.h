#ifndef MESSAGESTOUSER_H
#define MESSAGESTOUSER_H

#include "ui_messagestouser.h"

#include <QWidget>

namespace Ui
{
class MessagesToUser;
}

class MessagesToUser : public QWidget
{
    Q_OBJECT
public:
    MessagesToUser(QWidget* parent = 0);
    ~MessagesToUser();
    
    void initNewMessage();
    void writeMessage(QVariant textVariant);
    void appendText(QString text);
    void clear();
    
public slots:
    void setVisible(bool visible);

private:
    Ui::MessagesToUser* ui;
};

#endif // MESSAGESTOUSER_H
