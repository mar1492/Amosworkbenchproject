#include "savemenutextedit.h"
#include <QMenu>
#include <QContextMenuEvent>

SaveMenuTextEdit::SaveMenuTextEdit(QWidget *parent) : QTextEdit(parent)
{

}

SaveMenuTextEdit::SaveMenuTextEdit(const QString &text, QWidget *parent) : QTextEdit(text, parent)
{

}

void SaveMenuTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    QAction* actionSave = menu->addAction(tr("Save"));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(onSaveText()));
    menu->exec(event->globalPos());
    delete menu;
}

void SaveMenuTextEdit::onSaveText()
{
    emit saveText(toPlainText());
}

