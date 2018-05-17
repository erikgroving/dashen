#ifndef MASTER_H
#define MASTER_H

#include "../searchclient/searchclient"
#include "deadend.h"
#include "goalpriority.h"
#include <vector>

namespace SearchEngine {

struct SortPrio{
    int idx;
    unsigned int priority;
    bool operator<(const SortPrio &src) {
        return priority < src.priority;
    }
};

class Master {

public: // Constructors
    Master();
    Master(const State& s1, const std::vector<SearchClient::Agent> &agents);
    Master(const Master& m);

public:
    static Deadend deadends;

public:
    /* Conduct a search */
    void conductSearch();
    /* Post the goals identified to the blackboard */
    void postBlackBoard();
    /* Assigns boxes to all goals */
    void assignBoxesToGoals();
    /* Calls for joint actions from the agents, returns a joint action */
    SearchClient::JointAction callForActions(std::vector<int>&);
    /* Update the current state to reflect the previous actions */
    void updateCurrentState(SearchClient::JointAction*, std::vector<int>&);
    /* Sends the solution once a universal goal state is found */
    void sendSolution();
    /* Checks if a sub-action in a joint action plan is valid, used by updateCurrentState */
    bool isActionValid(SearchEngine::State*, SearchEngine::Command, char);
    /* Performs a sub-action on the internal state, used by updateCurrentState*/
    void updateStateWithNewMove(SearchEngine::Command, char);


    bool allNoOps(SearchClient::JointAction ja);
    void nukeHelp();
    void computeGoalPriorities();
    void clearCompleteUntakenHelpEntries();
    void revokeBlackboardEntries(SearchClient::JointAction);
    void printBlackboard(Communication::Blackboard* b);

private:
    std::vector<SearchClient::JointAction> jointActions_;
    std::vector<unsigned int> goalPriorities_;
    bool noopMove_;
    State masterState_;
    State prevMasterState_;
    std::vector<SearchClient::Agent> agents_;
    Communication::Blackboard masterBlackboard_;

};

}

#endif
