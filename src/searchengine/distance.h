#ifndef DISTANCE_H
#define DISTANCE_H

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
class Distance {
public:

    /* Constructors */
    Distance() {};
    Distance(State* input);
    ~Distance();
    //Distance(const State &state);

    std::vector<std::vector<std::vector<std::vector<short int> > > > getDistanceMatrix() const {
        return distanceMatrix_;
    }

    short int getDistance(size_t source_x, size_t source_y, size_t target_x, size_t target_y) const;
    short int getDistance(Coord source, Coord target) const;
    
    static std::vector<std::vector<short int> > 
        getDistanceFromPosition(const State *state, size_t x, size_t y);
    static std::vector<std::vector<std::vector<std::vector<short int> > > >
        getDistanceMatrix(const State *state);

private:
    State *inputState_;
    std::vector<std::vector<std::vector<std::vector<short int> > > > distanceMatrix_;

};

}
#endif
