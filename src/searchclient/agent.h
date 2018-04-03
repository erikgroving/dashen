#ifndef SEARCHENGINE_AGENT_H
#define SEARCHENGINE_AGENT_H

#include <vector>
#include "../strategies/strategydfs.h"
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


    const Coord& getLocation() const { return loc; }
    Coord& getLocation() { return loc; }

    /**
     * Return the highest priority goal
     */
    Goal& chooseGoal();
    
    /**
     * Given a goal, return a sequence of action to accomplish it.
     */
    std::vector<State*> search(const Goal &goal);

    /**
     * Set the initial state for all the agents.
     * Note: Could trigger an event to indicate that the initial state has changed
     */
    static void setSharedState(State *sharedState);

private:

    std::vector<Goal> goalsToAchieve;
    std::vector<Box> movableBoxes;

    static State *sharedState;
    SearchEngine::State *private_initialState;

    Color color;
    char num;
    Coord loc;

};

}
#endif
