#pragma once

#include "algorithms/find_methods/preprocessed_graph.h"

class VisibilityGraph : public PreprocessedGraph
{
private:
    void createGraph(const PolygonSet& obstacles) override;
    void addPoints(const std::vector<QPointF> points, const Polygon& polygon);
};