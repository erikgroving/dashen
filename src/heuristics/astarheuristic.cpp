#include "astarheuristic.h"

using Heur::AgentToBoxAStarHeuristic;
using Heur::BoxToTargetAStarHeuristic;
using Heur::BoxToGoalAStarHeuristic;
using Heur::DistanceOracle;
using Heur::AStarHeuristic;

AStarHeuristic::AStarHeuristic(SearchClient::Agent* agentPtr, unsigned int f_weight): SearchEngine::Heuristic(agentPtr),
    f_weight_(f_weight) {

}

unsigned long AgentToBoxAStarHeuristic::f(const SearchEngine::State* state) const {

    unsigned long result = 0;

    Coord agentLoc = state->getAgents()[getReferenceAgent()->getIndex()].loc;

    short currCorrectGoals = 0;
    short startSearchCorrectgoals = getReferenceAgent()->getCorrectGoals();
    for (Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            currCorrectGoals += 1;
        }
    }
    result += (1 + startSearchCorrectgoals - currCorrectGoals) * 20;

    if (state->getAction().action() != MOVE) {
        result += 2;
    }


    /* Find the box assigned to the goal */
    TaskStackElement task = getReferenceAgent()->getCurrentTask();
    int targetBoxIdx;
    Box targetBox;
    if (task.type == GOAL) {
        targetBoxIdx = task.goal.assignedBoxID;
    }
    else {
        targetBoxIdx = task.clearBox.boxToMoveID;
    }

    targetBox = state->getBoxes()[targetBoxIdx];
    
    /* Add the agent distance to box */
    result += DistanceOracle::fetchDistFromCoord(targetBox.loc, agentLoc);
    return result;
}

unsigned long BoxToTargetAStarHeuristic::f(const SearchEngine::State* state) const {
    unsigned long result = 0;
    
    short currCorrectGoals = 0;
    short startSearchCorrectgoals = getReferenceAgent()->getCorrectGoals();
    for (Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            currCorrectGoals += 1;
        }
    }

    result += 1 + startSearchCorrectgoals - currCorrectGoals;

    /* Find the box assigned to the goal */
    Coord target = getReferenceAgent()->getCurrentTask().clearBox.target;
    Coord boxLoc = state->getBoxes()[getReferenceAgent()->getCurrentTask().clearBox.boxToMoveID].loc;
    result += DistanceOracle::fetchDistFromCoord(target, boxLoc);
    return result;
}

unsigned long BoxToGoalAStarHeuristic::f(const SearchEngine::State* state) const {
     /*
      * Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
      */
    unsigned long result = 0;
    Goal searchGoal = getReferenceAgent()->getCurrentTask().goal;

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
