#ifndef ZOOMWEBVIEW_HXX
#define ZOOMWEBVIEW_HXX

#include <QWebEngineView>

class ZoomWebView : public QWebEngineView
{

    Q_OBJECT

public:
    ZoomWebView(QWidget* parent = Q_NULLPTR);

public slots:
    void back();
    void forward();
    void reload();

protected:
    void wheelEvent(QWheelEvent * event);

signals:
    void reloadHtml();

protected:
    qreal fontFactor;
    QMenu* contextMenu;
//    QAction* reloadAction;
//    QAction* backAction;
//    QAction* forwardAction;
//    QAction* copyAction;
};

#endif // ZOOMWEBVIEW_HXX
