#include "IdleState.h"

#include "Core/DisplayView.h"
#include "Core/DisplayView.h"
#include "Core/Algorithms/FindMethods/RRT.h"
#include "Core/Algorithms/Utils.h"

#include "QMessageBox"


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
    if (zoom < 0.6 || zoom > 20)
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

    QPointF source = pDisplayView->getDevicePosition();
    QPointF destination = pDisplayView->mapToScene(pMouseEvent->pos());
    PolygonSet obstacles = pDisplayView->getObstaclesMSums();

    if (obstacles.inside(destination))
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "Invalid destination",
            "Destination point is inside the polygon."
        );
        return;
    }

    IFindMethod* pFindMethod = pDisplayView->getFindMethod();
    assert(pFindMethod);

    auto start = std::chrono::system_clock::now();

    Path path = pFindMethod->findPath(source, destination, obstacles);

    auto end = std::chrono::system_clock::now();
    qreal elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;

    QString pathInfo;

    if (!path.empty())
    {
        qreal length = pathLength(path);
        pathInfo.sprintf("Path length: %4.2f units\nCalculation time: %6.4f ms.", length, elapsed);
    }
    else
    {
        pathInfo.sprintf("Path length: Path not found\nCalculation time: %6.4f ms.", elapsed);
    }

    pDisplayView->setPathInfo(pathInfo, destination);

    if (path.empty())
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "Could not find a path",
            "No path exists or was not found."
        );
        pDisplayView->clearGroup(pDisplayView->m_pPathMapGroup);
        pDisplayView->m_pPathMapGroup->addToGroup(pFindMethod->getPathMap());
        return;
    }

    if (path == INVALID_PATH)
    {
        return;
    }

    QPainterPath qpath;

    for (size_t i = 0; i < path.size() - 1; ++i)
    {
        QPainterPath pathLine(path[i]);
        pathLine.lineTo(path[i + 1]);

        qpath.addPath(pathLine);
    }

    auto gpath = new QGraphicsPathItem(qpath);
    gpath->setPen(QPen(Qt::red, 3));
    
    pDisplayView->moveDevice(path);

    pDisplayView->clearGroup(pDisplayView->m_pPathMapGroup);
    pDisplayView->m_pPathMapGroup->addToGroup(pFindMethod->getPathMap());
    pDisplayView->m_pPathMapGroup->addToGroup(gpath);

    pDisplayView->setDragMode(QGraphicsView::NoDrag);
    pDisplayView->QGraphicsView::mousePressEvent(pMouseEvent);
}
