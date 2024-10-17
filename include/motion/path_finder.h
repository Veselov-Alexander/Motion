#pragma once

#include "motion/algorithms/find_methods/find_method.h"

namespace Motion
{

struct IPathFinder
{
    virtual void findPath(QPoint physicCoord) = 0;
    virtual void subPathReached() {};
protected:
    void displayPath(const Path& path);
};

class RegularPathFinder : public IPathFinder
{
public:
    void findPath(QPoint physicCoord) override;
};

class VisionPathFinder : public IPathFinder
{
public:
    void findPath(QPoint physicCoord) override;
    void subPathReached() override;
private:
    bool visited(const QPointF& v);
    bool m_pathFound = false;
    QPointF m_source;
    QPointF m_destination;
    PolygonSet m_obstacles;
    Path m_path;

    std::vector<QPointF> m_visited = {};
};

}  // namespace Motion