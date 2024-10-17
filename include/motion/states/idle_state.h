#pragma once

#include "motion/states/state.h"
#include "motion/algorithms/find_methods/find_method.h"

#include <QtMath>

#include <memory>

namespace Motion
{

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

} // namespace Motion

