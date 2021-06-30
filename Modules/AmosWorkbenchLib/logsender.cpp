#include "logsender.h"

LogSender::LogSender()
{
}

LogSender::~LogSender()
{
}

void LogSender::logText(QString text)
{
    emit sendText(text);
}

