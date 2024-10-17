#pragma once

#include "Algorithms/FindMethods/PreprocessedGraph.h"

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