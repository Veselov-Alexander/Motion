#include "states/Reshape_state.h"

#include "algorithms/Utils.h"
#include "display_view.h"

#include "QMessageBox"


void ReshapeState::finish()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    QPolygonF polygon = m_pPolygon->polygon();

    if (polygon.size() < 3)
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "Invalid polygon",
            "A polygon has less than 3 points\nThe device will not be changed."
        );
        return;
    }

    if (Polygon(polygon).isSimple())
    {
        if (!checkPolygonIntersection(polygon, pDisplayView->m_pObstaclesGroup->childItems()))
        { 
            pDisplayView->reshapeDevice(polygon);
        }
        else
        {
            QMessageBox::warning(
                pDisplayView->parentWidget(),
                "Invalid polygon position",
                "The polygon has an intersection with other polygon\nThe device will not be changed"
            );
        }
    }
    else
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "Invalid polygon",
            "The polygon has a self-intersection\nThe device will not be changed"
        );
    }
}

bool ReshapeState::checkPolygonIntersection(const QPolygonF& polygon, const QList<QGraphicsItem*>& polygons)
{
    for (QGraphicsItem* pItem : polygons)
    {
        QGraphicsPolygonItem* pObstacle = dynamic_cast<QGraphicsPolygonItem*>(pItem);
        if (pObstacle)
        {
            if (polygon.intersects(pObstacle->polygon()))
            {
                return true;
            }
        }
    }
    return false;
}
