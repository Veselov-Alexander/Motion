#pragma once

#include "Algorithms/FindMethods/PreprocessedGraph.h"

class VisibilityGraph : public PreprocessedGraph
{
private:
    void createGraph(const PolygonSet& obstacles) override;
    void addPoints(const std::vector<QPointF> points, const Polygon& polygon);
};