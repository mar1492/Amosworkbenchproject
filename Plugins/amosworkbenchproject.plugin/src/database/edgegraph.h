#ifndef EDGEGRAPH_H
#define EDGEGRAPH_H

#include <QString>
#include <QColor>

class EdgeGraph
{
public:
    EdgeGraph(QString uuidIn, QString uuidOut, QColor color);

    bool operator == (const EdgeGraph &edgeGraph);

    QString getUuidIn() const;
    QString getUuidOut() const;
    QColor getColor() const;
    void setColor(const QColor &color);

protected:
    QString uuidIn;
    QString uuidOut;
    QColor color;
};

#endif // EDGEGRAPH_H
