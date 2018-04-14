#ifndef MASTER_H
#define MASTER_H

#include "../searchclient/searchclient"
#include <vector>

using SearchClient::JointAction;
using std::vector;

namespace SearchEngine {
    class Master {
        vector<JointAction> jointActions;
        public:
            /* Constructor, copy constructor, and destructor */
            Master() { jointActions = vector<JointAction>(); } 
            Master(const Master& m) { jointActions = m.jointActions; }
            ~Master() {}
            
            /* Conduct a search */
            void conductSearch();
            /* Identifies the goals in a state */
            void identifyGoals();
            /* Post the goals identified to the blackboard */
            void postBlackBoard();
            /* Calls for joint actions from the agents, returns a joint action */
            JointAction callForActions();
            /* Update the current state to reflect the previous actions */
            void updateCurrentState();
            /* Sends the joint plan once a universal goal state is found */
            void sendJointPlan();
    };
}

#endif