#pragma once

#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <QPolygonF>
#include <QGraphicsPolygonItem>

#include "algorithms/find_methods/find_method.h"
#include "vision.h"

#include <stack>


class DeviceGraphicsItem : public QGraphicsPolygonItem
{
public:
    DeviceGraphicsItem(const QPolygonF& polygon, Vision* pVision);

    void move(const Path& path);
    void reshape(const QPolygonF& polygon);
    bool intersects(const QPolygonF& polygon);
    void setDevicePosition(const QPointF& position);
    void setMovable(bool bEnabled);
    void stopAnimation();
    void undoMove();
    void clearMovesHistory();
private:
    QTimeLine* m_animationTimer;
    QGraphicsItemAnimation* m_animation;
    std::stack<QPointF> m_positions;
    Vision* m_pVision = nullptr;
};

