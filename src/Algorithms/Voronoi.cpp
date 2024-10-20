#include "motion/algorithms/voronoi.h"

#include <boost/polygon/voronoi.hpp>
#include "motion/algorithms/utils.h"
#include "motion/display_view.h"
#include <unordered_set>

using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;

namespace
{

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

}  // namespace

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

namespace Motion
{

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

void pushBounds(std::vector<Segment>& segments)
{
    int minx = segments[0].p0.a;
    int miny = segments[0].p0.b;
    int maxx = segments[0].p1.a;
    int maxy = segments[0].p1.b;
    for (const auto& s : segments)
    {
        minx = std::min(minx, s.p0.a);
        miny = std::min(miny, s.p0.b);
        maxx = std::max(maxx, s.p0.a);
        maxy = std::max(maxy, s.p0.b);
        minx = std::min(minx, s.p1.a);
        miny = std::min(miny, s.p1.b);
        maxx = std::max(maxx, s.p1.a);
        maxy = std::max(maxy, s.p1.b);
    }

    minx -= 100;
    miny -= 100;
    maxx += 100;
    maxy += 100;

    segments.push_back(Segment(minx, miny, maxx, miny));
    segments.push_back(Segment(maxx, miny, maxx, maxy));
    segments.push_back(Segment(maxx, maxy, minx, maxy));
    segments.push_back(Segment(minx, maxy, minx, miny));

    auto d = 50;
    DisplayView* pDisplayView = DisplayView::getInstance();
    auto p = QPolygonF() << QPointF(minx - d, miny) << QPointF(maxx + d, miny)
                         << QPointF(maxx + d, miny - d) << QPointF(minx - d, miny - d);
    pDisplayView->addObstacle(p);

    p = QPolygonF() << QPointF(maxx, miny) << QPointF(maxx, maxy)
                    << QPointF(maxx + d, maxy) << QPointF(maxx + d, miny);
    pDisplayView->addObstacle(p);

    p = QPolygonF() << QPointF(maxx + d, maxy) << QPointF(minx - d, maxy)
                    << QPointF(minx - d, maxy + d) << QPointF(maxx + d, maxy + d);

    pDisplayView->addObstacle(p);

    p = QPolygonF() << QPointF(minx, maxy) << QPointF(minx, miny)
                    << QPointF(minx - d, miny) << QPointF(minx - d, maxy);

    pDisplayView->addObstacle(p);
}

std::vector<QPolygonF> voronoiDiagram_1(const std::vector<QPointF>& sites)
{
    voronoi_diagram<double> vd;

    std::vector<Point> points;
    std::vector<Segment> segments;

    for (const QPointF& site : sites)
    {
        points.push_back(Point(site.x(), site.y()));
    }

    pushBounds(segments);

    construct_voronoi(points.begin(), points.end(), segments.begin(), segments.end(), &vd);

    return voronoiDiagramToPolygons(vd);
}

std::vector<QPointF> voronoiDiagram_2(const std::vector<QPointF>& sites)
{
    voronoi_diagram<double> vd;

    std::vector<Point> points;
    std::vector<Segment> segments;

    for (const QPointF& site : sites)
    {
        points.push_back(Point(site.x(), site.y()));
    }

    pushBounds(segments);

    construct_voronoi(points.begin(), points.end(), segments.begin(), segments.end(), &vd);

    return voronoiDiagramToPoints(vd);
}

std::vector<QLineF> voronoiDiagram_3(const std::vector<QLineF>& lines)
{
    voronoi_diagram<double> vd;

    std::vector<Point> points;
    std::vector<Segment> segments;

    for (const QLineF& line : lines)
    {
        segments.push_back(Segment(line.p1().x(), line.p1().y(), line.p2().x(), line.p2().y()));
    }

    pushBounds(segments);

    construct_voronoi(points.begin(), points.end(), segments.begin(), segments.end(), &vd);

    return voronoiDiagramToLines(vd);
}

}  // namespace Motion