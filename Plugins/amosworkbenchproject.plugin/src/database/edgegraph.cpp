#include "edgegraph.h"

EdgeGraph::EdgeGraph(QString uuidIn, QString uuidOut, QColor color) :
    uuidIn(uuidIn), uuidOut(uuidOut), color(color)
{

}

bool EdgeGraph::operator ==(const EdgeGraph &edgeGraph)
{
    bool ret = (uuidIn == edgeGraph.getUuidIn() && uuidOut == edgeGraph.getUuidOut());
    return ret;
}

QString EdgeGraph::getUuidIn() const
{
    return uuidIn;
}

QString EdgeGraph::getUuidOut() const
{
    return uuidOut;
}

QColor EdgeGraph::getColor() const
{
    return color;
}

void EdgeGraph::setColor(const QColor &color)
{
    this->color = color;
}
