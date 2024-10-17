#pragma once

#include <QPoint>

#include "motion/states/idle_state.h"

namespace Motion
{

class DragState : public IdleState
{
public:
    DragState();
    virtual void finish() override;
private:
    QPointF m_startPoint;
};

}  // namespace Motion