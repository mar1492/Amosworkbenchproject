#include "nodegraph.h"
#include <QUuid>

NodeGraph::NodeGraph() :
    uuid(QUuid().toString()), label("label"), color(Qt::white)
{

}

NodeGraph::NodeGraph(QString uuid, QString label, QColor color) :
        uuid(uuid), label(label), color(color)
{

}

//NodeGraph::NodeGraph(const NodeGraph &other)
//{

//}

bool NodeGraph::operator == (const NodeGraph &nodeGraph)
{
    bool ret = (uuid == nodeGraph.getUuid());
    return ret;
}

QString NodeGraph::getUuid() const
{
    return uuid;
}

QString NodeGraph::getLabel() const
{
    return label;
}

void NodeGraph::setLabel(const QString &label)
{
    this->label = label;
}

QColor NodeGraph::getColor() const
{
    return color;
}

void NodeGraph::setColor(const QColor &color)
{
    this->color = color;
}
