#pragma once

#include "motion/structures/Graph.h"

namespace Motion
{

Path dijkstraShortestPath(int source, int destination, const Graph& graph);

}  // namespace Motion