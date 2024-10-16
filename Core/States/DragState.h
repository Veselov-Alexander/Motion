#pragma once

#include "QPoint"

#include "Core/States/IdleState.h"


class DragState : public IdleState
{
public:
    DragState();
    virtual void finish() override;
private:
    QPointF m_startPoint;
};

