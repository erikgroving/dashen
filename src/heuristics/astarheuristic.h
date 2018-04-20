#ifndef HEURISTICS_ASTAR
#define HEURISTICS_ASTAR

#include "../searchengine/searchengine"

namespace Heur {
    class AgentToBoxAStarHeuristic: public SearchEngine::Heuristic
    {
    private:
        const SearchClient::Agent* agentPtr_;
    public:    
        AgentToBoxAStarHeuristic(const SearchClient::Agent* agentPtr);
        unsigned long f(const SearchEngine::State *state) const;
        unsigned long heuristic(const SearchEngine::State *state) const;  
        std::string name() const;
    };
    
    class BoxToGoalAStarHeuristic: public SearchEngine::Heuristic
    {
    private:
        const SearchClient::Agent* agentPtr_;
    public:    
        BoxToGoalAStarHeuristic(const SearchClient::Agent* agentPtr);
        unsigned long f(const SearchEngine::State *state) const;
        unsigned long heuristic(const SearchEngine::State *state) const;  
        std::string name() const;
    };
}
/*
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
*/
#endif
