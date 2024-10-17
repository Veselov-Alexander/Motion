#pragma once

#include "Structures/Graph.h"
#include "Structures/Polygon.h"
#include "Algorithms/FindMethods/IFindMethod.h"

class PreprocessedGraph : public IFindMethod
{
public:
    virtual Path findPath(
        const QPointF& startPoint,
        const QPointF& endPoint,
        const PolygonSet& obstacles
    ) override;

    virtual QGraphicsPathItem* getPathMap() override;
protected:
    virtual void createGraph(const PolygonSet& obstacles) = 0;
    void addPoint(const QPointF& point);
protected:
    Graph m_graph;
    Graph m_extGraph;
    bool m_bClosest = false;
    std::vector<QPointF> m_points;
    std::vector<std::vector<size_t>> m_indexes;
    std::vector<std::pair<size_t, QPointF>> m_specialPoints;
    size_t m_nObstacleCount = 0;
};

