#ifndef RIGHTCLICKMENU_H
#define RIGHTCLICKMENU_H

#include <QMenu>

class QMouseEvent;

class RightCLickMenu : public QMenu
{
public:
    enum ActionTypes {Add, Remove, Save, Load, New, Restore, WarningBox, Html};
    RightCLickMenu(QWidget *parent = Q_NULLPTR);
    RightCLickMenu(QString &title, QWidget *parent = Q_NULLPTR);

    void setActions(QList<QAction*> actions);
    static QString getMenuActionText(ActionTypes type);
    static void addMenuAction(ActionTypes type, QAction* action);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    static QMap<ActionTypes, QAction*> menuActions;
};

#endif // RIGHTCLICKMENU_H
