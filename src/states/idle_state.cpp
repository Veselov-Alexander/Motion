#include "motion/states/Idle_state.h"

#include "motion/display_view.h"
#include "motion/display_view.h"
#include "motion/algorithms/find_methods/RRT.h"

namespace Motion
{

void IdleState::wheelEvent(QWheelEvent* pWheelEvent)
{
    zoomEvent(pWheelEvent);
}

void IdleState::mousePressEvent(QMouseEvent* pMouseEvent)
{
    switch (pMouseEvent->button())
    {
    case Qt::MiddleButton:
        dragEvent(pMouseEvent);
        break;
    case Qt::RightButton:
        moveEvent(pMouseEvent);
        break;
    default:
        DisplayView::getInstance()->QGraphicsView::mousePressEvent(pMouseEvent);
    }
}

void IdleState::mouseReleaseEvent(QMouseEvent* pMouseEvent)
{
    DisplayView* pDisplayView = DisplayView::getInstance();

    if (pMouseEvent->button() == Qt::MiddleButton)
    {
        QMouseEvent fakeEvent(pMouseEvent->type(), pMouseEvent->localPos(),
            pMouseEvent->screenPos(), pMouseEvent->windowPos(),
            Qt::LeftButton, pMouseEvent->buttons() &
            ~Qt::LeftButton, pMouseEvent->modifiers());
        pDisplayView->QGraphicsView::mouseReleaseEvent(&fakeEvent);

        pDisplayView->setDragMode(QGraphicsView::NoDrag);
    }
    else
    {
        pDisplayView->QGraphicsView::mouseReleaseEvent(pMouseEvent);
    }
}

void IdleState::zoomEvent(QWheelEvent* pWheelEvent)
{
    DisplayView* pDisplayView = DisplayView::getInstance();

    double angle = pWheelEvent->angleDelta().y();
    double factor = qPow(1.0015, angle);

    qreal zoom = pDisplayView->transform().scale(factor, factor).mapRect(QRectF(0, 0, 1, 1)).width();

    // magic...
    if (zoom < 0.4 || zoom > 20)
        return;

    auto targetViewportPos = pWheelEvent->pos();
    auto targetScenePos = pDisplayView->mapToScene(pWheelEvent->pos());

    pDisplayView->scale(factor, factor);
    pDisplayView->centerOn(targetScenePos);
    QPointF halfViewport = QPointF(pDisplayView->viewport()->width() / 2.0,
                                   pDisplayView->viewport()->height() / 2.0);
    QPointF deltaViewportPos = targetViewportPos - halfViewport;
    QPointF viewportCenter = pDisplayView->mapFromScene(targetScenePos) - deltaViewportPos;
    pDisplayView->centerOn(pDisplayView->mapToScene(viewportCenter.toPoint()));
}

void IdleState::dragEvent(QMouseEvent* pMouseEvent)
{
    DisplayView* pDisplayView = DisplayView::getInstance();

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, pMouseEvent->localPos(),
        pMouseEvent->screenPos(), pMouseEvent->windowPos(),
        Qt::LeftButton, 0, pMouseEvent->modifiers());
    pDisplayView->QGraphicsView::mouseReleaseEvent(&releaseEvent);

    pDisplayView->setDragMode(QGraphicsView::ScrollHandDrag);

    QMouseEvent fakeEvent(pMouseEvent->type(), pMouseEvent->localPos(),
        pMouseEvent->screenPos(), pMouseEvent->windowPos(),
        Qt::LeftButton, pMouseEvent->buttons() | Qt::LeftButton,
        pMouseEvent->modifiers());

    pDisplayView->QGraphicsView::mousePressEvent(&fakeEvent);
}

void IdleState::moveEvent(QMouseEvent* pMouseEvent)
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    IPathFinder* pPathFinder = pDisplayView->getPathFinder();
    
    pPathFinder->findPath(pMouseEvent->pos());

    pDisplayView->setDragMode(QGraphicsView::NoDrag);
    pDisplayView->QGraphicsView::mousePressEvent(pMouseEvent);
}

}  // namespace Motion