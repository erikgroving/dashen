#ifndef SEARCHENGINE_GOALPRIORITY_H
#define SEARCHENGINE_GOALPRIORITY_H

#include <climits>
#include <algorithm>
#include <vector>

#include "typedefs.h"
#include "state.h"
#include "predicate.h"

namespace SearchEngine {

    namespace GoalPriorityComputation {
        unsigned int computeGoalPriority(const SearchEngine::State *state, const Goal &goal);
        unsigned int computeTilePriority(const SearchEngine::State *state, size_t x, size_t y, std::vector<Coord> &visitedTiles);
    }
}
#endif // GOALPRIORITY_H
