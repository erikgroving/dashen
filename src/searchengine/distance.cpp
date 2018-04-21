#include "distance.h"

using SearchEngine::State;
using SearchEngine::Distance;
using namespace SearchEngine::Predicate;

Distance::Distance(State *input): inputState_(input), distanceMatrix_() {
    distanceMatrix_ = Distance::getDistanceMatrix(input);    
}

Distance::~Distance() {
    
}

short int Distance::getDistance(size_t source_x, size_t source_y, size_t target_x, size_t target_y) const {
        if(!SearchEngine::Predicate::inBound(inputState_, source_x, source_y) ||
           !SearchEngine::Predicate::inBound(inputState_, target_x, target_y))
           return -1;

        return distanceMatrix_[source_y][source_x][target_y][target_x];
}

short int Distance::getDistance(Coord source, Coord target) const {
    return Distance::getDistance(source.x, source.y, target.x, target.y);
}

// increasing x moves the target to the right, on the printed level.
std::vector<std::vector<short int> > 
Distance::getDistanceFromPosition(const State *state, size_t x, size_t y) {

    // Initialization
    std::vector<std::vector<short int> > distances;//n, std::vector<short int>(n, -1);
    for (std::vector<bool> wallvector : state->walls) {
        size_t lengthOfRow = wallvector.size();
        distances.push_back(std::vector<short int>(lengthOfRow, -1));
    }

    // Go with BFS starting from x,y through level and update distance value
    std::queue<std::vector<size_t> > queue;
    std::vector<size_t> initialVector = {x, y, 0}; // x, y, value

    // if the starting point is a wall, do not add any position to the queue, i.e.
    // return array filled with -1
    if (!wallAt(state, x, y)) {
        queue.push(initialVector);
    }

    while (!queue.empty()) {
        // Get front element        
        std::vector<size_t> currentElement = queue.front();
        queue.pop();

        //std::cerr << currentElement[0] << " " << currentElement[1] << " "<< currentElement[2] << std::endl;

        // distances are first indixed by row and then column (y,x)
        distances[currentElement[1]][currentElement[0]] = currentElement[2];        

        // Set all neighbour values (distances[][]) that are not -1 and add those to queue
        // ignore walls
        std::vector<std::vector<int> > directions = {{1,0},{0,1},{-1,0},{0,-1}}; 
        for (std::vector<int> curDirection : directions) {
            size_t newX = currentElement[0] + curDirection[0];
            size_t newY = currentElement[1] + curDirection[1];
            //std::cerr << "testing " << newX << " " << newY << std::endl;
            if (inBound(state, newX, newY) ) {
                if (distances[newY][newX] == -1 && 
                    !wallAt(state, newX, newY)) {
                    
                    queue.push(std::vector<size_t> {newX, newY, currentElement[2]+1});
                    //std::cerr << "pushing: " << newX << " " << newY << " "<< currentElement[2]+1 << std::endl;
                }
            }
        }
    }
    return distances;
}


std::vector<std::vector<std::vector<std::vector<short int> > > >
 Distance::getDistanceMatrix(const State *state) {

    // Initialization
    std::vector<std::vector<std::vector<std::vector<short int> > > > result;

    std::cerr << "Vector init" << std::endl;
     for (size_t y=0; y<state->walls.size(); y++) {
        result.push_back(std::vector<std::vector<std::vector<short int> > > (state->walls[y].size()));
        for (size_t x=0; x<state->walls[y].size(); x++) {
            result[y][x] = std::vector<std::vector<short int> > ();
        }
    }
    std::cerr << "End Vector init" << std::endl;

     for (size_t y=0; y<state->walls.size(); y++) {
        //result.push_back(std::vector<std::vector<std::vector<short int> > > ());
        for (size_t x=0; x<state->walls[y].size(); x++) {
            //std::cerr << x << " " << y << std::endl;
            result[y][x] = Distance::getDistanceFromPosition(state, x, y);
        }
    } 

    return result;

}
