#pragma once

#include "motion/states/state.h"

namespace Motion
{

class IdleState : public IState
{
public:
    void wheelEvent(QWheelEvent* pWheelEvent) override;
    void mousePressEvent(QMouseEvent* pWheelEvent) override;
    void mouseReleaseEvent(QMouseEvent* pWheelEvent) override;
    void finish() override { }
private:
    void zoomEvent(QWheelEvent* pWheelEvent);
    void dragEvent(QMouseEvent* pMouseEvent);
    void moveEvent(QMouseEvent* pMouseEvent);
};

} // namespace Motion

