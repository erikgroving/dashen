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

namespace Communication{
    class HelpEntry;
}
namespace SearchEngine {
    class Strategy;
}
namespace SearchClient {
    struct TaskInfo {
        TaskInfo(){}
        TaskInfo(TaskStackElement t, Communication::HelpEntry* e) {
            task = t;
            hEntryToPerform = e;
            waitingForHelp = false;
            hEntryToMonitor = std::vector<Communication::HelpEntry*>();
        }
        TaskInfo(const TaskInfo& src) {
            task = src.task;
            hEntryToPerform = src.hEntryToPerform;
            waitingForHelp = src.waitingForHelp;
            hEntryToMonitor = src.hEntryToMonitor;
        }
        TaskStackElement task;
        Communication::HelpEntry* hEntryToPerform;
        bool waitingForHelp;
        std::vector<Communication::HelpEntry*> hEntryToMonitor;
    };
class Agent {

public:
    Agent (Color color, char num, Coord loc, Communication::Blackboard *blackboard = nullptr);
    Agent(const Agent &src);
    ~Agent();

    bool operator<(const Agent& a) const { return num < a.num;  }

    const short getCorrectGoals() const { return correctGoals_; }
    short getCorrectGoals() { return correctGoals_; }
    
    const TaskStackElement getCurrentTask() const { return takenTasks_[ctIdx_].task; }
    TaskStackElement getCurrentTask() { return takenTasks_[ctIdx_].task; }

    int getIndex() const { return num; }

    void setBlackboard(Communication::Blackboard *blackboard) { blackboard_ = blackboard; }
    
    const Communication::Blackboard* getBlackboard() const { return blackboard_; }
    Communication::Blackboard* getBlackboard() { return blackboard_; }

    const SearchEngine::Strategy* getSearchStrategy() const { return searchStrategy_; }
    SearchEngine::Strategy* getSearchStrategy() { return searchStrategy_; }

    const SearchEngine::State* getState() const { return sharedState; }
    SearchEngine::State* getState() { return sharedState; }
    
    void identifyBlockingObjects(const std::vector<SearchEngine::State* > &path);
    void askForHelp(Coord agentLoc, char, std::vector<Coord> forbiddenCoords, int idx);

    void checkHelpEntries();   
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
    std::vector<SearchEngine::State*> searchClearSelf(SearchEngine::Strategy &strategy, bool ignoreOthers = false);
    std::vector<SearchEngine::State*> searchClearBox(SearchEngine::Strategy &strategy, bool ignoreOthers = false);
    std::vector<SearchEngine::State *> searchClearAgent(SearchEngine::Strategy &strategy, bool ignoreOthers);


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
    bool determineNextGoal();

    /* Conducts the search for a subgoal */
    std::vector<SearchEngine::State*> conductSubgoalSearch(bool *searchFailed);
    std::vector<SearchEngine::State*> conductGoalSearch(bool* searchFailed);
    std::vector<SearchEngine::State*> conductClearBoxSearch(bool* searchFailed);
    std::vector<SearchEngine::State*> conductClearSelfSearch(bool* searchFailed);


    /* Extracts the plan from a subgoal search */
    void extractPlan(const std::vector<SearchEngine::State*>& ans);

    /* Posts the position and box position entries */
    void postAllPositionEntries(const std::vector<SearchEngine::State*>& ans);

    Coord findBoxTargetWithBFS(int boxID);
    bool isTaskSatisfied(SearchEngine::State* state, TaskStackElement t);

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
    SearchEngine::Strategy *searchStrategy_;
    SearchEngine::State *private_initialState;

    /* Tasks can be Goal, Clear self (out of way), clear box, or clear box and self. */
    /* Clear box and self devolves into two tasks, clear self, and then clear box */
    int ctIdx_; // current task index
    std::vector<TaskInfo> takenTasks_;

    std::vector<SearchEngine::Command> plan_;
    Communication::Blackboard *blackboard_;
    short correctGoals_;
};

}
#endif
