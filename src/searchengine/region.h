#ifndef REGION_H
#define REGION_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "typedefs.h"
#include "predicate.h"
#include "state.h"

namespace SearchEngine {
class State;
class Region {
public:

    /* Constructors */
    Region() {numRegions_ = 0;};
    Region(State* input);
    ~Region();

    size_t getRegion(size_t source_x, size_t source_y) const;
    size_t getRegion(Coord source) const;
    
    bool isInSameRegion(Coord a, Coord b) const;

    std::vector<std::vector<size_t> > getRegionMatrix() const {
        return regionMatrix_;
    }
    std::vector<std::vector<size_t> > getRegionMatrix(const State *state);

    void computeRegionFromPositionRecursive(
        const State *state, std::vector<std::vector<size_t>> *result, 
        size_t x, size_t y, size_t regionCounter);


    size_t numRegions_;
private:
    State *inputState_;
    std::vector<std::vector<size_t> > regionMatrix_;

};

}
#endif
