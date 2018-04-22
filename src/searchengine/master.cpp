#include "master.h"
#include "searchclient.h"

using SearchClient::Blackboard;
using SearchClient::BlackboardEntry;
void printBlackboard(SearchClient::Blackboard* b);
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
    while (!SearchEngine::Predicate::isGoalState(&masterState_) && round < 3000) {
        std::cerr << "\n------------ ROUND " << round++ << " ------------\n\n";
        SearchClient::Agent::setSharedState(&masterState_);
        SearchClient::JointAction ja = callForActions();
        revokeBlackboardEntries(ja);
        jointActions_.push_back(ja); 

        /** TODO When updating the internal state, if a command from
         * one of the agents was found to be invalid due to the action of another
         * agent, send a signal to the agent to recompute his plan
         */
        prevMasterState_ = masterState_;
        updateCurrentState(&ja);
        std::cerr << "Joint Action: " << ja.toActionString(); 
        std::cout << ja.toActionString() << std::endl;
        std::cerr << std::endl;
        printMap(&masterState_);
        printBlackboard(&masterBlackboard_);
        std::cerr<<std::endl;
        std::string x;
        for (int i = 0; i < agents_.size(); i++) {
            std::cin >> x;
            if (x[0] == 'f') {
                std::cerr << "Agent " << i << " had a wrong move.\n";
                exit(0);
            }
        }
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
    computeGoalPriorities();
}

/* This function calls for actions from all the agents */
SearchClient::JointAction Master::callForActions() {
    SearchClient::JointAction action = SearchClient::JointAction();
    action.initialize(agents_.size());
    for (size_t i = 0; i < agents_.size(); i++) {
        action.setAction(i, agents_[i].nextMove());
    } 

    return action;
}

/* This function updates the current state to reflect the previous joint actions */
void Master::updateCurrentState(SearchClient::JointAction* ja) {
    std::vector<SearchEngine::Command> actions = ja->getData();
    for (size_t i = 0; i < actions.size(); i++) {

        /* Move must be valid in the previous state and the in process state */
        if (isActionValid(&masterState_, actions[i], i) &&
            isActionValid(&prevMasterState_, actions[i], i)) {
            updateStateWithNewMove(actions[i], i);
        } 
        else {
            std::cerr << "Detected a wrong move for agent " << i << std::endl;
            ja->setAction(i, Command());
            agents_[i].clearPlan();
        }
    }
}

bool Master::isActionValid(SearchEngine::State* state, SearchEngine::Command cmd, char AgentID) {
    int newAgentRow = state->getAgents()[AgentID].loc.y + Command::rowToInt(cmd.d1());
    int newAgentCol = state->getAgents()[AgentID].loc.x + Command::colToInt(cmd.d1());

    if (cmd.action() == Action::MOVE) {
        return SearchEngine::Predicate::isFree(state, newAgentCol, newAgentRow);
    }
    else if (cmd.action() == Action::PUSH) {
        int newBoxRow = newAgentRow + Command::rowToInt(cmd.d2());
        int newBoxCol = newAgentCol + Command::colToInt(cmd.d2());
        return (SearchEngine::Predicate::isFree(state, newBoxCol, newBoxRow) &&
                SearchEngine::Predicate::boxAt(state, newAgentCol, newAgentRow));
    }
    else if (cmd.action() == Action::PULL) {
        int boxRow = state->getAgents()[AgentID].loc.y + Command::rowToInt(cmd.d2());
        int boxCol = state->getAgents()[AgentID].loc.x + Command::colToInt(cmd.d2());
        return (SearchEngine::Predicate::isFree(state, newAgentCol, newAgentRow) &&
                SearchEngine::Predicate::boxAt(state, boxCol, boxRow));
    }
    return true; //NoOp
}

void Master::updateStateWithNewMove(SearchEngine::Command cmd, char AgentID) {    
    int newAgentRow = masterState_.getAgents()[AgentID].loc.y + Command::rowToInt(cmd.d1());
    int newAgentCol = masterState_.getAgents()[AgentID].loc.x + Command::colToInt(cmd.d1());

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

void Master::computeGoalPriorities()
{
    for(BlackboardEntry *goalEntry: masterBlackboard_.getGoalEntries()) {
        int goalIndex = -1;
            SearchEngine::Predicate::goalAt(&masterState_, goalEntry->getLocation().x, goalEntry->getLocation().y, &goalIndex);
        goalEntry->setPriority(SearchEngine::GoalPriorityComputation::computeGoalPriority(&masterState_, SearchEngine::State::goals[goalIndex]) );
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
            if (agents_[i].isFirstMoveInPlan()) {
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
                    entry->getLocation().x << "," << entry->getLocation().y <<
                    ")\t\t" << entry->getAuthorId() << std::endl;
    }
}
