#include "master.h"
using SearchClient::Blackboard;
using namespace SearchEngine;

/* 
 * This is the primary function of the master class, that
 * conducts the main search
 */
void Master::conductSearch() {
    std::cerr << "--- Starting conductSearch! ---\n";

    std::cerr << "--- Posting goals for the state onto the blackboard ---\n";
    postBlackBoard();

    while (!SearchEngine::Predicate::isGoalState(&masterState_)) {
        SearchClient::Agent::setSharedState(&masterState_);
        SearchClient::JointAction ja = callForActions();
        jointActions_.push_back(ja); 

        /** TODO When updating the internal state, if a command from
         * one of the agents was found to be invalid due to another
         * agent, send a signal to the agent to recompute his plan
         */
        updateCurrentState(ja);
    }

    for (SearchClient::JointAction j : jointActions_) {
        for (Command& act : j.getData()) {
            std::cerr << act.toString();
        }
        std::cerr << std::endl;
    }

}

/* This adds all the goal tiles from the initial state to the blackboard */
void Master::postBlackBoard() {
    for (Goal& g : masterState_.goals) {
        Blackboard::Entry entry = Blackboard::Entry();
        entry.setBlackboardParent(nullptr);
        entry.setAuthorId(-1);  // -1 is master
        entry.setPosition(g.loc);
        entry.setType(Blackboard::Entry::GLOBAL_GOAL_ENTRY);
        masterBlackboard_.addEntry(entry);
    }
}

/* This function calls for actions from all the agents */
SearchClient::JointAction Master::callForActions() {
    Blackboard* bbptr = &masterBlackboard_;
    SearchClient::JointAction action = SearchClient::JointAction();
    action.initialize(agents_.size());
    for (size_t i = 0; i < agents_.size(); i++) {
        action.setAction(i, agents_[i].nextMove(bbptr, masterState_));
    } 
    return action;
}

/* This function updates the current state to reflect the previous joint actions */
void Master::updateCurrentState(SearchClient::JointAction ja) {
    std::vector<SearchEngine::Command> actions = ja.getData();
    for (size_t i = 0; i < actions.size(); i++) {
        
    }
}