#include "graphvizdialog.h"
#include "ui_graphvizdialog.h"

#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#include "QGVSubGraph.h"
#include <QMessageBox>
#include <QMenu>
#include <QTimer>

GraphVizDialog::GraphVizDialog(QWidget *parent) :
    QewExtensibleDialog(parent),
    ui(new Ui::GraphVizDialog)
{
    ui->setupUi(this);
    _scene = 0;

//    _scene = new QGVScene("DEMO", this);
//    ui->graphicsView->setScene(_scene);

//    connect(_scene, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
//    connect(_scene, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));

//    initGraph();
}

GraphVizDialog::~GraphVizDialog()
{
    delete ui;
}

void GraphVizDialog::initGraph()
{
    _scene = new QGVScene("DEMO", this);
    ui->graphicsView->setScene(_scene);

    connect(_scene, SIGNAL(nodeContextMenu(QGVNode*)), SLOT(nodeContextMenu(QGVNode*)));
    connect(_scene, SIGNAL(nodeDoubleClick(QGVNode*)), SLOT(nodeDoubleClick(QGVNode*)));
//     connect(_scene, SIGNAL(graphContextMenuEvent()), SLOT(viewContextMenu()));
    
    //Configure scene attributes
    _scene->setGraphAttribute("label", " ");
//    _scene->setGraphAttribute("bgcolor", "white");
//    _scene->setGraphAttribute("splines", "ortho");
    _scene->setGraphAttribute("rankdir", "TB");
    //_scene->setGraphAttribute("concentrate", "true"); //Error !
    _scene->setGraphAttribute("nodesep", "0.4");

    _scene->setNodeAttribute("shape", "ellipse");
    _scene->setNodeAttribute("style", "filled");
    _scene->setNodeAttribute("fillcolor", "white");
    _scene->setNodeAttribute("margin", "0.2");

//    _scene->setNodeAttribute("height", "1.2");
    _scene->setNodeAttribute("fontsize", "16");
    _scene->setEdgeAttribute("minlen", "3");
    _scene->setNodeAttribute("penwidth", "2");
    _scene->setEdgeAttribute("penwidth", "2");
}

void GraphVizDialog::fillUi()
{
    initGraph();
    //_scene->setEdgeAttribute("dir", "both");
//    _scene->setGraphAttribute("label", "Project");

    //Add some nodes
    QGVSubGraph* sgraph = _scene->addSubGraph("SUB1");
    sgraph->setAttribute("style", "invisible");
    sgraph->setAttribute("label", "");

    QGVNode *node = sgraph->addNode(scansNode.getLabel());
    node->setFlag (QGraphicsItem::ItemIsMovable, false);
    node->setAttribute("fillcolor", addAlphaToColor(scansNode.getColor()));
    nodes.insert(scansNode.getUuid(), node);

    node = sgraph->addNode(annotatedNode.getLabel());
    node->setFlag (QGraphicsItem::ItemIsMovable, false);
    node->setAttribute("fillcolor", addAlphaToColor(annotatedNode.getColor()));
    nodes.insert(annotatedNode.getUuid(), node);

    node = sgraph->addNode(externNode.getLabel());
    node->setFlag (QGraphicsItem::ItemIsMovable, false);
    node->setAttribute("fillcolor", addAlphaToColor(externNode.getColor()));
    nodes.insert(externNode.getUuid(), node);

    for(int i = 0; i < nodesGraph.size(); i++) {
        node = _scene->addNode(nodesGraph.at(i).getLabel());
        node->setFlag (QGraphicsItem::ItemIsMovable, false);
        node->setAttribute("fillcolor", addAlphaToColor(nodesGraph.at(i).getColor()));
        nodes.insert(nodesGraph.at(i).getUuid(), node);
    }

    for(int i = 0; i < edgesGraph.size(); i++) {
        QString uuidIn = edgesGraph.at(i).getUuidIn();
        QGVNode *nodeIn = nodes.value(uuidIn);
        QString uuidOut = edgesGraph.at(i).getUuidOut();
        QGVNode *nodeOut = nodes.value(uuidOut);
        QGVEdge* edge = _scene->addEdge(nodeIn, nodeOut, "");
        edge->setAttribute("color", addAlphaToColor(edgesGraph.at(i).getColor()));
        edges.insert(edgesGraph.at(i).getUuidIn() + edgesGraph.at(i).getUuidOut(), edge);
    }

    //Layout scene
    _scene->applyLayout("dot");

    //Fit in view
    ui->graphicsView->fitInView(_scene->sceneRect(), Qt::KeepAspectRatio);
}

