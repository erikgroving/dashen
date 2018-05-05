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

void Distance::computeDistanceFromPositionRecursive(const State *state, std::vector<std::vector<short int>> *result, size_t x, size_t y, short int currentDistance) {
    if(!inBound(state, x, y)) return;
    else if((*result)[y][x] >= 0) return;

    (*result)[y][x] = currentDistance;
    if(inBound(state, x + 1, y))
        computeDistanceFromPositionRecursive(state, result, x + 1, y, currentDistance + 1);
    if(inBound(state, x - 1, y))
        computeDistanceFromPositionRecursive(state, result, x - 1, y, currentDistance + 1);
    if(inBound(state, x, y + 1))
        computeDistanceFromPositionRecursive(state, result, x, y + 1, currentDistance + 1);
    if(inBound(state, x, y - 1))
        computeDistanceFromPositionRecursive(state, result, x, y - 1, currentDistance + 1);
}

// increasing x moves the target to the right, on the printed level.
std::vector<std::vector<short int> > 
Distance::computeDistanceFromPosition(const State *state, size_t x, size_t y) {
    std::vector<std::vector<short int>> result;
    size_t s_height = height(state);

    result.resize(s_height);

    for(size_t i = 0; i < s_height; i++)
        result[i].resize(width(state, i), -1);

    if(!wallAt(state, x, y))
        computeDistanceFromPositionRecursive(state, &result, x, y, 0);
    return result;


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
            result[y][x] = Distance::computeDistanceFromPosition(state, x, y);
        }
    } 

    return result;

}
