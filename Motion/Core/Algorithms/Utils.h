#pragma once

#include "QPolygonF"
#include "Core/Algorithms/FindMethods/IFindMethod.h"
#include "Core/Structures/Polygon.h"
#include "QGraphicsPolygonItem"


QPolygonF centered(const QPolygonF& polygon, const QPointF& center);
QPolygonF inverted(const QPolygonF& polygon);
QPointF centroid(const QPolygonF& polygon);

QPointF affineLine(const QPointF& source, const QPointF& destination, qreal t);

qreal pathLength(const Path& path);
qreal euclideanDist(const QPointF& from, const QPointF& to);