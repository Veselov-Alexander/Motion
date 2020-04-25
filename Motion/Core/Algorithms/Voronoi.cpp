#include "Voronoi.h"

#include "boost/polygon/voronoi.hpp"

#include <unordered_set>

using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;

struct Point 
{
    Point(int x, int y) :
        a(x), b(y)
    {}

    int a, b;
};

struct Segment 
{
    Segment(int x1, int y1, int x2, int y2) :
        p0(x1, y1), p1(x2, y2) 
    {}

    Point p0, p1;
};

namespace boost 
{
    namespace polygon 
    {

        template <>
        struct geometry_concept<Point> 
        {
            typedef point_concept type;
        };

        template <>
        struct point_traits<Point> 
        {
            typedef int coordinate_type;

            static inline coordinate_type get(const Point& point, orientation_2d orient) 
            {
                return (orient == HORIZONTAL) ? point.a : point.b;
            }
        };

        template <>
        struct geometry_concept<Segment> 
        {
            typedef segment_concept type;
        };

        template <>
        struct segment_traits<Segment> 
        {
            typedef int coordinate_type;
            typedef Point point_type;

            static inline point_type get(const Segment& segment, direction_1d dir) 
            {
                return dir.to_int() ? segment.p1 : segment.p0;
            }
        };
    }  // polygon
}  // boost

namespace std 
{
    template <> struct hash<std::pair<int, int>> 
    {
        inline size_t operator()(const std::pair<int, int>& v) const 
        {
            std::hash<int> int_hasher;
            return int_hasher(v.first) ^ int_hasher(v.second);
        }
    };

}

std::vector<QPolygonF> voronoiDiagramToPolygons(const voronoi_diagram<double>& vd)
{
    std::vector<QPolygonF> result;

    for (const auto& cell : vd.cells())
    {
        auto edge = cell.incident_edge();
        if (!edge) continue;

        QPolygonF polygon;

        do
        {
            auto v0 = edge->vertex0();
            auto v1 = edge->vertex1();
            if (v0 && v1)
                polygon.append({ {v0->x(), v0->y()}, {v1->x(), v1->y()} });

            edge = edge->next();
        } while (edge != cell.incident_edge());

        result.push_back(polygon);
    }

    return result;
}

std::vector<QPointF> voronoiDiagramToPoints(const voronoi_diagram<double>& vd)
{
    std::unordered_set<std::pair<int, int>> points;

    for (const auto& cell : vd.cells())
    {
        auto edge = cell.incident_edge();
        if (!edge) continue;

        do
        {
            auto v0 = edge->vertex0();
            auto v1 = edge->vertex1();
            if (v0 && v1)
            {
                points.insert({(int)v0->x(), (int)v0->y()});
                points.insert({ (int)v1->x(), (int)v1->y()});
            }

            edge = edge->next();
        } while (edge != cell.incident_edge());
    }

    std::vector<QPointF> result;
    result.reserve(points.size());

    for (const auto& pair : points)
    {
        result.push_back({ (double)pair.first, (double)pair.second});
    }

    return result;
}

std::vector<QLineF> voronoiDiagramToLines(const voronoi_diagram<double>& vd)
{
    std::vector<QLineF> result;

    for (const auto& cell : vd.cells())
    {
        auto edge = cell.incident_edge();
        if (!edge) continue;

        do
        {
            auto v0 = edge->vertex0();
            auto v1 = edge->vertex1();
            if (v0 && v1)
            {
                result.push_back({ {v0->x(), v0->y()}, {v1->x(), v1->y()} });
            }

            edge = edge->next();
        } while (edge != cell.incident_edge());
    }

    return result;
}

std::vector<QPolygonF> voronoiDiagram_1(const std::vector<QPointF>& sites, const int diagramSize)
{
    voronoi_diagram<double> vd;

    std::vector<Point> points;
    std::vector<Segment> segments;

    for (const QPointF& site : sites)
    {
        points.push_back(Point(site.x(), site.y()));
    }

    segments.push_back(Segment(diagramSize, diagramSize, diagramSize, -diagramSize));
    segments.push_back(Segment(diagramSize, -diagramSize, -diagramSize, -diagramSize));
    segments.push_back(Segment(-diagramSize, -diagramSize, -diagramSize, diagramSize));
    segments.push_back(Segment(-diagramSize, diagramSize, diagramSize, diagramSize));

    construct_voronoi(points.begin(), points.end(), segments.begin(), segments.end(), &vd);

    return voronoiDiagramToPolygons(vd);
}

std::vector<QPointF> voronoiDiagram_2(const std::vector<QPointF>& sites, const int diagramSize)
{
    voronoi_diagram<double> vd;

    std::vector<Point> points;
    std::vector<Segment> segments;

    for (const QPointF& site : sites)
    {
        points.push_back(Point(site.x(), site.y()));
    }

    segments.push_back(Segment(diagramSize, diagramSize, diagramSize, -diagramSize));
    segments.push_back(Segment(diagramSize, -diagramSize, -diagramSize, -diagramSize));
    segments.push_back(Segment(-diagramSize, -diagramSize, -diagramSize, diagramSize));
    segments.push_back(Segment(-diagramSize, diagramSize, diagramSize, diagramSize));

    construct_voronoi(points.begin(), points.end(), segments.begin(), segments.end(), &vd);

    return voronoiDiagramToPoints(vd);
}

std::vector<QLineF> voronoiDiagram_3(const std::vector<QLineF>& lines, const int diagramSize)
{
    voronoi_diagram<double> vd;

    std::vector<Point> points;
    std::vector<Segment> segments;

    for (const QLineF& line : lines)
    {
        segments.push_back(Segment(line.p1().x(), line.p1().y(), line.p2().x(), line.p2().y()));
    }

    segments.push_back(Segment(diagramSize, diagramSize, diagramSize, -diagramSize));
    segments.push_back(Segment(diagramSize, -diagramSize, -diagramSize, -diagramSize));
    segments.push_back(Segment(-diagramSize, -diagramSize, -diagramSize, diagramSize));
    segments.push_back(Segment(-diagramSize, diagramSize, diagramSize, diagramSize));

    construct_voronoi(points.begin(), points.end(), segments.begin(), segments.end(), &vd);

    return voronoiDiagramToLines(vd);
}
