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

        std::vector<unsigned int> computeAllGoalPriorities(const SearchEngine::State* state);
        bool goalsStillAccessible(const SearchEngine::State& s, std::vector<Goal> remGoals, Goal prospectiveGoal);
        bool canReach(const SearchEngine::State& state, Coord box, Coord goal);
        int getSurroundingWalls(const SearchEngine::State& state, Coord c);

        void computeGoalDependencies(const SearchEngine::State *state);
    }
}
#endif // GOALPRIORITY_H
