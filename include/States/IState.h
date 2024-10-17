#pragma once

#include "QMouseEvent"


struct IState
{
    virtual ~IState() {};

    virtual void wheelEvent(QWheelEvent* pWheelEvent) = 0;
    virtual void mousePressEvent(QMouseEvent* pMouseEvent) = 0;
    virtual void mouseReleaseEvent(QMouseEvent* pMouseEvent) = 0;

    virtual void finish() = 0;
};