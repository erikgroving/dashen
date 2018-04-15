#ifndef MASTER_H
#define MASTER_H

#include "../searchclient/searchclient"
#include <vector>

using SearchEngine::State;
using SearchClient::Blackboard;
using SearchClient::Agent;
using std::vector;

namespace SearchEngine {
    class Master {
        vector<SearchClient::JointAction> jointActions_;
        State masterState_;
        vector<Agent> agents_;
        Blackboard masterBlackboard_;

        public:
        
            /* Constructor, copy constructor, and destructor */
            Master() { jointActions_ = vector<SearchClient::JointAction>(); } 
            Master(State& s1, vector<Agent> agents) {
                masterState_ = s1; 
                agents_ = agents;
                jointActions_ = vector<SearchClient::JointAction>(); 
                masterBlackboard_ = Blackboard();
            }
            Master(const Master& m) { jointActions_ = m.jointActions_; }
            ~Master() {}
            
            /* Conduct a search */
            void conductSearch();
            /* Post the goals identified to the blackboard */
            void postBlackBoard();
            /* Calls for joint actions from the agents, returns a joint action */
            SearchClient::JointAction callForActions();
            /* Update the current state to reflect the previous actions */
            void updateCurrentState(SearchClient::JointAction);
            /* Sends the joint plan once a universal goal state is found */
            void sendJointPlan();
    };
}

#endif