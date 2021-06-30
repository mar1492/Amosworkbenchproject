#ifndef NODEGRAPH_H
#define NODEGRAPH_H

#include <QString>
#include <QColor>

class NodeGraph
{
public:
    NodeGraph();
    NodeGraph(QString uuid, QString label, QColor color);
//    NodeGraph(const NodeGraph &other);

    bool operator == (const NodeGraph & nodeGraph);

    QString getUuid() const;
    QString getLabel() const;
    void setLabel(const QString &label);
    QColor getColor() const;
    void setColor(const QColor &color);

protected:
    QString uuid;
    QString label;
    QColor color;

};

#endif // NODEGRAPH_H
