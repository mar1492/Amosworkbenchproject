#include "messagestouser.h"
// #include "ui_messagestouser.h"

MessagesToUser::MessagesToUser(QWidget* /*parent*/)
{
    ui = new Ui::MessagesToUser;
    ui->setupUi(this);
}

MessagesToUser::~MessagesToUser()
{
    delete ui;
}


void MessagesToUser::initNewMessage()
{
    if(!ui->textBrowser->toPlainText().isEmpty())
        ui->textBrowser->append("\n    ///////////////////\n");
}

void MessagesToUser::writeMessage(QVariant textVariant)
{
    QStringList textList = textVariant.toStringList();
    if(textList.size() > 0)
        initNewMessage();
    for(int i = 0; i < textList.size(); i++)
        ui->textBrowser->append(textList.at(i));
    show();
}

void MessagesToUser::appendText(QString text)
{
    ui->textBrowser->append(text);
}

void MessagesToUser::clear()
{
    ui->textBrowser->clear();
}

void MessagesToUser::setVisible(bool visible)
{
    if(!visible)
	ui->textBrowser->clear();
    QWidget::setVisible(visible);
}
