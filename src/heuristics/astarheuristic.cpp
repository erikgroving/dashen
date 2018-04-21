#include "astarheuristic.h"

using Heur::AgentToBoxAStarHeuristic;
using Heur::BoxToGoalAStarHeuristic;
using std::vector;


AgentToBoxAStarHeuristic::AgentToBoxAStarHeuristic(const SearchClient::Agent* agentPtr)  : Heuristic(agentPtr), agentPtr_(agentPtr) {
}


unsigned long AgentToBoxAStarHeuristic::f(const SearchEngine::State* state) const {
     /*
      * Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
      */
    unsigned long result = 0;
    char agtIdx = agentPtr_->getIndex();
    vector<AgentDescription> agents = state->getAgents();
    Coord agentLoc = state->getAgents()[agtIdx].loc;    
    Goal searchGoal = *(agentPtr_->getCurrentSearchGoal());

    short currCorrectGoals = 0;
    short startSearchCorrectgoals = agentPtr_->getCorrectGoals();
    for (Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            currCorrectGoals += 1;
        }
    }

    result += (startSearchCorrectgoals - currCorrectGoals) * 10;


    /* Find the box assigned to the goal */
    Box closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    /* Add the agent distance to box */
    result += Coord::distance(closestBox.loc, agentLoc);
    
    return result;
}

unsigned long AgentToBoxAStarHeuristic::heuristic(const SearchEngine::State* state) const {
    return f(state) + state->getPathCost();
}

std::string AgentToBoxAStarHeuristic::name() const {
    return "AStar Agent To Box";
}


BoxToGoalAStarHeuristic::BoxToGoalAStarHeuristic(const SearchClient::Agent* agentPtr)  : Heuristic(agentPtr), agentPtr_(agentPtr) {
}


unsigned long BoxToGoalAStarHeuristic::f(const SearchEngine::State* state) const {
     /*
      * Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
      */
    unsigned long result = 0;
    vector<AgentDescription> agents = state->getAgents();
    Goal searchGoal = *(agentPtr_->getCurrentSearchGoal());

    short currCorrectGoals = 0;
    short startSearchCorrectgoals = agentPtr_->getCorrectGoals();
    for (Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            currCorrectGoals += 1;
        }
    }

    result += (startSearchCorrectgoals - currCorrectGoals);


    /* Find the box assigned to the goal */
    Box closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    result += abs(closestBox.loc.x - searchGoal.loc.x) + abs(closestBox.loc.y - searchGoal.loc.y);
    return result;
}

unsigned long BoxToGoalAStarHeuristic::heuristic(const SearchEngine::State* state) const {
    return f(state) + state->getPathCost();
}

std::string BoxToGoalAStarHeuristic::name() const {
    return "AStar - Box To Goal";
}
