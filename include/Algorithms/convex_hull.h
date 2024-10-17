#pragma once

#include <QPolygonF>

QPolygonF convexHull(const std::vector<QPointF>& points);
bool isConvex(const QPolygonF& polygon);