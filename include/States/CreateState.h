#pragma once

#include "States/IdleState.h"

#include "QGraphicsPolygonItem"


class CreateState : public IdleState
{
public:
    CreateState();
    ~CreateState();

    virtual void wheelEvent(QWheelEvent* pWheelEvent) override;
    virtual void mousePressEvent(QMouseEvent* pMouseEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent* pMouseEvent) override;

    virtual void finish() override;

private:
    void addGraphicsPointItem(const QPointF& point);

protected:
    QGraphicsPolygonItem* m_pPolygon;
};

