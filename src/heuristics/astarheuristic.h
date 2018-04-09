#ifndef HEURISTICS_ASTAR
#define HEURISTICS_ASTAR

#include "../searchengine/searchengine"

namespace Heuristic {

class AStarHeuristic: public SearchEngine::Heuristic {

public:
    // WARNING: name() needs to be overriden in deriving classes !!
    virtual unsigned long f(const SearchEngine::State *state) const = 0;
    virtual unsigned long heuristic(const SearchEngine::State *state) const {
        return state->getPathCost() + f(state);
    }
};

class BasicAStar : public AStarHeuristic {

public:
    virtual std::string name() const { return "BasicAStar"; }
    virtual unsigned long f(const SearchEngine::State *state) const {
        unsigned long result = 0;
        for(const Box &box: state->getBoxes())
            result += SearchEngine::Predicate::distanceToClosestGoal(state, box);

        return result;
    }
};

}
#endif
