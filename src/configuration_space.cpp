#include "configuration_space.h"

#include "QBrush"
#include "QPen"
#include "QGraphicsPolygonItem"

#include "algorithms/Utils.h"
#include "display_view.h"


QPolygonFList ConfigurationSpace::getObstacles()
{
    return m_obstacles;
}

PolygonSet ConfigurationSpace::getObstaclesMSums()
{
    return m_minkowskiSums;
}

QGraphicsPolygonItem* ConfigurationSpace::addObstacle(const QPolygonF& obstacle)
{
    m_obstacles.push_back(obstacle);

    return update(obstacle);
}

QGraphicsPolygonItem* ConfigurationSpace::update(const QPolygonF& polygon)
{
    DisplayView *pDisplayView = DisplayView::getInstance();
    assert(pDisplayView);

    Polygon P(polygon);
    Polygon Q(inverted(pDisplayView->getDevicePolygon()));

    Polygon minkowskiSumPolygon = P.minkowskiSum(Q);

    m_minkowskiSums.insert(minkowskiSumPolygon);

    QGraphicsPolygonItem* pItem = new QGraphicsPolygonItem(polygon);
    pItem->setBrush(QBrush(QColor(30, 30, 30, 255), Qt::BrushStyle::SolidPattern));
    pItem->setPen(QPen(Qt::transparent));

    return pItem;
}

void ConfigurationSpace::update()
{
    m_minkowskiSums.clear();

    for (const auto& obstacle : m_obstacles)
    {
        update(obstacle);
    }
}

void ConfigurationSpace::draw(QGraphicsItemGroup* pGroup)
{
    for (const Polygon& polygon : m_minkowskiSums)
    {
        QPainterPath path = polygon.toPath();
        auto pathitem = new QGraphicsPathItem(path);
        pathitem->setPen(QPen(Qt::black, 3));

        pGroup->addToGroup(pathitem);
    }
}