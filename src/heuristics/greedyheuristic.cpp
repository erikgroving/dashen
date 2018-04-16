#include "greedyheuristic.h"
#include "../searchengine/distance.h"

using Heur::GreedyHeuristic;
using std::vector;


GreedyHeuristic::GreedyHeuristic(const SearchClient::Agent* agentPtr)  : Heuristic(agentPtr), agentPtr_(agentPtr) {
    std::cerr << "Greedy heuristic was constructed!" << std::endl;
}


unsigned long GreedyHeuristic::f(const SearchEngine::State* state) const {
    /** Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
     * Using hamiltonian distances like a scrub
     */
    char agtIdx = agentPtr_->getIndex();
    vector<AgentDescription> agents = state->getAgents();
    Coord agentLoc = state->getAgents()[agtIdx].loc;    
    Goal searchGoal = *(agentPtr_->getCurrentSearchGoal());

    /* Find the closest box to the goal */
    Box closestBox;
    unsigned short minDist = 50000;
    for (Box b : state->getBoxes()) {
        if (b.letter == searchGoal.letter) {
            unsigned short dist = abs(b.loc.x - searchGoal.loc.x) + abs(b.loc.y - searchGoal.loc.y);
            if (dist < minDist) {
                minDist = dist;
                closestBox = b;
            }
        }
    }

    unsigned long result = minDist;
    /* Add the agent distance to box */
    result += abs(closestBox.loc.x - agentLoc.x) + abs(closestBox.loc.x - agentLoc.y);
    
    return result;
}

unsigned long GreedyHeuristic::heuristic(const SearchEngine::State* state) const {
    return f(state);
}

std::string GreedyHeuristic::name() const {
    return "Greedy";
}