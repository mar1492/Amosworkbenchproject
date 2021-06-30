#ifndef GRAPHVIZDIALOG_H
#define GRAPHVIZDIALOG_H

#include <qewextensibledialog.h>
#include <QGVScene.h>
#include "edgegraph.h"
#include "nodegraph.h"

namespace Ui {
class GraphVizDialog;
}

class GraphVizDialog : public QewExtensibleDialog
{
    Q_OBJECT

public:
    explicit GraphVizDialog(QWidget *parent = 0);
    ~GraphVizDialog();    
    bool dialogActivated(bool activated, QewDialogInactivate = FromOutside, bool = false);

    void addNode(NodeGraph nodeGraph);
//    void removeNode(NodeGraph nodeGraph);
    void addEdge(EdgeGraph edgeGraph);
//    void removeEdge(EdgeGraph edgeGraph);

    void setScansNode(const NodeGraph &scansNode);

    void setAnnotatedNode(const NodeGraph &annotatedNode);

    void setExternNode(const NodeGraph &externNode);

    QString toSvg();

protected:
    void changeEvent(QEvent *e);
    void initGraph();
    void closeGraph();
    void fillUi();
    void contextMenuEvent(QContextMenuEvent *event);  
    QString addAlphaToColor(QColor color);

private slots:
    void nodeContextMenu(QGVNode*);
    void fitInView();
    void nodeDoubleClick(QGVNode*);
    void delayedFillUi();

private:
    Ui::GraphVizDialog *ui;
    QHash<QString, QGVNode *> nodes;
    QHash<QString, QGVEdge *> edges;
    QGVScene *_scene;
    NodeGraph scansNode;
    NodeGraph annotatedNode;
    NodeGraph externNode;
    QList<EdgeGraph> edgesGraph;
    QList<NodeGraph> nodesGraph;

};

#endif // GRAPHVIZDIALOG_H
