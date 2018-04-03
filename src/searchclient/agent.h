#ifndef SEARCHENGINE_AGENT_H
#define SEARCHENGINE_AGENT_H

#include <vector>
#include "src/strategies/strategydfs.h"
#include "state.h"
#include "typedefs.h"
#include "searchclient.h"

namespace SearchEngine {

class Agent {

public:
    Agent (Color color, char num, Coord loc) :  
        color(color), num(num), loc(loc), 
        goalsToAchieve(), movableBoxes(), 
        private_initialState() {}

    void updateGoalsList(const SearchEngine::State &initialState);
    void updateBoxesList(const SearchEngine::State &initialState);

    /**
     * Return the highest priority goal
     */
    Goal& chooseGoal();
    
    /**
     * Given a goal, return a sequence of action to accomplish it.
     */
    std::vector<SearchEngine::State*> search(const Goal &goal);

    /**
     * Set the initial state for all the agents.
     * Note: Could trigger an event to indicate that the initial state has changed
     */
    static void setSharedState(const State &sharedState);

private:

    std::vector<Goal> goalsToAchieve;
    std::vector<Box> movableBoxes;

    static State sharedState;
    State private_initialState;

    Color color;
    char num;
    Coord loc;
};

}
#endif