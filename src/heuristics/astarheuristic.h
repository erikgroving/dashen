#ifndef HEURISTICS_ASTAR
#define HEURISTICS_ASTAR

#include "../searchengine/searchengine"
#include "distanceoracle.h"
#include <vector>

namespace Heur {

/**
 * \brief Base class for the AStar heuristic
 */
class AStarHeuristic: public SearchEngine::Heuristic {

public:
    AStarHeuristic(SearchClient::Agent* agentPtr, unsigned int f_weight = 1);
    virtual unsigned long f(const SearchEngine::State *state) const = 0;
    unsigned long heuristic(const SearchEngine::State *state) const {
        return state->getPathCost() + f_weight_ * f(state);
    }

    void setHeuristicWeight(unsigned int nWeight) { f_weight_ = nWeight; }
    unsigned int getHeuristicWeight() const { return f_weight_; }

private:
    unsigned int f_weight_;
};

/**
 * \brief Heuristic used to compute the path from a agent to a box.
 *
 * Computation:
 * f(state) = #(goals that have been unsatisfied by the sequence of moves)*10 + (real distance agent to box)
 */
class AgentToBoxAStarHeuristic: public AStarHeuristic
{

public:
    AgentToBoxAStarHeuristic(SearchClient::Agent *agentPtr, unsigned int f_weight = 1): AStarHeuristic(agentPtr, f_weight) {
        setHeuristicWeight(2);
    }

    unsigned long f(const SearchEngine::State *state) const;
    std::string name() const { return "AStar Agent To Box"; }
};

/**
 * \brief Heuristic used to compute the path from a agent with a box to a goal.
 *
 * Computation:
 * f(state) = #(goals that have been unsatisfied by the sequence of moves)*10 + (real distance agent to box)
 */
class BoxToGoalAStarHeuristic: public AStarHeuristic
{
private:
    const SearchClient::Agent* agentPtr_;
public:
    BoxToGoalAStarHeuristic(SearchClient::Agent *agentPtr, unsigned int f_weight = 1): AStarHeuristic(agentPtr, f_weight) {
        setHeuristicWeight(2);
    }
    unsigned long f(const SearchEngine::State *state) const;
    std::string name() const { return "AStar - Box To Goal"; }
};

    std::vector< std::vector<unsigned long> > calculateDistancesFromPoint(Coord point, const SearchEngine::State* s);

}

#endif
