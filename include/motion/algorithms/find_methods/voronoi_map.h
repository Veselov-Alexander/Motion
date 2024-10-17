#pragma once

#include "motion/algorithms/find_methods/preprocessed_graph.h"

namespace Motion
{

class VoronoiMap : public PreprocessedGraph
{
public:
    VoronoiMap(int nWidth, int nHeight, int nPoints = 300);
private:
    void createGraph(const PolygonSet& obstacles) override;
private:
    int m_nPoints;
    int m_nWidth;
    int m_nHeight;
};

}  // namespace Motion