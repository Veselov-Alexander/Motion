#include "algorithms/find_methods/visibility_graph.h"

#include "algorithms/Utils.h"

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
        if (auto point = shifted(j, points, polygon))
            addPoint(*point);
    }
}
