#include "PreprocessedGraph.h"

#include "Core/DisplayView.h"
#include "Core/Algorithms/Dijkstra.h"

#include <vector>

Path PreprocessedGraph::findPath(const QPointF& startPoint, const QPointF& endPoint,
    const PolygonSet& obstacles)
{
    if (m_nObstacleCount != obstacles.size())
    {
        createGraph(obstacles);
    }

    m_extGraph = m_graph;

    const size_t startIndex = m_extGraph.addVertex(startPoint);
    const size_t endIndex = m_extGraph.addVertex(endPoint);


    if (!obstacles.intersects({ startPoint, endPoint }, true))
    {
        return { startPoint, endPoint };
    }

    if (obstacles.inside(startPoint))
    {
        return INVALID_PATH;
    }

    m_specialPoints.clear();

    m_specialPoints.push_back({ startIndex, startPoint });
    m_specialPoints.push_back({ endIndex, endPoint });

    if (!m_bClosest)
    {
        for (const auto& pair : m_specialPoints)
        {
            for (int i = 0; i < m_points.size(); ++i)
            {
                QLineF line(pair.second, m_points[i]);
                if (!obstacles.intersects(line, true))
                {
                    m_extGraph.addEdge(pair.first, i);
                }
            }
        }
    }
    else
    {
        for (const auto& pair : m_specialPoints)
        {
            int minI = 0;
            qreal minLength = QLineF(pair.second, m_points[minI]).length();

            for (int i = 0; i < m_points.size(); ++i)
            {
                QLineF line(pair.second, m_points[i]);
                if (!obstacles.intersects(line, true))
                {
                    qreal currentLength = line.length();
                    if (currentLength < minLength)
                    {
                        minLength = currentLength;
                        minI = i;
                    }
                }
            }
            m_extGraph.addEdge(pair.first, minI);
        }
    }

    return dijkstraShortestPath(startIndex, endIndex, m_extGraph);
}

QGraphicsPathItem* PreprocessedGraph::getPathMap()
{
    return m_extGraph.asGraphicsItems();
}

void PreprocessedGraph::addPoint(const QPointF& point)
{
    m_graph.addVertex(point);
    m_points.push_back(point);
}

