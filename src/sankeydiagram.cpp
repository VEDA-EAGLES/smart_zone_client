#include "sankeydiagram.h"
#include <QPen>
#include <QBrush>
#include <QRandomGenerator>
#include <QDebug>
#include <QGRaphicsTextItem>

SankeyDiagram::SankeyDiagram(QWidget *parent)
    : QGraphicsView(parent),
    m_scene(new QGraphicsScene(this))
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_title = "Sankey Diagram";
    titleHeight = 0;
}

void SankeyDiagram::setNodes(const std::vector<SankeyNode>& nodes) {
    m_nodes = nodes;
}

void SankeyDiagram::setLinks(const std::vector<SankeyLink>& links) {
    m_links = links;
}

void SankeyDiagram::addNode(QString name, double value, QColor color, int layer) {
    m_nodes.push_back({name, value, color, layer});
}

void SankeyDiagram::addLink(QString source, QString target, double value) {
    int sourceIndex = -1;
    int targetIndex = -1;
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].name == source) {
            sourceIndex = i;
        }
        if (m_nodes[i].name == target) {
            targetIndex = i;
        }
    }

    if (sourceIndex == -1 || targetIndex == -1) {
        qDebug() << "Invalid source or target node";
        return;
    }

    m_links.push_back({sourceIndex, targetIndex, value});

    m_nodes[sourceIndex].outLinks.push_back({sourceIndex, targetIndex, value});
    m_nodes[targetIndex].inLinks.push_back({sourceIndex, targetIndex, value});
}

void SankeyDiagram::calculateNodePositions(QVector<QPair<qreal, qreal>>& nodePositions, QVector<qreal>& nodeHeights) {
    // Reset containers
    nodePositions.resize(m_nodes.size());
    nodeHeights.resize(m_nodes.size());

    // Create a map to group nodes by layer
    std::map<int, std::vector<size_t>> layerNodes;
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        layerNodes[m_nodes[i].layer].push_back(i);
    }

    // Find max layer to determine horizontal positioning
    int maxLayer = std::max_element(
                       layerNodes.begin(), layerNodes.end(),
                       [](const auto& a, const auto& b) { return a.first < b.first; }
                       )->first;

    double totalHeight = height() * 0.9;
    double currentY = height() * 0.1;
    double spacing = 5.0;

    if (currentY < titleHeight + 25) {
        currentY = titleHeight + 25;
    }
    totalHeight -= currentY;

    for (int layer = 0; layer <= maxLayer; ++layer) {
        // Calculate total value for nodes in this layer
        double layerTotalValue = spacing * (layerNodes[layer].size() - 1);
        for (size_t nodeIndex : layerNodes[layer]) {
            layerTotalValue += m_nodes[nodeIndex].value;
        }

        // Horizontal positioning based on layer
        double layerX = width() * (layer / static_cast<double>(maxLayer + 1)) + width() * 0.1;

        // Reset Y for this layer
        double currentLayerY = currentY;

        // Position nodes within this layer
        for (size_t nodeIndex : layerNodes[layer]) {
            // Calculate node height proportional to its value
            double nodeHeight = (m_nodes[nodeIndex].value / layerTotalValue) * totalHeight;

            // Store node position
            nodePositions[nodeIndex] = {layerX, currentLayerY};
            nodeHeights[nodeIndex] = nodeHeight;

            // Increment Y for next node
            currentLayerY += nodeHeight + (spacing / layerTotalValue) * totalHeight;
        }
    }
}

void SankeyDiagram::drawNodes(const QVector<QPair<qreal, qreal>>& nodePositions, QVector<qreal>& nodeHeights) {
    m_scene->clear();
    double nodeWidth = 40.0;

    for (size_t i = 0; i < m_nodes.size(); ++i) {
        double nodeHeight = nodeHeights[i];

        // Use provided color or generate one
        QColor nodeColor = m_nodes[i].color;

        QGraphicsRectItem* rect = m_scene->addRect(
            nodePositions[i].first, nodePositions[i].second,
            nodeWidth, nodeHeight,
            QPen(nodeColor),
            QBrush(nodeColor)
        );

        // Add text label
        QGraphicsTextItem* label = m_scene->addText(m_nodes[i].name);
        label->setPos(nodePositions[i].first + nodeWidth + 10, nodePositions[i].second + nodeHeight / 2 - 10);
    }
}

