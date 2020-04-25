#pragma once

#include "Core/States/IState.h"
#include "Core/Algorithms/FindMethods/IFindMethod.h"

#include "QtMath"

#include <memory>


class IdleState : public IState
{
public:
    virtual void wheelEvent(QWheelEvent* pWheelEvent) override;
    virtual void mousePressEvent(QMouseEvent* pWheelEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent* pWheelEvent) override;
    virtual void finish() override { }
private:
    void zoomEvent(QWheelEvent* pWheelEvent);
    void dragEvent(QMouseEvent* pMouseEvent);
    void moveEvent(QMouseEvent* pMouseEvent);
};

