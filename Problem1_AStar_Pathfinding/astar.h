#ifndef ASTAR_H
#define ASTAR_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include "grid_generator.h"
#include "heuristics.h"

struct Node {
    Cell cell;
    double g_cost;  // Cost from start to this node
    double h_cost;  // Heuristic cost to goal
    double f_cost() const { return g_cost + h_cost; }
    Node* parent;
    
    Node(Cell c, double g, double h, Node* p = nullptr) 
        : cell(c), g_cost(g), h_cost(h), parent(p) {}
};

struct NodeCompare {
    bool operator()(const Node* a, const Node* b) const {
        return a->f_cost() > b->f_cost();
    }
};

class AStar {
private:
    Grid grid;
    HeuristicFunction heuristic;
    std::vector<Node*> open_list;
    std::unordered_map<int, Node*> all_nodes;
    std::vector<Cell> path;
    int nodes_expanded;
    
    int cellToKey(const Cell& cell) const {
        return cell.y * grid.width + cell.x;
    }
    
    std::vector<Cell> getNeighbors(const Cell& cell) const {
        std::vector<Cell> neighbors;
        int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
        int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
        
        for (int i = 0; i < 8; ++i) {
            int new_x = cell.x + dx[i];
            int new_y = cell.y + dy[i];
            
            if (grid.isValidCell(new_x, new_y) && !grid.isObstacle(new_x, new_y)) {
                neighbors.push_back({new_x, new_y});
            }
        }
        
        return neighbors;
    }
    
    double getMoveCost(const Cell& from, const Cell& to) const {
        // Diagonal moves cost more
        if (from.x != to.x && from.y != to.y) {
            return 1.414; // sqrt(2)
        }
        return 1.0;
    }
    
    void reconstructPath(Node* node) {
        path.clear();
        while (node != nullptr) {
            path.push_back(node->cell);
            node = node->parent;
        }
        std::reverse(path.begin(), path.end());
    }
    
    void cleanup() {
        for (auto& pair : all_nodes) {
            delete pair.second;
        }
        all_nodes.clear();
        open_list.clear();
        path.clear();
    }

public:
    AStar(const Grid& g, HeuristicFunction h) : grid(g), heuristic(h), nodes_expanded(0) {}
    
    ~AStar() {
        cleanup();
    }
    
    bool findPath() {
        cleanup();
        nodes_expanded = 0;
        
        Node* start_node = new Node(grid.start, 0, heuristic(grid.start, grid.goal));
        all_nodes[cellToKey(grid.start)] = start_node;
        open_list.push_back(start_node);
        std::push_heap(open_list.begin(), open_list.end(), NodeCompare());
        
        while (!open_list.empty()) {
            std::pop_heap(open_list.begin(), open_list.end(), NodeCompare());
            Node* current = open_list.back();
            open_list.pop_back();
            
            nodes_expanded++;
            
            if (current->cell.x == grid.goal.x && current->cell.y == grid.goal.y) {
                reconstructPath(current);
                return true;
            }
            
            for (const Cell& neighbor_cell : getNeighbors(current->cell)) {
                double new_g_cost = current->g_cost + getMoveCost(current->cell, neighbor_cell);
                int neighbor_key = cellToKey(neighbor_cell);
                
                if (all_nodes.find(neighbor_key) == all_nodes.end()) {
                    Node* neighbor_node = new Node(neighbor_cell, new_g_cost, 
                                                 heuristic(neighbor_cell, grid.goal), current);
                    all_nodes[neighbor_key] = neighbor_node;
                    open_list.push_back(neighbor_node);
                    std::push_heap(open_list.begin(), open_list.end(), NodeCompare());
                } else {
                    Node* existing_node = all_nodes[neighbor_key];
                    if (new_g_cost < existing_node->g_cost) {
                        existing_node->g_cost = new_g_cost;
                        existing_node->parent = current;
                        // Re-heapify
                        std::make_heap(open_list.begin(), open_list.end(), NodeCompare());
                    }
                }
            }
        }
        
        return false; // No path found
    }
    
    int getPathLength() const {
        return path.size();
    }
    
    int getNodesExpanded() const {
        return nodes_expanded;
    }
    
    const std::vector<Cell>& getPath() const {
        return path;
    }
};

#endif