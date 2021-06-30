#ifndef SAVEMENUTEXTEDIT_H
#define SAVEMENUTEXTEDIT_H

#include <QTextEdit>

class SaveMenuTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    SaveMenuTextEdit(QWidget * parent = 0);
    SaveMenuTextEdit(const QString & text, QWidget * parent = 0);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

protected slots:
    void onSaveText();

signals:
    void saveText(const QString&);

};

#endif // SAVEMENUTEXTEDIT_H
