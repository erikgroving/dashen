#include "greedyheuristic.h"
#include "../searchengine/distance.h"

using Heur::GreedyHeuristic;
using std::vector;


GreedyHeuristic::GreedyHeuristic(SearchClient::Agent* agentPtr)  : SearchEngine::Heuristic(agentPtr) {
}


unsigned long GreedyHeuristic::f(const SearchEngine::State* state) const {
    /** Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
     *
     */
    unsigned long result = 0;
    char agtIdx = getReferenceAgent()->getIndex();
    Coord agentLoc = state->getAgents()[agtIdx].loc;    
    Goal searchGoal = getReferenceAgent()->getCurrentTask().goal;


    for (Goal& g : SearchEngine::State::goals) {
        if (!SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            result += 100;
        }
    }


    /* Find the closest box to the goal */
    Box closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    result += Coord::distance(closestBox.loc, searchGoal.loc);
    /* Add the agent distance to box */
    result += Coord::distance(closestBox.loc, agentLoc);
    
    return result;
}

std::string GreedyHeuristic::name() const {
    return "Greedy";
}
