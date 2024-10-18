#pragma once

#include "motion/algorithms/find_methods/find_method.h"

#include <QPointF>
#include <vector>

class QGraphicsPathItem;

namespace Motion
{

class Graph
{
public:
    struct Node
    {
        QPointF point;
        size_t index;
    };

    Graph();
    Graph(const std::vector<QPointF>& vertices);
    int addVertex(const QPointF& vertex);
    void addEdge(size_t from, size_t to);
    Node nearest(const QPointF& vertex);
    Path findPath(size_t startPoint, size_t endPoint);
    void setMask(const Polygon& polygon);
    size_t size();

    std::vector<QPointF> getVertices() const;
    std::vector<std::vector<size_t>> getAdjacencyList() const;

    QGraphicsPathItem* asGraphicsItems();
private:
    std::vector<std::vector<size_t>> m_adjacencyList;
    std::vector<QPointF> m_vertices;
};

}  // namespace Motion