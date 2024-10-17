#include "Algorithms/FindMethods/VoronoiMap.h"
#include "Algorithms/Voronoi.h"

namespace std
{
    template <> struct hash<QPointF>
    {
        inline size_t operator()(const QPointF& v) const
        {
            std::hash<double> int_hasher;
            return int_hasher(v.x()) ^ int_hasher(v.y());
        }
    };

}

VoronoiMap::VoronoiMap(int nWidth, int nHeight, int nPoints)
    : m_nWidth(nWidth), m_nHeight(nHeight), m_nPoints(nPoints)
{
    m_bClosest = true;
}

void VoronoiMap::createGraph(const PolygonSet& obstacles)
{
    m_graph = Graph();
    m_indexes.clear();
    m_nObstacleCount = obstacles.size();

    std::vector<QLineF> lines = voronoiDiagram_3(obstacles.lines());


    auto equalsDouble = [](double a, double b)
    {
        return std::fabs(a - b) < 1e-2;
    };

    auto equalsPoint = [equalsDouble](const QPointF& left, const QPointF& right)
    {
        return equalsDouble(left.x(), right.x()) && equalsDouble(left.y(), right.y());
    };

    auto lessPoint = [equalsDouble](const QPointF& left, const QPointF& right)
    {
        if (equalsDouble(left.x(), right.x()))
        {
            return left.y() < right.y();
        }

        return left.x() < right.x();
    };

    auto compareQLineF = [equalsPoint, lessPoint](const QLineF& left, const QLineF& right)
    {
        if (equalsPoint(left.p1(), right.p1()))
            return lessPoint(left.p2(), right.p2());
        return lessPoint(left.p1(), right.p1());
    };

    for (auto& line : lines)
    {
        auto p1 = line.p1();
        auto p2 = line.p2();
        if (lessPoint(p1, p2))
        {
            line.setP1(p2);
            line.setP2(p1);
        }
    }

    std::sort(lines.begin(), lines.end(), compareQLineF);
    auto end = std::unique(lines.begin(), lines.end());
    lines.erase(end, lines.end());

    m_points.clear();
    m_points.reserve(lines.size());

    for (auto it = lines.begin(); it != lines.end(); )
    {
        if (obstacles.inside(it->p1()) || obstacles.inside(it->p2()))
        {
            it = lines.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for (auto& line : lines)
    {
        m_points.push_back(line.p1());
        m_points.push_back(line.p2());
    }

    std::sort(m_points.begin(), m_points.end(), lessPoint);
    auto end2 = std::unique(m_points.begin(), m_points.end());
    m_points.erase(end2, m_points.end());

    std::unordered_map<QPointF, int> indexes;

    for (const auto& point : m_points)
    {
        indexes[point] = m_graph.addVertex(point);
    }

    for (const auto& line : lines)
    {
        if (!obstacles.intersects(line, true))
        {
            m_graph.addEdge(indexes[line.p1()], indexes[line.p2()]);
        }
    }
}
