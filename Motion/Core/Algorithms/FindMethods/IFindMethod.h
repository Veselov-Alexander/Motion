#pragma once

#include "QPointF"
#include "QLineF"
#include "QGraphicsPathItem"
#include "Core/Structures/Polygon.h"

#include <vector>


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
};
