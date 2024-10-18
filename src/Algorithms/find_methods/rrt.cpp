#include "motion/algorithms/find_methods/RRT.h"

#include "motion/algorithms/utils.h"
#include <QtMath>

#include <vector>

namespace Motion
{

RRT::RRT(int nWidth, int nHeight,
         int nMaxIterations, int nMaxDistance,
         double biasProb) :
    m_nWidth(nWidth),
    m_nHeight(nHeight),
    m_nMaxIterations(nMaxIterations),
    m_nMaxDistance(nMaxDistance),
    m_biasProb(biasProb)
{

}

QPointF RRT::generatePoint(const QPointF& end)
{
    if (static_cast<double>(rand()) / RAND_MAX < m_biasProb)
    {
        return end;
    }
    else
    {
        qreal x = rand() % m_nWidth - m_nWidth / 2;
        qreal y = rand() % m_nHeight - m_nHeight / 2;

        return { x, y };
    }
}

QPointF RRT::cropLine(const QPointF& start, const QPointF& end)
{
    if (euclideanDist(start, end) < m_nMaxDistance)
    {
        return end;
    }
    else
    {
        qreal theta = qAtan2(end.y() - start.y(), end.x() - start.x());

        return QPointF(start.x() + m_nMaxDistance * cos(theta),
                       start.y() + m_nMaxDistance * sin(theta));
    }
}

Path RRT::findPath(const QPointF& startPoint,
                   const QPointF& endPoint,
                   const PolygonSet& obstacles)
{
    m_tree = Graph({ startPoint });

    for (int i = 0; i <= m_nMaxIterations; ++i)
    {
        assert(i != m_nMaxIterations);
        if (i == m_nMaxIterations)
        {
            return {};
        }

        QPointF randomPoint = generatePoint(endPoint);
        Graph::Node nearestPoint = m_tree.nearest(randomPoint);
        QPointF newPoint = cropLine(nearestPoint.point, randomPoint);

        if (obstacles.intersects(QLineF(nearestPoint.point, newPoint)))
        {
            continue;
        }

        size_t newVertexIndex = m_tree.addVertex(newPoint);
        m_tree.addEdge(newVertexIndex, nearestPoint.index);

        if (newPoint == endPoint)
        {
            break;
        }
    }
    return m_tree.findPath(0, m_tree.size() - 1);
}

QGraphicsPathItem* RRT::getPathMap()
{
    return m_tree.asGraphicsItems();
}

void RRT::setMask(const Polygon& polygon)
{
  // Not needed
}

}  // namespace Motion