#pragma once

#include "motion/algorithms/find_methods/preprocessed_graph.h"

namespace Motion
{

class VisibilityGraph : public PreprocessedGraph
{
private:
    void createGraph(const PolygonSet& obstacles) override;
    void addPoints(const std::vector<QPointF> points, const Polygon& polygon);
};

}  // namespace Motion