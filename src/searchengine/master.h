#ifndef MASTER_H
#define MASTER_H

#include "../searchclient/searchclient"
#include "goalpriority.h"
#include <vector>

namespace SearchEngine {
class Master {

public: // Constructors
    Master();
    Master(const State& s1, const std::vector<SearchClient::Agent> &agents);
    Master(const Master& m);

public:
    /* Conduct a search */
    void conductSearch();
    /* Post the goals identified to the blackboard */
    void postBlackBoard();
    /* Calls for joint actions from the agents, returns a joint action */
    SearchClient::JointAction callForActions();
    /* Update the current state to reflect the previous actions */
    void updateCurrentState(SearchClient::JointAction*);
    /* Sends the solution once a universal goal state is found */
    void sendSolution();
    /* Checks if a sub-action in a joint action plan is valid, used by updateCurrentState */
    bool isActionValid(SearchEngine::State*, SearchEngine::Command, char);
    /* Performs a sub-action on the internal state, used by updateCurrentState*/
    void updateStateWithNewMove(SearchEngine::Command, char);

    void computeGoalPriorities();
    void revokeBlackboardEntries(SearchClient::JointAction);
    void printBlackboard(Communication::Blackboard* b);

private:
    std::vector<SearchClient::JointAction> jointActions_;
    State masterState_;
    State prevMasterState_;
    std::vector<SearchClient::Agent> agents_;
    Communication::Blackboard masterBlackboard_;

};

}

#endif
