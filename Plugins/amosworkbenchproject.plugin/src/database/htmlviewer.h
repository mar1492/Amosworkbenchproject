#ifndef HTMLVIEWER_H
#define HTMLVIEWER_H

#include <QWidget>
#include "zoomwebview.h"
#include <qobjectdefs.h>

namespace Ui {
class HtmlViewer;
}

class HtmlViewer : public QWidget
{
    Q_OBJECT

public:
    explicit HtmlViewer(QWidget *parent = 0);
    ~HtmlViewer();

    void setHtml(const QString & html);

protected:
    void changeEvent(QEvent *e);

protected slots:
    void print();
    void save();
    void reload();
    void handleHtml(QString html);

signals:
//    void save();
    void reloadHtml();
    void getHtml(QString html);

protected:
    ZoomWebView* webview;

private:
    Ui::HtmlViewer *ui;
};

#endif // HTMLVIEWER_H
