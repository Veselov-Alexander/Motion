#include "motion/algorithms/convex_hull.h"
#include "motion/display_view.h"

#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <QRectF>

namespace Motion
{

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point_2;
typedef CGAL::Convex_hull_traits_adapter_2<Kernel, CGAL::Pointer_property_map<Point_2>::type > Convex_hull_traits_2;

QPolygonF convexHull(const std::vector<QPointF>& pts)
{
    std::vector<Point_2> points;

    for (int i = 0; i < pts.size(); ++i)
    {
        const QPointF point = pts[i];
        points.push_back(Point_2(point.x(), point.y()));
    }
    std::vector<std::size_t> indices(points.size()), out;
    std::iota(indices.begin(), indices.end(), 0);

    CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
        Convex_hull_traits_2(CGAL::make_property_map(points)));
    QPolygonF output;
    for (std::size_t i : out)
    {
        output.append(QPointF(CGAL::to_double(points[i].x()), CGAL::to_double(points[i].y())));
    }
    return output;
}

bool isConvex(const QPolygonF& polygon)
{
    std::vector<Point_2> points;

    for (const auto& p : polygon)
    {
        points.push_back(Point_2(p.x(), p.y()));
    }

    return CGAL::is_convex_2(points.begin(), points.end());
}

}  // namespace Motion