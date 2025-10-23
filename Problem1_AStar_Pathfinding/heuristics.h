#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "grid_generator.h"

typedef double (*HeuristicFunction)(const Cell&, const Cell&);

double manhattanDistance(const Cell& a, const Cell& b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

double euclideanDistance(const Cell& a, const Cell& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

double chebyshevDistance(const Cell& a, const Cell& b) {
    return std::max(abs(a.x - b.x), abs(a.y - b.y));
}

#endif