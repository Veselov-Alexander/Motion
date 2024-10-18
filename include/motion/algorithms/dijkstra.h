#pragma once

#include "motion/structures/graph.h"

namespace Motion
{

Path dijkstraShortestPath(int source, int destination, const Graph& graph);

}  // namespace Motion