#ifndef GRID_GENERATOR_H
#define GRID_GENERATOR_H

#include <vector>
#include <random>

struct Cell {
    int x, y;
    bool operator==(const Cell& other) const {
        return x == other.x && y == other.y;
    }
};

struct Grid {
    std::vector<std::vector<bool>> obstacles;
    Cell start;
    Cell goal;
    int width, height;
    
    Grid(int w, int h) : width(w), height(h) {
        obstacles.resize(height, std::vector<bool>(width, false));
    }
    
    bool isValidCell(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
    
    bool isObstacle(int x, int y) const {
        return isValidCell(x, y) && obstacles[y][x];
    }
};

Grid generateRandomGrid(int width, int height, double obstacle_ratio) {
    Grid grid(width, height);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    // Generate obstacles
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            grid.obstacles[y][x] = (dis(gen) < obstacle_ratio);
        }
    }
    
    // Generate start and goal positions
    std::uniform_int_distribution<> x_dist(0, width - 1);
    std::uniform_int_distribution<> y_dist(0, height - 1);
    
    do {
        grid.start = {x_dist(gen), y_dist(gen)};
        grid.goal = {x_dist(gen), y_dist(gen)};
    } while (grid.isObstacle(grid.start.x, grid.start.y) || 
             grid.isObstacle(grid.goal.x, grid.goal.y) ||
             (grid.start.x == grid.goal.x && grid.start.y == grid.goal.y));
    
    return grid;
}

#endif