QString GraphVizDialog::addAlphaToColor(QColor color)
{
    // patch for QColor::name() not writing alpha
    QString sColor = color.name();
    if(sColor.size() == 7) {
        int alpha = color.alpha();
        sColor = "#" + QString::number(alpha, 16) + sColor.remove(0, 1);
    }
    return sColor;
}

void GraphVizDialog::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    QAction* actionFit = menu.addAction(tr("FitInView"));
    connect(actionFit, SIGNAL(triggered()), this, SLOT(fitInView()));
    menu.exec(event->globalPos());
//    delete menu;
}

bool GraphVizDialog::dialogActivated(bool activated, QewExtensibleDialog::QewDialogInactivate /*from*/, bool /*noSaveChildren*/)
{
    if(activated) {
        emit emitExternVal("updateGraph", QVariant());
        QTimer::singleShot(100, this, SLOT(delayedFillUi()));
    }
    else
        closeGraph();

    return true;
}

void  GraphVizDialog::closeGraph()
{
    nodes.clear();
    edges.clear();
    nodesGraph.clear();
    edgesGraph.clear();
    if(_scene) {
        _scene->clear();
        _scene->freeLayout();
        delete _scene;
        _scene = 0;
    }    
}

void GraphVizDialog::addNode(NodeGraph nodeGraph)
{
    if(!nodesGraph.contains(nodeGraph))
        nodesGraph.append(nodeGraph);
}

//void GraphVizDialog::removeNode(NodeGraph nodeGraph)
//{
//    if(nodesGraph.contains(nodeGraph)) {
//        for(int i = edgesGraph.size() - 1; i >= 0; i++) {
//            if(edgesGraph.at(i).getUuidIn() == nodeGraph.getUuid() ||
//                    edgesGraph.at(i).getUuidOut() == nodeGraph.getUuid())
//                edgesGraph.removeAt(i);
//        }
//        nodesGraph.removeOne(nodeGraph);
//    }
//}

void GraphVizDialog::addEdge(EdgeGraph edgeGraph)
{
    if(!edgesGraph.contains(edgeGraph))
        edgesGraph.append(edgeGraph);
}

//void GraphVizDialog::removeEdge(EdgeGraph edgeGraph)
//{
//    if(edgesGraph.contains(edgeGraph))
//        edgesGraph.removeOne(edgeGraph);
//}

void GraphVizDialog::changeEvent(QEvent *e)
{
    QewExtensibleDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void GraphVizDialog::nodeContextMenu(QGVNode */*node*/)
{
    //Context menu example
//    QMenu menu(node->label());

//    menu.addSeparator();
//    menu.addAction(tr("Informations"));
//    menu.addAction(tr("Options"));

//    QAction *action = menu.exec(QCursor::pos());
//    if(action == 0)
//        return;
}

void GraphVizDialog::fitInView()
{
    ui->graphicsView->fitInView(_scene->sceneRect(), Qt::KeepAspectRatio);
}

void GraphVizDialog::nodeDoubleClick(QGVNode */*node*/)
{
//    QMessageBox::information(this, tr("Node double clicked"), tr("Node %1").arg(node->label()));
}

void GraphVizDialog::delayedFillUi()
{
   fillUi();
}

void GraphVizDialog::setExternNode(const NodeGraph &externNode)
{
    this->externNode = externNode;
}

QString GraphVizDialog::toSvg()
{
    return ui->graphicsView->toSvg();
}

void GraphVizDialog::setAnnotatedNode(const NodeGraph &annotatedNode)
{
    this->annotatedNode = annotatedNode;
}

void GraphVizDialog::setScansNode(const NodeGraph &scansNode)
{
    this->scansNode = scansNode;
}
