#pragma once

#include "motion/structures/graph.h"
#include "motion/structures/polygon.h"
#include "motion/algorithms/find_methods/find_method.h"

#include <optional>

namespace Motion
{

class PreprocessedGraph : public IFindMethod
{
public:
    Path findPath(
        const QPointF& startPoint,
        const QPointF& endPoint,
        const PolygonSet& obstacles
    ) override;

    QGraphicsPathItem* getPathMap() override;
    void setMask(const Polygon& polygon) override;
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
    std::optional<Polygon> m_mask;
    size_t m_nObstacleCount = 0;
};

}  // namespace Motion