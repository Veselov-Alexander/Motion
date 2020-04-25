#include "Utils.h"

#include "CGAL/Boolean_set_operations_2.h"
#include "CGAL/Vector_2.h"

#include "math.h"


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

qreal pathLength(const Path& path)
{
    qreal qLength = 0.0;

    for (int i = 0; i < path.size() - 1; ++i)
    {
        qLength += euclideanDist(path[i], path[i + 1]);
    }

    return qLength;
}

qreal euclideanDist(const QPointF& from, const QPointF& to)
{
    return std::sqrt(std::pow(from.x() - to.x(), 2) + std::pow(from.y() - to.y(), 2));
}