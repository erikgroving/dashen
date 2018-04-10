#ifndef SEARCHENGINE_AGENT_H
#define SEARCHENGINE_AGENT_H

#include <vector>
#include "../searchengine/searchengine"
#include "client.h"

namespace SearchClient {

class Client;
class Agent {

public:
    Agent (Color color, char num, Coord loc, SearchEngine::Strategy *strategy, SearchClient::Client *client);

    void updateGoalsList(const SearchEngine::State &initialState);
    void updateBoxesList(const SearchEngine::State &initialState);

    const Coord& getLocation() const { return loc; }
    Coord& getLocation() { return loc; }

    int getIndex() const { return num; }
    const SearchEngine::Strategy* getSearchStrategy() const { return searchStrategy_; }
    SearchEngine::Strategy* getSearchStrategy() { return searchStrategy_; }

    void setSearchStrategy(SearchEngine::Strategy *strategy) { searchStrategy_ = strategy; }

    /**
     * Return the highest priority goal
     */
    Goal chooseGoal();

    void sendPlan(const std::vector<SearchEngine::State*> &plan) const;
    
    /**
     * Compute a plan to accomplish all its goal.
     */
    void makeSearch();

public: // Search methods
    /**
     * Given a goal, return a sequence of action to accomplish it.
     */
    std::vector<SearchEngine::State*> searchGoal(const Goal &goal, SearchEngine::Strategy &strategy);
    /**
     * Returns a sequence of action that accomplishes all the goals according to the given strategy.
     */
    std::vector<SearchEngine::State*> searchAllGoals(SearchEngine::Strategy &strategy);

public: // Static public methods
    /**
     * Set the initial state for all the agents.
     * Note: Could trigger an event to indicate that the initial state has changed
     */
    static void setSharedState(SearchEngine::State *sharedState);

private:

    Color color;
    char num;
    Coord loc; // is it still useful ?

    SearchEngine::Strategy *searchStrategy_;
    std::vector<Goal> goalsToAchieve;
    std::vector<Box> movableBoxes;

    static SearchEngine::State *sharedState;
    SearchEngine::State *private_initialState;

    SearchClient::Client *client_;

};

}
#endif
