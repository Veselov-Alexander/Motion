#pragma once

#include "QPolygonF"
#include "QPointF"
#include "QLineF"

#include <vector>


std::vector<QPolygonF> voronoiDiagram_1(const std::vector<QPointF>& sites, const int diagramSize = 1500);
std::vector<QPointF> voronoiDiagram_2(const std::vector<QPointF>& sites, const int diagramSize = 1500);
std::vector<QLineF> voronoiDiagram_3(const std::vector<QLineF>& lines, const int diagramSize = 1500);

