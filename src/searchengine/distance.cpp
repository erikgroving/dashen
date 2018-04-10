#include "distance.h"

using SearchEngine::State;
using SearchEngine::Distance;

Distance::Distance() {
    
}
Distance::~Distance() {
    
}

std::vector<std::vector<short int> > 
Distance::getDistanceFromPosition(const State &state, size_t x, size_t y) {

    // Initialization
    std::vector<std::vector<short int> > distances;//n, std::vector<short int>(n, -1);
    for (std::vector<bool> wallvector : state.walls) {
        size_t lengthOfRow = wallvector.size();
        distances.push_back(std::vector<short int>(lengthOfRow, -1));
    }

    // Set starting position to zero
    //distances[x][y] = 0;

    // Go with BFS starting from x,y through level and update distance value
    std::queue<std::vector<size_t> > queue;
    std::vector<size_t> initialVector = {x, y, 0}; // x, y, value
    queue.push(initialVector);

    while (!queue.empty()) {
        // Get front element        
        std::vector<size_t> currentElement = queue.front();
        queue.pop();

        distances[currentElement[0]][currentElement[1]] = currentElement[2];        

        // Set all neighbour values (distances[][]) that are not -1 and add those to queue
        // ignore walls
        std::vector<std::vector<int> > directions = {{1,0},{0,1},{-1,0},{0,-1}}; 
        for (std::vector<int> curDirection : directions) {
            size_t newX = currentElement[0] + curDirection[0];
            size_t newY = currentElement[1] + curDirection[1];
            if (newY <= state.walls.size() &&
                newX <= state.walls[newY].size() ) {
                if (distances[newX][newY] == -1 && 
                    !SearchEngine::Predicate::wallAt(&state, newX, newY)) {
                    
                    queue.push(std::vector<size_t> {newX, newY, currentElement[2]+1});
                }
            }
        }
    }
    return distances;
}
