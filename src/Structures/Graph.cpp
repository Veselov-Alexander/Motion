#include "structures/Graph.h"

#include "algorithms/Utils.h"

#include "QPen"

#include <queue>
#include <map>

Graph::Graph()
{

}

Graph::Graph(const std::vector<QPointF>& vertices) :
    m_vertices(vertices)
{
    m_adjacencyList.resize(vertices.size());
}

int Graph::addVertex(const QPointF& vertex)
{
    m_vertices.push_back(vertex);
    m_adjacencyList.push_back({});
    return m_vertices.size() - 1;
}

void Graph::addEdge(size_t from, size_t to)
{
    assert(from < m_vertices.size() && to < m_vertices.size());
    m_adjacencyList[from].push_back(to);
    m_adjacencyList[to].push_back(from);
}

Graph::Node Graph::nearest(const QPointF& vertex)
{
    assert(m_vertices.size() != 0);

    size_t nearestPointIndex = 0;
    qreal distanceToNearest = euclideanDist(m_vertices[nearestPointIndex], vertex);

    for (size_t currentIndex = 1; currentIndex < m_vertices.size(); ++currentIndex)
    {
        qreal currentDistance = euclideanDist(m_vertices[currentIndex], vertex);
        if (currentDistance < distanceToNearest)
        {
            distanceToNearest = currentDistance;
            nearestPointIndex = currentIndex;
        }
    }

    return { m_vertices[nearestPointIndex], nearestPointIndex };
}

Path Graph::findPath(size_t startPoint, size_t endPoint)
{
    const size_t size = m_vertices.size();
    const size_t root = -1;

    std::vector<bool> used(size);
    std::vector<size_t> dist(size);
    std::vector<size_t> next(size);

    // BFS implementation

    std::queue<size_t> queue;
    queue.push(startPoint);

    used[startPoint] = true;
    next[startPoint] = root;

    while (!queue.empty())
    {
        size_t v = queue.front();
        queue.pop();
        for (size_t i = 0; i < m_adjacencyList[v].size(); ++i)
        {
            size_t to = m_adjacencyList[v][i];
            if (!used[to])
            {
                used[to] = true;
                queue.push(to);
                dist[to] = dist[v] + 1;
                next[to] = v;
            }
        }
    }
    if (!used[endPoint])
    {
        assert(!"No path!");
        return Path();
    }
    else
    {
        Path path;
        for (size_t v = endPoint; v != root; v = next[v])
        {
            path.push_back(m_vertices[v]);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
}

void Graph::setMask(const Polygon& polygon)
{
    std::set<size_t> invalid;
    int valid = 0;
    for (size_t i = 0; i < m_vertices.size(); ++i)
    {
        if (!polygon.inside(m_vertices[i], false))
        {
            invalid.insert(i);
        }
        else
        {
            valid = i;
        }
    }

    for (size_t i = 0; i < m_vertices.size(); ++i)
    {
        if (invalid.find(i) != invalid.end())
        {
            m_vertices[i] = m_vertices[valid];
        }
    }

    for (size_t i = 0; i < m_adjacencyList.size(); ++i)
    {
        if (invalid.find(i) != invalid.end())
        {
            m_adjacencyList[i] = {};
            continue;
        }
        std::vector<size_t> newList;
        for (size_t j = 0; j < m_adjacencyList[i].size(); ++j)
        {
            //QLineF line(m_vertices[i], m_vertices[m_adjacencyList[i][j]]);
            if (invalid.find(m_adjacencyList[i][j]) == invalid.end())
            {
                newList.push_back(m_adjacencyList[i][j]);
            }
        }
        m_adjacencyList[i] = newList;
    }
}

size_t Graph::size()
{
    return m_vertices.size();
}

std::vector<QPointF> Graph::getVertices() const
{
    return m_vertices;
}

std::vector<std::vector<size_t>> Graph::getAdjacencyList() const
{
    return m_adjacencyList;
}

QGraphicsPathItem* Graph::asGraphicsItems()
{
    std::vector<QLineF> lines;
    std::map<std::pair<size_t, size_t>, bool> used;

    for (size_t from = 0; from < m_adjacencyList.size(); ++from)
    {
        for (size_t next = 0; next < m_adjacencyList[from].size(); ++next)
        {
            const size_t to = m_adjacencyList[from][next];
            
            std::pair<size_t, size_t> edge;

            if (from > to)
            {
                edge = { from, to };
            }
            else
            {
                edge = { to, from };
            }

            if (!used[edge])
            {
                lines.push_back(QLineF(m_vertices[from], m_vertices[to]));
                used[edge] = true;
            }
        }
    }
    
    QPainterPath path;

    for (const auto& line : lines)
    {
        QPainterPath pathLine(line.p1());
        pathLine.lineTo(line.p2());

        path.addPath(pathLine);
    }

    auto gpath = new QGraphicsPathItem(path);
    gpath->setPen(QPen(Qt::gray, 1));

    return gpath;
}