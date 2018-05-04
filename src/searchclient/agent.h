#ifndef SEARCHCLIENT_AGENT_H
#define SEARCHCLIENT_AGENT_H

#include "../searchengine/state.h"
#include "../searchengine/typedefs.h"
#include "../searchengine/command.h"


#include "../communication/blackboard.h"

#include <algorithm>
#include <climits>
#include <vector>
#include <iostream>

namespace SearchEngine {
    class Strategy;
}
namespace SearchClient {

struct HelpGoal {

    enum HelpType {
        None,
        Agent,
        Box
    } type;

    HelpGoal(HelpType htype = None, int idd = -1, const Coord& hposition = Coord(), std::vector<Coord> hforbiddenPath = std::vector<Coord>(), bool hover = true):
        type(htype), id(idd), position(hposition), forbiddenPath(hforbiddenPath), over(hover){

    }
    HelpGoal(const HelpGoal &src):
        type(src.type), id(src.id), position(src.position), forbiddenPath(src.forbiddenPath), over(src.over) {}


    int id;
    Coord position;
    std::vector<Coord> forbiddenPath;
    bool over;
};

class Agent {

public:
    Agent (Color color, char num, Coord loc, Communication::Blackboard *blackboard = nullptr);
    Agent(const Agent &src);
    ~Agent();

    bool operator<(const Agent& a) const { return num < a.num;  }

    const Coord& getLocation() const { return loc; }
    Coord& getLocation() { return loc; }

    const short getCorrectGoals() const { return correctGoals_; }
    short getCorrectGoals() { return correctGoals_; }
    
    bool isFirstMoveInPlan() { return firstMoveInPlan_; }
    const bool isHelpGoal() const { return currGoalIsHelpGoal_; }
    
    const Goal getCurrentSearchGoal() const { return currentSearchGoal_; }
    Goal getCurrentSearchGoal() { return currentSearchGoal_; }
    
    const HelpGoal getCurrentHelpGoal() const { return currentHelpGoal_; }
    HelpGoal getCurrentHelpGoal() { return currentHelpGoal_; }

    int getIndex() const { return num; }

    void setBlackboard(Communication::Blackboard *blackboard) { blackboard_ = blackboard; }
    
    const Communication::Blackboard* getBlackboard() const { return blackboard_; }
    Communication::Blackboard* getBlackboard() { return blackboard_; }

    const SearchEngine::Strategy* getSearchStrategy() const { return searchStrategy_; }
    SearchEngine::Strategy* getSearchStrategy() { return searchStrategy_; }

    const SearchEngine::State* getState() const { return sharedState; }
    SearchEngine::State* getState() { return sharedState; }
    
    void askForHelp(const std::vector<SearchEngine::State *> &path);
    
    void setSearchStrategy(SearchEngine::Strategy *strategy) { searchStrategy_ = strategy; }
    void configurePrivateInitialState();
    void initialStateRemovedAllBut(char agentIndex, char boxIndex);
    void clearPlan(SearchEngine::Command);



public: // Search methods
    /**
     * Given a goal, return a sequence of action to accomplish it.
     */
    std::vector<SearchEngine::State*> searchGoal(const Goal &goal, SearchEngine::Strategy& strategy, bool ignoreOthers = false);

    /**
     * Given a box, agent returns a sequence of actions to get to the box
     */
    std::vector<SearchEngine::State*> searchBox(const Box& box, SearchEngine::Strategy& strategy, bool ignoreOthers = false);
    
    /**
     * Returns a sequence of action that accomplishes all the goals according to the given strategy.
     */
    std::vector<SearchEngine::State*> searchAllGoals(SearchEngine::Strategy &strategy, bool ignoreOthers = false);
    std::vector<SearchEngine::State*> searchHelpMoveAgent(SearchEngine::Strategy &strategy, bool ignoreOthers = false);
    std::vector<SearchEngine::State*> searchHelpMoveBox(SearchEngine::Strategy &strategy, bool ignoreOthers = false);


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

    
    bool positionFree(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep, std::string &errorDescription) const;

    /* Fetches a goal from the blackboard, used by determineNextGoal */
    Goal getGoalFromBlackboard();

    /* Determines the next goal for the agent */
    bool determineNextGoal(bool *isHelpGoal);

    /* Conducts the search for a subgoal */
    std::vector<SearchEngine::State*> conductSubgoalSearch(bool *searchFailed);

    std::vector<SearchEngine::State*> conductHelpSubgoalSearch();

    /* Extracts the plan from a subgoal search */
    void extractPlan(const std::vector<SearchEngine::State*>& ans);

    /* Posts the position and box position entries */
    void postAllPositionEntries(const std::vector<SearchEngine::State*>& ans);

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

    bool currGoalIsHelpGoal_;
    SearchEngine::Strategy *searchStrategy_;


    SearchEngine::State *private_initialState;

    Goal currentSearchGoal_;

    // Help
    HelpGoal currentHelpGoal_;
    Communication::BlackboardEntry* currentHelpEntry_;

    /* Added with master class update. */
    std::vector<Goal> takenGoals_;   // goals taken down from blackboard.
                                    // help requests delete upon completion,
                                    // tile requests stay after completion
    std::vector<SearchEngine::Command> plan_;

    Communication::Blackboard *blackboard_;
    short correctGoals_;
    bool firstMoveInPlan_;

    bool isWaitingForHelp;
    std::vector<Communication::BlackboardEntry*> helpEntriesToMonitor_;
};

}
#endif
