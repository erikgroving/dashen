#include "astarheuristic.h"

using Heur::AgentToBoxAStarHeuristic;
using Heur::BoxToGoalAStarHeuristic;
using Heur::DistanceOracle;
using Heur::AStarHeuristic;

AStarHeuristic::AStarHeuristic(SearchClient::Agent* agentPtr, unsigned int f_weight): SearchEngine::Heuristic(agentPtr),
    f_weight_(f_weight) {

}

unsigned long AgentToBoxAStarHeuristic::f(const SearchEngine::State* state) const {

    unsigned long result = 0;

    Coord agentLoc = state->getAgents()[getReferenceAgent()->getIndex()].loc;

    bool isHelpGoal = getReferenceAgent()->isHelpGoal();

    short currCorrectGoals = 0;
    short startSearchCorrectgoals = getReferenceAgent()->getCorrectGoals();
    for (Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            currCorrectGoals += 1;
        }
    }

    result += (1 + startSearchCorrectgoals - currCorrectGoals) * 10;



    /* Find the box assigned to the goal */
    Box closestBox;
    if (isHelpGoal) {
        closestBox = state->getBoxes()[getReferenceAgent()->getCurrentHelpGoal().id];
    }
    else {
        Goal searchGoal = getReferenceAgent()->getCurrentSearchGoal();
        closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    }
    //Box closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    
    
    /* Add the agent distance to box */
    result += DistanceOracle::fetchDistFromCoord(closestBox.loc, agentLoc);
    //std::cerr << "This node was only: " << DistanceOracle::fetchDistFromCoord(closestBox.loc, agentLoc) << " away\n, result was: " << result << std::endl;
    //result += Coord::distance(closestBox.loc, agentLoc);
    return result;
}

unsigned long BoxToGoalAStarHeuristic::f(const SearchEngine::State* state) const {
     /*
      * Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
      */
    unsigned long result = 0;
    Goal searchGoal = getReferenceAgent()->getCurrentSearchGoal();

    short currCorrectGoals = 0;
    short startSearchCorrectgoals = getReferenceAgent()->getCorrectGoals();
    for (Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            currCorrectGoals += 1;
        }
    }

    result += 1 + startSearchCorrectgoals - currCorrectGoals;

    /* Find the box assigned to the goal */
    Box closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    result += DistanceOracle::fetchDistFromCoord(searchGoal.loc, closestBox.loc);
    //result += abs(closestBox.loc.x - searchGoal.loc.x) + abs(closestBox.loc.y - searchGoal.loc.y);
    return result;
}
