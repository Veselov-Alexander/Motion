#pragma once

#include "motion/structures/polygon.h"

#include <QGraphicsItemGroup>
#include <QGraphicsPolygonItem>

#include <vector>

namespace Motion
{

typedef std::vector<QPolygonF> QPolygonFList;

class ConfigurationSpace
{
public:
    QGraphicsPolygonItem* addObstacle(const QPolygonF& obstacle);
    QPolygonFList getObstacles();
    PolygonSet getObstaclesMSums();
    void draw(QGraphicsItemGroup* pGroup);
    void update();
private:
    QGraphicsPolygonItem* update(const QPolygonF& polygon);
private:
    QPolygonFList m_obstacles;
    PolygonSet m_minkowskiSums;
};

}  // namespace Motion