#ifndef DISTANCE_H
#define DISTANCE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "typedefs.h"
#include "predicate.h"
#include "state.h"
#include "queue"

namespace SearchEngine {

class Distance {
public:

    /* Constructors */
    Distance(State *input);
    ~Distance();
    //Distance(const State &state);

    std::vector<std::vector<std::vector<std::vector<short int> > > > getDistanceMatrix() const {
        return distanceMatrix_;
    }

    short int getDistance(size_t source_x, size_t source_y, size_t target_x, size_t target_y) const {
        if(!SearchEngine::Predicate::inBound(inputState_, source_x, source_y) ||
           !SearchEngine::Predicate::inBound(inputState_, target_x, target_y))
           return -1;

        return distanceMatrix_[source_y][source_x][target_y][target_x];
    }
    
    static std::vector<std::vector<short int> > 
        computeDistanceFromPosition(const State *state, size_t x, size_t y);
    static std::vector<std::vector<std::vector<std::vector<short int> > > >
        computeDistanceMatrix(const State *state);

private:
    State *inputState_;
    std::vector<std::vector<std::vector<std::vector<short int> > > > distanceMatrix_;

};

}
#endif
