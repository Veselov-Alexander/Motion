#pragma once

#include "motion/states/create_state.h"

#include <QPolygonF>
#include <QGraphicsItem>

namespace Motion
{

class ReshapeState : public CreateState
{
public:
    virtual void finish() override;
private:
    bool checkPolygonIntersection(const QPolygonF& polygon, const QList<QGraphicsItem*>& polygons);
};

} // namespace Motion

