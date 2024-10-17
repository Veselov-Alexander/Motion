#include "states/drag_state.h"

#include "display_view.h"
#include "algorithms/utils.h"

#include <QMessageBox>


DragState::DragState()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->setDeviceMovable(true);

    m_startPoint = pDisplayView->getDevicePosition();
}

void DragState::finish()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    pDisplayView->setDeviceMovable(false);

    QPointF currentPosition = pDisplayView->getDevicePosition();
    PolygonSet obstacles = pDisplayView->getObstaclesMSums();

    if (obstacles.inside(currentPosition))
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "Invalid position",
            "Device is inside the polygon."
        );

        pDisplayView->setDevicePosition(m_startPoint);
    }
    else
    {
        pDisplayView->useSensors(pDisplayView->getVision()->getEnabled());
        pDisplayView->setDevicePosition(currentPosition);
    }
}