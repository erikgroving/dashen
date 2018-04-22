#include "region.h"

using SearchEngine::State;
using SearchEngine::Region;
using namespace SearchEngine::Predicate;

Region::Region(State *input): inputState_(input), regionMatrix_() {
    regionMatrix_ = Distance::getRegionMatrix(input);    
}

Region::~Region() {
    
}


std::vector<std::vector<short int> > Region::getRegionMatrix(const State *state) {

    // Initialization
    std::vector<std::vector<short int> > result;
    size_t s_height = height(state);
    result.resize(s_height);
    // -1 field is a wall or uninitialized
    for(size_t i = 0; i < s_height; i++) {
        result[i].resize(width(state, i), -1);
    }

    size_t regionCounter = 0;

    for (size_t y=0; y<state->walls.size(); y++) {
        for (size_t x=0; x<state->walls[y].size(); x++) {
            if (result[y][x]==-1 && !wallAt(state, x, y)) {
                // BFS

                // counter + 1
                regionCounter++;
            }
        }
    }

    return result;

}
