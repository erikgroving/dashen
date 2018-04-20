#include "greedyheuristic.h"
#include "../searchengine/distance.h"

using Heur::GreedyHeuristic;
using std::vector;


GreedyHeuristic::GreedyHeuristic(const SearchClient::Agent* agentPtr)  : Heuristic(agentPtr), agentPtr_(agentPtr) {
}


unsigned long GreedyHeuristic::f(const SearchEngine::State* state) const {
    /** Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
     *
     */
    unsigned long result = 0;
    char agtIdx = agentPtr_->getIndex();
    vector<AgentDescription> agents = state->getAgents();
    Coord agentLoc = state->getAgents()[agtIdx].loc;    
    Goal searchGoal = *(agentPtr_->getCurrentSearchGoal());


    for (Goal& g : SearchEngine::State::goals) {
        if (!SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            result += 100;
        }
    }


    /* Find the closest box to the goal */
    Box closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    result += abs(closestBox.loc.x - searchGoal.loc.x) + abs(closestBox.loc.y - searchGoal.loc.y);
    /* Add the agent distance to box */
    result += abs(closestBox.loc.x - agentLoc.x) + abs(closestBox.loc.y - agentLoc.y);
    
    return result;
}

unsigned long GreedyHeuristic::heuristic(const SearchEngine::State* state) const {
    return f(state);
}

std::string GreedyHeuristic::name() const {
    return "Greedy";
}