#pragma once

#include <QPointF>
#include <vector>

class QGraphicsPathItem;

namespace Motion
{

class Polygon;
class PolygonSet;

typedef std::vector<QPointF> Path;

inline const Path INVALID_PATH = { {-1, -1} };

struct IFindMethod
{
    virtual Path findPath(
        const QPointF& startPoint, 
        const QPointF& endPoint, 
        const PolygonSet& obstacles
    ) = 0;

    virtual QGraphicsPathItem* getPathMap() = 0;
    virtual void setMask(const Polygon& polygon) = 0;
};

}  // namespace Motion