#include "rightclickmenu.h"
#include <QMouseEvent>
#include <QToolTip>

QMap<RightCLickMenu::ActionTypes, QAction*> RightCLickMenu::menuActions;

RightCLickMenu::RightCLickMenu(QWidget *parent) : QMenu(parent)
{

}

RightCLickMenu::RightCLickMenu(QString &title, QWidget *parent) : QMenu(title, parent)
{

}

void RightCLickMenu::setActions(QList<QAction *> actions)
{
    clear();
    addActions(actions);
}

QString RightCLickMenu::getMenuActionText(RightCLickMenu::ActionTypes type)
{
    QString typeName = "";
    switch (type) {
        case Add:
            typeName = "addElement";
            break;
        case Remove:
            typeName = "removeElement";
            break;
        case Save:
            typeName = "saveElement";
            break;
        case Load:
            typeName = "loadElement";
            break;
        case New:
            typeName = "newElement";
            break;
        case Restore:
            typeName = "restoreElement";
            break;
        case WarningBox:
            typeName = "warningBox";
            break;
        case Html:
            typeName = "generateHtml";
            break;
    }
    return typeName;
}

void RightCLickMenu::addMenuAction(ActionTypes type, QAction *action)
{
    menuActions.insert(type, action);
}

void RightCLickMenu::mousePressEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::RightButton) {
        if(activeAction()) {
            QToolTip::showText(e->globalPos(), activeAction()->toolTip());
            setActiveAction(0);
//            e->ignore();
        }
    }
    else
        QMenu::mousePressEvent(e);
}

void RightCLickMenu::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->buttons() == Qt::RightButton) {
        if(activeAction()) {
            QToolTip::hideText();
            setActiveAction(0);
        }
    }
    else
        QMenu::mouseReleaseEvent(e);
}
