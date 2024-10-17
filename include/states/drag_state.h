#pragma once

#include "QPoint"

#include "states/Idle_state.h"


class DragState : public IdleState
{
public:
    DragState();
    virtual void finish() override;
private:
    QPointF m_startPoint;
};

