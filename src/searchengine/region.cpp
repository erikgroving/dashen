#include "region.h"

using SearchEngine::State;
using SearchEngine::Region;
using namespace SearchEngine::Predicate;

Region::Region(State *input): numRegions_(0), inputState_(input), regionMatrix_() {
    regionMatrix_ = Region::getRegionMatrix(input);    
}

Region::~Region() {
    
}

size_t Region::getRegion(size_t source_x, size_t source_y) const {
    return regionMatrix_[source_y][source_x];
}
size_t Region::getRegion(Coord source) const {
    return Region::getRegion(source.x, source.y);
}

bool Region::isInSameRegion(Coord a, Coord b) const {
    return Region::getRegion(a) == Region::getRegion(b);
}

void Region::computeRegionFromPositionRecursive(const State *state, std::vector<std::vector<size_t>> *result, size_t x, size_t y, size_t regionCounter) {
    // for all neighbours, 
    //  check if not out of bounds, is -1, is not wall, 
    //  then set to same counter 
    //  recursive call
    if(!inBound(state, x, y)) return;
    //std::cerr << (*result)[y][x] << std::endl;

    if((*result)[y][x] != 0) return;
    //std::cerr << "this line2" << std::endl;
    if(wallAt(state, x, y)) return;
    //std::cerr << "this line3" << std::endl;
    /*
    for (const auto& inner : *result) {
        for (auto value : inner) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }*/

    (*result)[y][x] = regionCounter;
    if(inBound(state, x + 1, y))
        computeRegionFromPositionRecursive(state, result, x + 1, y, regionCounter);
    if(inBound(state, x - 1, y))
        computeRegionFromPositionRecursive(state, result, x - 1, y, regionCounter);
    if(inBound(state, x, y + 1))
        computeRegionFromPositionRecursive(state, result, x, y + 1, regionCounter);
    if(inBound(state, x, y - 1))
        computeRegionFromPositionRecursive(state, result, x, y - 1, regionCounter);
}


std::vector<std::vector<size_t> > Region::getRegionMatrix(const State *state) {
    // Initialization
    std::vector<std::vector<size_t> > result;
    size_t s_height = height(state);
    result.resize(s_height);
    // 0 field is a wall or uninitialized
    for(size_t i = 0; i < s_height; i++) {
        result[i].resize(width(state, i), 0);
    }
    size_t regionCounter = 1;

    for (size_t y=0; y<state->walls.size(); y++) {
        for (size_t x=0; x<state->walls[y].size(); x++) {
            if (result[y][x]==0 && !wallAt(state, x, y)) {
                // BFS
                //result[y][x] = regionCounter;
                
                // for all neighbours, 
                //  check if not out of bounds, is -1, is not wall, 
                //  then set to same counter 
                //  recursive call
                computeRegionFromPositionRecursive(state, &result, x, y, regionCounter);

                // counter + 1
                regionCounter++;
            }
        }
    }
    numRegions_ = regionCounter;
    return result;
}