void SankeyDiagram::drawLinks(const QVector<QPair<qreal, qreal>>& nodePositions, QVector<qreal>& nodeHeights) {
    double nodeWidth = 40.0;
    QMap<int, double> targetCurrentY;
    for (int i = 0; i < m_nodes.size(); ++i) {
        const auto& node = m_nodes[i];
        double sourceCurrentY = nodePositions[i].second;
        // qsort(node.outLinks.begin(), node.outLinks.end(), [](const auto& a, const auto& b) {
        //     return a.targetIndex > b.targetIndex;
        // });
        double sourceTotalValue = 0.0;
        for (const auto& link : node.outLinks) {
            sourceTotalValue += link.value;
            if (!targetCurrentY[link.targetIndex]) targetCurrentY[link.targetIndex] = nodePositions[link.targetIndex].second;
        }

        for (const auto& link : node.outLinks) {
            double targetTotalValue = 0.0;
            for (const auto& targetLink : m_nodes[link.targetIndex].inLinks) {
                targetTotalValue += targetLink.value;
            }

            double sourceLinkWidth = (link.value / sourceTotalValue) * nodeHeights[link.sourceIndex];
            double targetLinkWidth = (link.value / targetTotalValue) * nodeHeights[link.targetIndex];

            double sourceX = nodePositions[link.sourceIndex].first + nodeWidth;
            double sourceY = sourceCurrentY;
            double targetX = nodePositions[link.targetIndex].first;
            double targetY = targetCurrentY[link.targetIndex];

            sourceCurrentY += sourceLinkWidth;
            targetCurrentY[link.targetIndex] += targetLinkWidth;

            QPainterPath path;
            path.moveTo(sourceX, sourceY);
            path.cubicTo(
                (sourceX + targetX) / 2.0, sourceY,  // First control point
                (sourceX + targetX) / 2.0, targetY,  // Second control point
                targetX, targetY  // End point
                );
            path.lineTo(targetX, targetY + targetLinkWidth);
            path.cubicTo(
                (sourceX + targetX) / 2.0, targetY + targetLinkWidth,
                (sourceX + targetX) / 2.0, sourceY + sourceLinkWidth,
                sourceX, sourceY + sourceLinkWidth
                );
            path.closeSubpath();

            // Generate a color for the link
            QColor linkColor = m_nodes[link.sourceIndex].color;

            QGraphicsPathItem* linkPath = m_scene->addPath(
                path,
                QPen(linkColor, 0, Qt::SolidLine, Qt::FlatCap),
                QBrush(linkColor)
                );
            linkPath->setOpacity(0.5);
        }
    }
}

void SankeyDiagram::drawTitle() {
    QFont font;
    font.setPixelSize(28);
    QGraphicsTextItem* titleItem = m_scene->addText(m_title, font);
    titleItem->setPos(width() / 2 - titleItem->boundingRect().width() / 2, 15);
    titleHeight = titleItem->boundingRect().height();
}

void SankeyDiagram::drawDiagram() {
    QVector<QPair<qreal, qreal>> nodePositions;
    QVector<qreal> nodeHeights;
    calculateNodePositions(nodePositions, nodeHeights);
    drawNodes(nodePositions, nodeHeights);
    drawLinks(nodePositions, nodeHeights);
    drawTitle();
}

void SankeyDiagram::setTitle(const QString& title) {
    m_title = title;
}

void SankeyDiagram::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    m_scene->setSceneRect(0, 0, width(), height());
    drawDiagram();
}
