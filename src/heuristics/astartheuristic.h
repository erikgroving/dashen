#ifndef HEURISTICS_ASTAR
#define HEURISTICS_ASTAR

#include "../searchclient/heuristic.h"
#include "../searchclient/state.h"

namespace Heuristic {

class AStarHeuristic: public SearchEngine::Heuristic {

public:
    // WARNING: name() needs to be overriden in deriving classes !!
    virtual unsigned long f(const SearchEngine::State *state) const = 0;
    unsigned long heuristic(const SearchEngine::State *state) const {
        return state->getPathCost() + f(state);
    }
};

}
#endif
