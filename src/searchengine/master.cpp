#include "master.h"
#include "searchclient.h"

using SearchClient::Blackboard;
using SearchClient::BlackboardEntry;
using namespace SearchEngine;
void printBlackboard(SearchClient::Blackboard* b);

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
        std::cerr << "\n------------ ROUND " << round++ << " ------------\n\n";
        //printBlackboard(&masterBlackboard_);
        SearchClient::Agent::setSharedState(&masterState_);
        SearchClient::JointAction ja = callForActions();
        jointActions_.push_back(ja); 

        /** TODO When updating the internal state, if a command from
         * one of the agents was found to be invalid due to the action of another
         * agent, send a signal to the agent to recompute his plan
         */
        revokeBlackboardEntries(ja);
        updateCurrentState(ja);

        std::cerr << "Joint Action: " << ja.toActionString() << std::endl; 
        std::cout << ja.toActionString() << std::endl;
        std::string x;
        for (int i = 0; i < agents_.size(); i++) {
            std::cerr << "Agent " << i << " after loc: (" << masterState_.getAgents()[i].loc.y << "," << masterState_.getAgents()[i].loc.x << ")\n";
            std::cin >> x;
            std::cerr << x << std::endl;
            if (x=="false," || x=="false]") {
                std::cerr << "Move " << i << " was bad." << std::endl;
                int sum = 0;
                for (int i = 0; i < 10000; i++) {
                    sum+= i;
                }
                std::cerr << "some sum: " << sum << std::endl;
                exit(0);
            }
        }
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
        BlackboardEntry *entry = BlackboardEntry::create(BlackboardEntry::GLOBAL_GOAL_ENTRY, &masterBlackboard_);
        entry->setPosition(g.loc);
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
        else {
            agents_[i].clearPlan();
        }
    }
}

bool Master::isActionValid(SearchEngine::Command cmd, char AgentID, int newAgentCol, int newAgentRow) {
    if (cmd.action() == Action::MOVE) {
        return SearchEngine::Predicate::isFree(&masterState_, newAgentCol, newAgentRow);
    }
    else if (cmd.action() == Action::PUSH) {
        int newBoxRow = newAgentRow + Command::rowToInt(cmd.d2());
        int newBoxCol = newAgentCol + Command::colToInt(cmd.d2());
        std::cerr << "Result: " <<  SearchEngine::Predicate::isFree(&masterState_, newBoxCol, newBoxRow) <<
         " Agent: " << (int)AgentID << " New Box Pos: (" << newBoxRow << "," << newBoxCol << ")" << std::endl;
         std::cerr << "Current agent pos: (" << masterState_.getAgents()[AgentID].loc.y << "," <<
            masterState_.getAgents()[AgentID].loc.x << ")" << std::endl;
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

void Master::revokeBlackboardEntries(SearchClient::JointAction ja) {
    auto commands = ja.getData();
    for (unsigned int i = 0; i < commands.size(); i++) {
        if (commands[i].action() != NOOP) {
            unsigned int sharedTime = SearchClient::Agent::sharedTime;
            SearchClient::BlackboardEntry::revoke(masterBlackboard_.findPositionEntry(sharedTime, i), agents_[i]);
            if (agents_[i].getIsFirstMoveInPlan()) {
                SearchClient::BlackboardEntry::revoke(masterBlackboard_.findPositionEntry(sharedTime - 1, i), agents_[i]);
            }
        }
    }
    Agent::sharedTime++;
}

void printBlackboard(SearchClient::Blackboard* b) {
    auto posEntries = b->getPositionEntries();
    std::cerr << "\n----Blackboard---\n";
    std::cerr << "Timestep\tPosition\tAuthor\n";
    for (auto& entry : posEntries) {
        std::cerr << entry->getTimeStep() << "\t\t(" <<
                    entry->getPosition().x << "," << entry->getPosition().y <<
                    ")\t\t" << entry->getAuthorId() << std::endl;
    }
}