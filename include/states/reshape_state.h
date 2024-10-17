#pragma once

#include "states/create_state.h"

#include <QPolygonF>
#include <QGraphicsItem>

class ReshapeState : public CreateState
{
public:
    virtual void finish() override;
private:
    bool checkPolygonIntersection(const QPolygonF& polygon, const QList<QGraphicsItem*>& polygons);
};

