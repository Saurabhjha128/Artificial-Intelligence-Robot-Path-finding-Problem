# A* Pathfinding with Multiple Heuristics

This project implements A* search algorithm for robot pathfinding in grid environments with three different heuristics.

## Features
- Random grid generation with obstacles
- Three heuristics: Manhattan, Euclidean, Chebyshev
- Performance comparison metrics
- Visualization of results

## Compilation
```bash
g++ -std=c++11 main.cpp -o astar_solver
./astar_solver
python performance_plot.py