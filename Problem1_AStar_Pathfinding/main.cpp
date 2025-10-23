#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <random>
#include "grid_generator.h"
#include "astar.h"
#include "heuristics.h"

using namespace std;

struct PerformanceMetrics {
    double time_taken;
    int path_length;
    int nodes_expanded;
    bool path_found;
};

PerformanceMetrics runAStarForHeuristic(Grid& grid, HeuristicFunction heuristic, const string& heuristic_name) {
    auto start = chrono::high_resolution_clock::now();
    
    AStar astar(grid, heuristic);
    bool success = astar.findPath();
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    
    PerformanceMetrics metrics;
    metrics.time_taken = duration.count();
    metrics.path_length = astar.getPathLength();
    metrics.nodes_expanded = astar.getNodesExpanded();
    metrics.path_found = success;
    
    return metrics;
}

int main() {
    const int NUM_RUNS = 50;
    const int GRID_SIZE = 50;
    const double OBSTACLE_RATIO = 0.3;
    
    ofstream data_file("performance_data.csv");
    data_file << "Run,Heuristic,TimeTaken,PathLength,NodesExpanded,PathFound\n";
    
    random_device rd;
    mt19937 gen(rd());
    
    vector<HeuristicFunction> heuristics = {
        manhattanDistance,
        euclideanDistance,
        chebyshevDistance
    };
    
    vector<string> heuristic_names = {
        "Manhattan", "Euclidean", "Chebyshev"
    };
    
    for (int run = 0; run < NUM_RUNS; ++run) {
        cout << "Running test " << run + 1 << "/" << NUM_RUNS << endl;
        
        Grid grid = generateRandomGrid(GRID_SIZE, GRID_SIZE, OBSTACLE_RATIO);
        
        for (size_t i = 0; i < heuristics.size(); ++i) {
            PerformanceMetrics metrics = runAStarForHeuristic(grid, heuristics[i], heuristic_names[i]);
            
            data_file << run + 1 << ","
                     << heuristic_names[i] << ","
                     << metrics.time_taken << ","
                     << metrics.path_length << ","
                     << metrics.nodes_expanded << ","
                     << (metrics.path_found ? "Yes" : "No") << "\n";
        }
    }
    
    data_file.close();
    cout << "Performance data saved to performance_data.csv" << endl;
    
    return 0;
}