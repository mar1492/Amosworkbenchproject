#include "projectpage.h"

ProjectPage::ProjectPage(QWidget *parent) : QWidget(parent)
{

}

void ProjectPage::enterEvent(QEvent */*event*/)
{
    emit emitActivated(true, windowTitle());
}

void ProjectPage::leaveEvent(QEvent */*event*/)
{
    emit emitActivated(false, windowTitle());
}
