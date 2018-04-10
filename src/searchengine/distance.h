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
    //Distance();
    ~Distance();

    Distance(const State &state);

    std::vector<std::vector<short int> > 
        getDistanceFromPosition(const State &state, size_t x, size_t y);
    std::vector<std::vector<std::vector<std::vector<short int> > > > 
        getDistanceMatrix(const State &state);

private:

    State &state;
    std::vector<std::vector<std::vector<std::vector<short int> > > > distanceMatrix;

};

}
#endif
