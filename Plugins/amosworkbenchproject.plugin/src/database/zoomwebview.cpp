#include "zoomwebview.h"

#include <QWheelEvent>
#include <QApplication>
#include <QMenu>

ZoomWebView::ZoomWebView(QWidget *parent) : QWebEngineView(parent)
{
    fontFactor = 1.0;
    QColor colorBack(Qt::white);
    setStyleSheet(QString("background-color:") + colorBack.name());
}

void ZoomWebView::back()
{
    QWebEngineView::back();
}

void ZoomWebView::forward()
{
    QWebEngineView::forward();
}

void ZoomWebView::reload()
{
    emit reloadHtml();
    QWebEngineView::reload();
}

void ZoomWebView::wheelEvent(QWheelEvent *event)
{
    if(QApplication::keyboardModifiers() & Qt::ControlModifier) {
        QPoint numDegrees = event->angleDelta() / 8;
        qreal factor = numDegrees.y()/150.0;
        fontFactor += factor;
        fontFactor = fontFactor < 0.25 ? 0.25 : fontFactor;
        fontFactor = fontFactor > 4.0 ? 4.0 : fontFactor;
//         setTextSizeMultiplier(fontFactor);
        setZoomFactor(fontFactor);
        event->accept();
    }
    else
        QWebEngineView::wheelEvent(event);
}
