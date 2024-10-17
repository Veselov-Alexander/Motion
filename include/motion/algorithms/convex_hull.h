#pragma once

#include <QPolygonF>

namespace Motion
{

QPolygonF convexHull(const std::vector<QPointF>& points);
bool isConvex(const QPolygonF& polygon);

}  // namespace Motion