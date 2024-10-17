#include "Algorithms/Dijkstra.h"

#include "Algorithms/Utils.h"

#include <boost/config.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;


Path dijkstraShortestPath(int source, int destination, const Graph& graph)
{
    typedef adjacency_list<listS, vecS, directedS, no_property, property <edge_weight_t, qreal>> graph_t;
    typedef graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    typedef graph_traits<graph_t>::edge_descriptor edge_descriptor;
    typedef std::pair<int, int> Edge;

    std::vector<QPointF> vertices = graph.getVertices();
    std::vector<std::vector<size_t>> adjacencyList = graph.getAdjacencyList();

    int edgeCount = 0;
    for (int i = 0; i < adjacencyList.size(); ++i)
    {
        edgeCount += adjacencyList[i].size();
    }

    Edge* edge_array = new Edge[edgeCount];
    qreal* weights = new qreal[edgeCount];

    int currentIndex = 0;

    for (int i = 0; i < adjacencyList.size(); ++i)
    {
        int from = i;

        for (int j = 0; j < adjacencyList[i].size(); ++j)
        {
            int to = adjacencyList[i][j];

            edge_array[currentIndex] = Edge(from, to);
            weights[currentIndex] = euclideanDist(vertices[from], vertices[to]);

            ++currentIndex;
        }
    }

    graph_t g(edge_array, edge_array + edgeCount, weights, vertices.size());
    //property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

    std::vector<vertex_descriptor> p(num_vertices(g));
    std::vector<int> d(num_vertices(g));
    vertex_descriptor s = vertex(source, g);

    dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));

    delete edge_array;
    delete weights;

    if (d[destination] == INT_MAX)
    {
        return {};
    }

    Path path;
    graph_traits< graph_t >::vertex_descriptor current = destination;

    while (current != source)
    {
        path.push_back(vertices[current]);
        current = p[current];
    }
    path.push_back(vertices[source]);

    std::reverse(path.begin(), path.end());

    return path;
}
