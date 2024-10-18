#include "motion/algorithms/utils.h"
#include "motion/display_view.h"

#include <QVector2D>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Vector_2.h>

#include <math.h>


namespace Motion
{

QPolygonF centered(const QPolygonF& polygon, const QPointF& center)
{
    QPolygonF centered;
    QPointF polygonCenter = centroid(polygon);

    for (auto it = polygon.begin(); it != polygon.end(); ++it)
    {
        centered.append(*it + (center - polygonCenter));
    }

    return centered;
}

QPolygonF inverted(const QPolygonF& polygon)
{
    QPolygonF inverted;

    for (auto it = polygon.begin(); it != polygon.end(); ++it)
    {
        inverted.append(-(*it));
    }

    return inverted;
}

QPointF centroid(const QPolygonF& polygon)
{
    qreal x = 0, y = 0;

    for (auto it = polygon.begin(); it != polygon.end(); ++it)
    {
        x += it->x();
        y += it->y();
    }

    return { x / polygon.size(), y / polygon.size() };
}

QPointF affineLine(const QPointF& source, const QPointF& destination, qreal t)
{
    return destination + t * (source - destination);
}

QPointF affineLine2(const QPointF& source, const QPointF& destination, qreal t)
{
    auto v = QVector2D(source - destination);
    return destination + (v.normalized() * t).toPointF();
}

qreal pathLength(const Path& path)
{
    if (path.empty())
        return -1;

    qreal qLength = 0.0;

    for (int i = 0; i < path.size() - 1; ++i)
    {
        qLength += euclideanDist(path[i], path[i + 1]);
    }

    return qLength;
}

qreal euclideanDistSqrd(const QPointF& from, const QPointF& to)
{
    return std::pow(from.x() - to.x(), 2) + std::pow(from.y() - to.y(), 2);
}

qreal euclideanDist(const QPointF& from, const QPointF& to)
{
    return std::sqrt(euclideanDistSqrd(from, to));
}

std::optional<QPointF> shifted(int index, const std::vector<QPointF>& points, const Polygon& polygon)
{
    const QPointF& a = points[index - 1 < 0 ? points.size() - 1 : index - 1];
    const QPointF& b = points[index];
    const QPointF& c = points[index + 1 >= points.size() ? 0 : index + 1];

    QPointF diag = (a - b) + (c - b);

    double angle = std::atan2(-diag.y(), -diag.x());

    double area = a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) + c.x() * (a.y() - b.y());

    if (std::abs(area) <= 1e-1)
    {
        return std::nullopt;
    }

    const double dist = 5;

    QPointF p1(b.x() - dist * std::cos(angle), b.y() - dist * std::sin(angle));
    QPointF p2(b.x() + dist * std::cos(angle), b.y() + dist * std::sin(angle));

    if (!polygon.inside(p1))
    {
        return p1;
    }
    else if (!polygon.inside(p2))
    {
        return p2;
    }
    assert(!"Point not shifted");
    return b;
}

QRectF getSceneBBox(double scale)
{
    qreal w = DisplayView::WIDTH * scale;
    qreal h = DisplayView::HEIGHT * scale;
    return { -w / 2, -h / 2, w, h };
}

QPolygonF unclose(const QPolygonF& polygon)
{
    if (polygon.isClosed())
    {
        QPolygonF p = polygon;
        p.pop_back();
        return p;
    }
    return polygon;
}

}  // namespace Motion