#ifndef REGION_H
#define REGION_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "typedefs.h"
#include "predicate.h"
#include "state.h"
#include <queue>

namespace SearchEngine {
class State;
class Region {
public:

    /* Constructors */
    //Region() {};
    Region(State* input);
    ~Region();

    short int getRegion(size_t source_x, size_t source_y) const;
    short int getRegion(Coord source) const;
    
    bool isInSameRegion(Coord a, Coord b) const;

    std::vector<std::vector<short int> > getRegionMatrix() const {
        return regionMatrix_;
    }
    static std::vector<std::vector<short int> > getRegionMatrix(const State *state);

private:
    State *inputState_;
    std::vector<std::vector<short int> > regionMatrix_;

};

}
#endif
