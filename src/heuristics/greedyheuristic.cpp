#include "greedyheuristic.h"
#include "../searchengine/distance.h"

using Heur::GreedyHeuristic;
using std::vector;


GreedyHeuristic::GreedyHeuristic(const SearchClient::Agent* agentPtr)  : Heuristic(agentPtr), agentPtr_(agentPtr) {
}


unsigned long GreedyHeuristic::f(const SearchEngine::State* state) const {
    /** Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
     * Using hamiltonian distances like a scrub
     */
    unsigned long result = 0;
    char agtIdx = agentPtr_->getIndex();
    vector<AgentDescription> agents = state->getAgents();
    Coord agentLoc = state->getAgents()[agtIdx].loc;    
    Goal searchGoal = *(agentPtr_->getCurrentSearchGoal());


    for (Goal g : SearchEngine::State::goals) {
        if (!SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            result += 100;
        }
    }


    /* Find the closest box to the goal */
    Box closestBox;
    unsigned long minDist = 50000;
    for (Box b : state->getBoxes()) {
        if (b.letter == searchGoal.letter) {
            unsigned long dist = SearchEngine::State::distance.getDistance(b.loc, searchGoal.loc);
            if (dist == (unsigned long)-1) dist = 10000;
            if (dist < minDist) {
                minDist = dist;
                closestBox = b;
            }
        }
    }

    result += minDist;
    /* Add the agent distance to box */
    result += SearchEngine::State::distance.getDistance(closestBox.loc, agentLoc);
    
    return result;
}

unsigned long GreedyHeuristic::heuristic(const SearchEngine::State* state) const {
    return f(state);
}

std::string GreedyHeuristic::name() const {
    return "Greedy";
}