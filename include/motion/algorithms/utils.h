#pragma once

#include "motion/structures/polygon.h"
#include "motion/algorithms/find_methods/find_method.h"

#include <QPolygonF>
#include <QGraphicsPolygonItem>

#include <optional>

namespace Motion
{

QPolygonF centered(const QPolygonF& polygon, const QPointF& center);
QPolygonF inverted(const QPolygonF& polygon);
QPointF centroid(const QPolygonF& polygon);

QPointF affineLine(const QPointF& source, const QPointF& destination, qreal t);
QPointF affineLine2(const QPointF& source, const QPointF& destination, qreal t);

qreal pathLength(const Path& path);
qreal euclideanDistSqrd(const QPointF& from, const QPointF& to);
qreal euclideanDist(const QPointF& from, const QPointF& to);
std::optional<QPointF> shifted(int index, const std::vector<QPointF>& points, const Polygon& polygon);
QRectF getSceneBBox(double scale = 1.0);
QPolygonF unclose(const QPolygonF& polygon);

}  // namespace Motion