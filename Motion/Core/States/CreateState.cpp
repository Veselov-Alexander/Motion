#include "CreateState.h"

#include "Core/DisplayView.h"

#include "Core/Algorithms/Utils.h"
#include "Core/Structures/Polygon.h"

#include "QGraphicsEllipseItem"
#include "QMessageBox"


CreateState::CreateState()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    m_pPolygon = new QGraphicsPolygonItem();
    m_pPolygon->setBrush(QBrush(Qt::gray, Qt::BrushStyle::SolidPattern));
    pDisplayView->m_pPolygonPreviewGroup->addToGroup(m_pPolygon);
}

CreateState::~CreateState()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    pDisplayView->clearGroup(pDisplayView->m_pPolygonPreviewGroup);
}

void CreateState::wheelEvent(QWheelEvent* pWheelEvent)
{
    IdleState::wheelEvent(pWheelEvent);
}

void CreateState::mousePressEvent(QMouseEvent* pMouseEvent)
{
    if (pMouseEvent->buttons() & Qt::LeftButton)
    {
        DisplayView* pDisplayView = DisplayView::getInstance();

        QPointF clickPoint = pDisplayView->mapToScene(pMouseEvent->pos());

        QPolygonF polygon = m_pPolygon->polygon();
        polygon.append(clickPoint);

        m_pPolygon->setPolygon(polygon);
        addGraphicsPointItem(clickPoint);
    }

    IdleState::mousePressEvent(pMouseEvent);
}

void CreateState::mouseReleaseEvent(QMouseEvent* pMouseEvent)
{
    IdleState::mouseReleaseEvent(pMouseEvent);
}

void CreateState::finish()
{
    DisplayView* pDisplayView = DisplayView::getInstance();

    QPolygonF polygon = m_pPolygon->polygon();

    if (polygon.size() < 3)
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "Invalid polygon",
            "A polygon has less than 3 points\nThis object will not be added."
        );
        return;
    }

    if (pDisplayView->m_pDevice->intersects(polygon))
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "Invalid polygon",
            "The polygon has an intersection with device.\nThis object will not be added."
        );
        return;
    }

    if (!Polygon(polygon).isSimple())
    {
        QMessageBox::warning(
            pDisplayView->parentWidget(),
            "Invalid polygon", 
            "The polygon has a self-intersection.\nThis object will not be added."
        );
        return;
    }
    else
    {
        pDisplayView->addObstacle(polygon);
    }
}

void CreateState::addGraphicsPointItem(const QPointF& point)
{
    const static qreal size = 4;

    DisplayView* pDisplayView = DisplayView::getInstance();

    QGraphicsEllipseItem* pEllipse = new QGraphicsEllipseItem(point.x() - size / 2,
                                                              point.y() - size / 2,
                                                              size, size);
    pEllipse->setBrush(QBrush(Qt::black, Qt::BrushStyle::SolidPattern));

    pDisplayView->m_pPolygonPreviewGroup->addToGroup(pEllipse);
}