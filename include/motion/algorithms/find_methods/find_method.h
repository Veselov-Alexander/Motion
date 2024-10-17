#pragma once

#include <QPointF>
#include <QLineF>
#include <QGraphicsPathItem>
#include "motion/structures/polygon.h"

#include <vector>

namespace Motion
{

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

    void setMask(const Polygon& polygon) { m_mask = polygon; };
protected:
    std::optional<Polygon> m_mask = std::nullopt;
};

}  // namespace Motion