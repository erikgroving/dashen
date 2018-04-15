#include "master.h"
#include "searchclient.h"
#include <fstream>
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

    int round = 0;
    while (!SearchEngine::Predicate::isGoalState(&masterState_)) {
        SearchClient::Agent::setSharedState(&masterState_);
        SearchClient::JointAction ja = callForActions();
        jointActions_.push_back(ja); 

        /** TODO When updating the internal state, if a command from
         * one of the agents was found to be invalid due to the action of another
         * agent, send a signal to the agent to recompute his plan
         */
        updateCurrentState(ja);

        std::cerr << "\n------------ ROUND " << round++ << " ------------\n\n";
        std::cerr << "Joint Action: " << ja.toActionString(); 
        std::cout << ja.toActionString() << std::endl;
        std::cerr << std::endl;
        printMap(&masterState_);
        std::cerr<<std::endl;

    }

    std::cerr << "Sending solution. Length = " << jointActions_.size() << std::endl; 
    // Send the solution
    //sendSolution();
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
        int newAgentRow = masterState_.getAgents()[i].loc.y + Command::rowToInt(actions[i].d1());
        int newAgentCol = masterState_.getAgents()[i].loc.x + Command::colToInt(actions[i].d1());

        // TODO action has to have been valid in both previous and the 'in-progress' state
        // this currently only checks the in-progress state
        if (isActionValid(actions[i], i, newAgentCol, newAgentRow)) {
            updateStateWithNewMove(actions[i], i, newAgentCol, newAgentRow);
        } // TODO signal to agent that his plan has been made invalid
    }
}

bool Master::isActionValid(SearchEngine::Command cmd, char AgentID, int newAgentCol, int newAgentRow) {
    if (cmd.action() == Action::MOVE) {
        return SearchEngine::Predicate::isFree(&masterState_, newAgentCol, newAgentRow);
    }
    else if (cmd.action() == Action::PUSH) {
        int newBoxRow = newAgentRow + Command::rowToInt(cmd.d2());
        int newBoxCol = newAgentCol + Command::colToInt(cmd.d2());
        return (SearchEngine::Predicate::isFree(&masterState_, newBoxCol, newBoxRow) &&
                SearchEngine::Predicate::boxAt(&masterState_, newAgentCol, newAgentRow));
    }
    else if (cmd.action() == Action::PULL) {
        int boxRow = masterState_.getAgents()[AgentID].loc.y + Command::rowToInt(cmd.d2());
        int boxCol = masterState_.getAgents()[AgentID].loc.x + Command::colToInt(cmd.d2());
        return (SearchEngine::Predicate::isFree(&masterState_, newAgentCol, newAgentRow) &&
                SearchEngine::Predicate::boxAt(&masterState_, boxCol, boxRow));
    }
    return true; //NoOp
}

void Master::updateStateWithNewMove(SearchEngine::Command cmd, char AgentID, int newAgentCol, int newAgentRow) {
    vector<AgentDescription> agentDescs = masterState_.getAgents();
    vector<Box> boxes = masterState_.getBoxes();
    Action action = cmd.action();
    /* Box Movement */
    if (action == Action::PUSH || action == Action::PULL) {
        Coord currBoxPos;
        Coord newBoxPos;

        /* Get the current and new box positions */
        if (action == Action::PUSH) {
            currBoxPos = Coord(newAgentCol, newAgentRow);
            newBoxPos = Coord(newAgentCol + Command::colToInt(cmd.d2()), 
                                newAgentRow + Command::rowToInt(cmd.d2()));
        }
        else {
            Coord agentPos = masterState_.getAgents()[AgentID].loc;
            currBoxPos = Coord(agentPos.x + Command::colToInt(cmd.d2()),
                                agentPos.y + Command::rowToInt(cmd.d2()));
            newBoxPos = agentPos;
        }

        
        /* Find the box index, box is where the agent will be in a push */
        int boxID;
        for (size_t i = 0; i < boxes.size(); i++) {
            if (boxes[i].loc == currBoxPos) {
                boxID = i;
                break;
            }
        }
        boxes[boxID].loc = newBoxPos;
        masterState_.setBoxes(boxes);
    }

    /* Agent Movement */
    if (action == Action::MOVE || action == Action::PULL || action == Action::PUSH) {
        agentDescs[AgentID].loc.x = newAgentCol;
        agentDescs[AgentID].loc.y = newAgentRow;
        masterState_.setAgents(agentDescs);
    }
}

void Master::sendSolution () {
    for (SearchClient::JointAction ja : jointActions_) {
        std::cout << ja.toActionString() << std::endl;
    }
}