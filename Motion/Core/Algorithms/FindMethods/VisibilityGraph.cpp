#include "VisibilityGraph.h"

namespace
{

std::pair<QPointF, QPointF> shifted(int index, const std::vector<QPointF>& points)
{
    const QPointF& a = points[index - 1 < 0 ? points.size() - 1 : index - 1];
    const QPointF& b = points[index];
    const QPointF& c = points[index + 1 >= points.size() ? 0 : index + 1];

    QPointF diag = (a - b) + (c - b);

    double angle = std::atan2(-diag.y(), -diag.x());

    double area = a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) + c.x() * (a.y() - b.y());

    if (std::abs(area) <= 1e-1)
    {
        throw std::exception();
    }

    const double dist = 5;

    QPointF p1(b.x() - dist * std::cos(angle), b.y() - dist * std::sin(angle));
    QPointF p2(b.x() + dist * std::cos(angle), b.y() + dist * std::sin(angle));

    return { p1, p2 };
}

}

void VisibilityGraph::createGraph(const PolygonSet& obstacles)
{
    m_graph = Graph();
    m_indexes.clear();
    m_nObstacleCount = obstacles.size();

    m_points.clear();

    for (size_t i = 0; i < obstacles.size(); ++i)
    {
        Polygon polygon = obstacles[i];
        addPoints(polygon.points(), polygon);
        for (const auto& hole : polygon.holes())
        {
            addPoints(hole, polygon);
        }
    }

    for (size_t i = 0; i < m_points.size(); ++i)
    {
        for (size_t j = i + 1; j < m_points.size(); ++j)
        {
            QLineF line(m_points[i], m_points[j]);

            if (!obstacles.intersects(line, true))
            {
                m_graph.addEdge(i, j);
            }
        }
    }
}

void VisibilityGraph::addPoints(const std::vector<QPointF> points, const Polygon& polygon)
{
    for (int j = 0; j < points.size(); ++j)
    {
        try
        {
            auto pairs = shifted(j, points);

            if (!polygon.inside(pairs.first))
            {
                addPoint(pairs.first);
            }
            else if (!polygon.inside(pairs.second))
            {
                addPoint(pairs.second);
            }
        }
        catch (...)
        {
            // Points are collinear. Skip
        }
    }
}
