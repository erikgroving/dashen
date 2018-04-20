#ifndef SEARCHENGINE_AGENT_H
#define SEARCHENGINE_AGENT_H

#include <vector>
#include "../searchengine/searchengine"
#include "blackboard.h"
#include "blackboardentry.h"
#include "client.h"

namespace SearchClient {
    
class Blackboard;
class Client;
class Agent {

public:
    Agent (Color color, char num, Coord loc, SearchEngine::Strategy *strategy = nullptr, SearchClient::Blackboard *blackboard = nullptr);
    
    bool operator<(const Agent& a) const { return num < a.num;  }

    void updateGoalsList(const SearchEngine::State &initialState);
    void updateBoxesList(const SearchEngine::State &initialState);

    const Coord& getLocation() const { return loc; }
    Coord& getLocation() { return loc; }

    const Goal* getCurrentSearchGoal() const { return currentSearchGoal_; }
    Goal* getCurrentSearchGoal() { return currentSearchGoal_; }

    int getIndex() const { return num; }

    void setBlackboard(SearchClient::Blackboard *blackboard) { blackboard_ = blackboard; }
    
    const Blackboard* getBlackboard() const { return blackboard_; }
    Blackboard* getBlackboard() { return blackboard_; }

    const SearchEngine::Strategy* getSearchStrategy() const { return searchStrategy_; }
    SearchEngine::Strategy* getSearchStrategy() { return searchStrategy_; }

    void setSearchStrategy(SearchEngine::Strategy *strategy) { searchStrategy_ = strategy; }

    /**
     * Return the highest priority goal
     */
    Goal chooseGoal();

public: // Search methods
    /**
     * Given a goal, return a sequence of action to accomplish it.
     */
    std::vector<SearchEngine::State*> searchGoal(const Goal &goal, SearchEngine::Strategy* strategy);

    /**
     * Given a box, agent returns a sequence of actions to get to the box
     */
    std::vector<SearchEngine::State*> searchBox(const Box& box, SearchEngine::Strategy* strategy);
    
    /**
     * Returns a sequence of action that accomplishes all the goals according to the given strategy.
     */
    std::vector<SearchEngine::State*> searchAllGoals(SearchEngine::Strategy &strategy);

    /**
     * Returns the next move in the plan if there is one. otherwise
     * will generate a new plan and return head of that plan
     */
    SearchEngine::Command nextMove(SearchClient::Blackboard* b, SearchEngine::State s);

    /**
     * Checks to see if the entry is doable by the agent, this currently means
     * that the agent checks that there is a box with th same color as the
     * agent that has the same letter as the goal
     */
    bool isEntryDoable(Goal& g, SearchEngine::State& s); 

    
    bool positionFree(size_t x, size_t y, unsigned int timeStep);

    Goal getGoalFromBlackboard(SearchClient::Blackboard* b, SearchEngine::State s);

public: // Static public methods
    /**
     * Set the initial state for all the agents.
     * Note: Could trigger an event to indicate that the initial state has changed
     */
    static void setSharedState(SearchEngine::State *sharedState);
    static void setSharedTime(unsigned int timeStep);
    static unsigned int sharedTime;
private:

    Color color;
    char num;
    Coord loc; // is it still useful ? not if we are using state

    SearchEngine::Strategy *searchStrategy_;
    std::vector<Goal> goalsToAchieve;
    std::vector<Box> movableBoxes;

    static SearchEngine::State *sharedState;

    SearchEngine::State *private_initialState;

    Goal* currentSearchGoal_;
    /* Added with master class update. */
    std::vector<Goal> takenGoals_;   // goals taken down from blackboard.
                                    // help requests delete upon completion,
                                    // tile requests stay after completion
    std::vector<SearchEngine::Command> plan_;

    SearchClient::Blackboard *blackboard_;
};

}
#endif
