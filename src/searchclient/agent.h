#ifndef SEARCHCLIENT_AGENT_H
#define SEARCHCLIENT_AGENT_H

#include "../searchengine/state.h"
#include "../searchengine/typedefs.h"
#include "../searchengine/command.h"
#include "../searchengine/strategy.h"

#include "../communication/blackboard.h"

#include <algorithm>
#include <climits>
#include <vector>


namespace SearchClient {
    
class Agent {

public:
    Agent (Color color, char num, Coord loc, Communication::Blackboard *blackboard = nullptr);
    ~Agent();

    bool operator<(const Agent& a) const { return num < a.num;  }

    void updateGoalsList(const SearchEngine::State &initialState);
    void updateBoxesList(const SearchEngine::State &initialState);

    const Coord& getLocation() const { return loc; }
    Coord& getLocation() { return loc; }

    const short getCorrectGoals() const { return correctGoals_; }
    short getCorrectGoals() { return correctGoals_; }
    
    bool isFirstMoveInPlan() { return firstMoveInPlan_; }
    
    const Goal* getCurrentSearchGoal() const { return currentSearchGoal_; }
    Goal* getCurrentSearchGoal() { return currentSearchGoal_; }

    int getIndex() const { return num; }

    void setBlackboard(Communication::Blackboard *blackboard) { blackboard_ = blackboard; }
    
    const Communication::Blackboard* getBlackboard() const { return blackboard_; }
    Communication::Blackboard* getBlackboard() { return blackboard_; }

    const SearchEngine::Strategy* getSearchStrategy() const { return searchStrategy_; }
    SearchEngine::Strategy* getSearchStrategy() { return searchStrategy_; }

    const SearchEngine::State* getState() const { return sharedState; }
    SearchEngine::State* getState() { return sharedState; }
    
    // void askForHelp(const std::string &helpType, const std::vector<State*> &path);
    
    void setSearchStrategy(SearchEngine::Strategy *strategy) { searchStrategy_ = strategy; }
    void configurePrivateInitialState();
    void clearPlan(SearchEngine::Command);

    

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
    SearchEngine::Command nextMove();

    /**
     * Checks to see if the entry is doable by the agent, this currently means
     * that the agent checks that there is a box with th same color as the
     * agent that has the same letter as the goal
     */
    bool isEntryDoable(const Communication::BlackboardEntry *entry, const SearchEngine::State* state, int *boxIndex = 0);

    
    bool positionFree(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep);

    Goal getGoalFromBlackboard();

public: // Static public methods
    /**
     * Set the initial state for all the agents.
     * Note: Could trigger an event to indicate that the initial state has changed
     */
    static void setSharedState(SearchEngine::State *sharedState);
    static void setSharedTime(unsigned int timeStep);
    static unsigned int sharedTime;
    static SearchEngine::State *sharedState;
private:

    Color color;
    char num;
    Coord loc; // is it still useful ? not if we are using state


    SearchEngine::Strategy *searchStrategy_;
    std::vector<Goal> goalsToAchieve;
    std::vector<Box> movableBoxes;


    SearchEngine::State *private_initialState;

    Goal* currentSearchGoal_;
    /* Added with master class update. */
    std::vector<Goal> takenGoals_;   // goals taken down from blackboard.
                                    // help requests delete upon completion,
                                    // tile requests stay after completion
    std::vector<SearchEngine::Command> plan_;

    Communication::Blackboard *blackboard_;
    short correctGoals_;
    bool firstMoveInPlan_;
};

}
#endif
