#ifndef SANKEYDIAGRAM_H
#define SANKEYDIAGRAM_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QColor>
#include <vector>


struct SankeyLink {
    int sourceIndex;
    int targetIndex;
    double value;
};

struct SankeyNode {
    QString name;
    double value;
    QColor color;
    int layer;
    QVector<SankeyLink> inLinks;
    QVector<SankeyLink> outLinks;
};


class SankeyDiagram : public QGraphicsView {
    Q_OBJECT

public:
    explicit SankeyDiagram(QWidget *parent = nullptr);

    void setNodes(const std::vector<SankeyNode>& nodes);
    void setLinks(const std::vector<SankeyLink>& links);
    void addNode(QString name, double value, QColor color, int layer);
    void addLink(QString source, QString target, double value);
    void drawDiagram();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QGraphicsScene *m_scene;
    std::vector<SankeyNode> m_nodes;
    std::vector<SankeyLink> m_links;

    void calculateNodePositions(QVector<QPair<qreal, qreal>>& nodePositions,QVector<qreal>& nodeHeights);
    void drawNodes(const QVector<QPair<qreal, qreal>>& nodePositions, QVector<qreal>& nodeHeights);
    void drawLinks(const QVector<QPair<qreal, qreal>>& nodePositions, QVector<qreal>& nodeHeights);
};

#endif // SANKEYDIAGRAM_H