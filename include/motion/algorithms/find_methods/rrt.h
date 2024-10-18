#pragma once

#include "motion/structures/graph.h"
#include "motion/algorithms/find_methods/find_method.h"

namespace Motion
{

class RRT : public IFindMethod
{
public:
    RRT(int nWidth, int nHeight,
        int nMaxIterations=10000, int nMaxDistance=50,
        double biasProb = 0.05);

    Path findPath(
        const QPointF& startPoint,
        const QPointF& endPoint,
        const PolygonSet& obstacles
    ) override;

    QGraphicsPathItem* getPathMap() override;

    void setMask(const Polygon& polygon) override;

private:
    QPointF generatePoint(const QPointF& end);
    QPointF cropLine(const QPointF& start, const QPointF& end);
private:
    double m_biasProb;
    int m_nMaxIterations;
    int m_nMaxDistance;
    int m_nWidth;
    int m_nHeight;

    Graph m_tree;
};

}  // namespace Motion