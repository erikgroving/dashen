#include "master.h"
#include "searchclient.h"
#include "../communication/communication"
#include "../heuristics/distanceoracle.h"
#include <cassert>

using Communication::Blackboard;
void printBlackboard(Communication::Blackboard* b);
using namespace SearchEngine;

/*
 * This is the primary function of the master class, that
 * conducts the main search
 */

SearchEngine::Deadend Master::deadends = SearchEngine::Deadend();

Master::Master(): jointActions_(), masterState_(), prevMasterState_(), agents_(), masterBlackboard_() {

}

Master::Master(const State &s1, const std::vector<SearchClient::Agent> &agents): jointActions_(), masterState_(s1),
    prevMasterState_(), agents_(agents), masterBlackboard_() {

    Master::deadends = SearchEngine::Deadend(s1);
    for(auto &agent: agents_)
        agent.setBlackboard(&masterBlackboard_);

}

Master::Master(const Master &m) {

    jointActions_ = m.jointActions_;
    masterState_ = m.masterState_;
    prevMasterState_ = m.prevMasterState_;
    masterBlackboard_ = m.masterBlackboard_;
    agents_ = m.agents_;

}

void Master::conductSearch() {
    std::cerr << "--- Starting conductSearch! ---\n";

    std::cerr << "--- Posting goals for the state onto the blackboard ---\n";
    postBlackBoard();
    assignBoxesToGoals();

    int round = 0;
    while (!SearchEngine::Predicate::isGoalState(&masterState_) && round < 3000) {
        std::cerr << "\n------------ ROUND " << round++ << " ------------\n\n";
        clearCompleteUntakenHelpEntries();
        SearchClient::Agent::setSharedState(&masterState_);
        SearchClient::JointAction ja = callForActions();
        jointActions_.push_back(ja);

        prevMasterState_ = masterState_;
        updateCurrentState(&ja);
        std::cerr << "Joint Action: " << ja.toActionString();
        std::cout << ja.toActionString() << std::endl;
        std::cerr << std::endl;
        printMap(&masterState_);
        printBlackboard(&masterBlackboard_);
        std::cerr<<std::endl;
    }

    std::cerr << "Sending solution. Length = " << jointActions_.size() << std::endl;
    // Send the solution
    //sendSolution();
}

/* This adds all the goal tiles from the initial state to the blackboard */
void Master::postBlackBoard() {
    for (Goal& g : masterState_.goals) {
        Communication::GlobalGoalEntry::create(g.loc, -1, 0, &masterBlackboard_);
    }
    computeGoalPriorities();
    

    masterBlackboard_.setAgentPositionEntryRegistrySize(masterState_.getAgents().size());
    masterBlackboard_.setBoxEntryRegistrySize(masterState_.getBoxes().size());

    for (auto a : masterState_.getAgents()) {
        Communication::PositionEntry::create(a.loc, -1, agents_[a.num], &masterBlackboard_);
    }
    /*
     * Create box position entries, these entries do not get removed
     * Box position entries are updated by changing the time step and
     * position values
     */
    for (Box& b : masterState_.getBoxes()) {
        Communication::BoxPositionEntry::create(b.loc, -1, b.id, &masterBlackboard_);
    }
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

void Master::assignBoxesToGoals() {
    SearchClient::Agent::setSharedState(&masterState_);
    Box closestBox;
    for (Goal& g : SearchEngine::State::goals) {
        unsigned long minDist = ULONG_MAX;
        for (const Box &b : masterState_.getBoxes()) {
            if (!State::takenBoxes[b.id] && g.letter == b.letter) {
                unsigned long dist = Heur::DistanceOracle::fetchDistFromCoord(g.loc, b.loc);
                if (dist < minDist) {
                    minDist = dist;
                    closestBox = b;
                }
            }
        }
        g.assignedBoxID = closestBox.id;
        State::takenBoxes[closestBox.id] = true;
    }
}

// Clears entries from help blackboard that are complete
// but not because an agent helped
void Master::clearCompleteUntakenHelpEntries() {
    if (masterBlackboard_.getHelpEntries().size() > 0) {

        for(auto* entry: masterBlackboard_.getHelpEntries()) {
            Communication::HelpEntry* entry_casted = (Communication::HelpEntry*) entry;
            if (entry_casted->getBlockingBoxId() == -1) {
                const int& agentId = entry_casted->getBlockingAgentId();
                if (SearchEngine::Predicate::isAgentNotOnForbiddenPath(
                        &masterState_, agentId,
                        entry_casted->getForbiddenPath())) {
                    entry_casted->setProblemType(Communication::HelpEntry::Done);
                }
            }
            else {
                const int& boxId = entry_casted->getBlockingBoxId();
                if (SearchEngine::Predicate::isBoxNotOnForbiddenPath(
                        &masterState_, boxId,
                        entry_casted->getForbiddenPath())) {
                    
                    entry_casted->setProblemType(Communication::HelpEntry::Done);
                }
            }
        }
    }
}

/* This function updates the current state to reflect the previous joint actions */
void Master::updateCurrentState(SearchClient::JointAction* ja) {
    std::vector<SearchEngine::Command> actions = ja->getData();
    std::vector< std::pair<int, SearchEngine::Command> > agentsWithPlansToBeCleared;
    for (size_t i = 0; i < actions.size(); i++) {
        /* Move must be valid in the previous state and the in process state */
        if (isActionValid(&masterState_, actions[i], i) &&
            isActionValid(&prevMasterState_, actions[i], i)) {
            updateStateWithNewMove(actions[i], i);
        }
        else {
            /* We should probably do something better */
            /* But it's kind of tricky */
            agentsWithPlansToBeCleared.push_back(std::make_pair(i, actions[i]));
            ja->setAction(i, SearchEngine::Command());
        }
    }
    revokeBlackboardEntries(*ja);
    for (size_t i = 0; i < agentsWithPlansToBeCleared.size(); i++) {
        int id = agentsWithPlansToBeCleared[i].first;
        auto cmd = agentsWithPlansToBeCleared[i].second;
        agents_[id].clearPlan(cmd);
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

    std::vector<AgentDescription> agentDescs = masterState_.getAgents();
    std::vector<Box> boxes = masterState_.getBoxes();

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
    auto goalPriorities = SearchEngine::GoalPriorityComputation::computeAllGoalPriorities(&masterState_);
    std::cerr << "goalPriorities: " << std::endl;
    for (Goal currentGoal : masterState_.goals) {
        std::cerr << currentGoal.loc.x << " " << currentGoal.loc.y;
        std::cerr << ", letter: " << currentGoal.letter;
        std::cerr << ", priority: " << currentGoal.priority << std::endl;
    }
    for (size_t i = 0; i < goalPriorities.size(); i++) {
        std::cerr << i << " value: " << goalPriorities[i] << std::endl;
    }


    for(Communication::BlackboardEntry *goalEntry: masterBlackboard_.getGoalEntries()) {
        int goalIndex = -1;
        SearchEngine::Predicate::goalAt(&masterState_, goalEntry->getLocation().x, goalEntry->getLocation().y, &goalIndex);
        static_cast<Communication::GlobalGoalEntry*>(goalEntry)->setPriority(goalPriorities[goalIndex]);
        //goalEntry->setPriority(SearchEngine::GoalPriorityComputation::computeGoalPriority(&masterState_, SearchEngine::State::goals[goalIndex]) );

        char letter;
        for (Goal g : masterState_.goals) {
            if (g.loc == goalEntry->getLocation()) {
                letter = g.letter;
                break;
            }
        }
        unsigned int currentPriority = static_cast<Communication::GlobalGoalEntry*>(goalEntry)->getPriority();
        std::cerr << "Goal with letter: " << letter <<  " has priority " << currentPriority << std::endl;
    }

    SearchEngine::StrictOrdering sO = SearchEngine::StrictOrdering();
    sO.calculateStrictOrderings(masterState_);
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
            masterBlackboard_.erase_front(Communication::Blackboard::PositionEntry, i);
        }
        if (commands[i].action() == PUSH || commands[i].action() == PULL) {
            // pop the front of the positional entry vector
            masterBlackboard_.erase_front(Communication::Blackboard::BoxPositionEntry, commands[i].targBoxId());
        }
    }

    SearchClient::Agent::sharedTime++;
}

void Master::printBlackboard(Communication::Blackboard* b) {

    auto posEntries = b->getPositionEntries();
 /*   std::cerr << "\n---------Position Blackboard--------\n";
    std::cerr << "Timestep\t\tPosition\t\tAuthor\n";

    for (size_t i = 0; i < posEntries.size(); i++) {
        std::cerr << "\n----- AGENT " << i << " -----\n";
        for (auto& entry : posEntries[i]) {
            std::cerr << (int)entry->getTimeStep() << "\t\t\t(" <<
                        entry->getLocation().x << "," << entry->getLocation().y <<
                        ")\t\t\t" << entry->getAuthorId() << std::endl;
        }
    }
    */
    std::cerr << "\n---------Box Blackboard--------\n";
    std::cerr << "Timestep\t\tPosition\t\tBox\t\tLetter\n";
    for (size_t boxID = 0; boxID < masterState_.getBoxes().size(); boxID++) {
        auto boxEntries = b->getBoxEntriesByID(boxID);
        for (auto* entry : boxEntries) {
            Communication::BoxPositionEntry *entry_casted = static_cast<Communication::BoxPositionEntry*>(entry);
            std::cerr << (int)entry_casted->getTimeStep() << "\t\t\t(" <<
                        entry_casted->getLocation().x << "," << entry_casted->getLocation().y <<
                        ")\t\t\t" << entry_casted->getBoxId() << "\t\t" << masterState_.getBoxes()[entry_casted->getBoxId()].letter << std::endl;
        }
    }

    std::cerr << "\n---------Help Blackboard--------\n";
    std::cerr << "Timestep\t\tRequestor\t\tType\t\tBlocking ID\t\tTaken?\n";
    auto helpEntries = b->getHelpEntries();
    for (auto* entry : helpEntries) {
        Communication::HelpEntry *entry_casted = static_cast<Communication::HelpEntry*>(entry);
        if (entry_casted->getBlockingBoxId() == -1) {
            std::cerr << (int)entry_casted->getTimeStep() << "\t\t\t" <<
                        entry_casted->getAuthorId() << "\t\t\t" << "AGENT" <<
                        "\t\t" << entry_casted->getBlockingAgentId() << "\t\t\t" <<  
                        (entry_casted->getProblemType() == Communication::HelpEntry::TakenCareOf ? "Yes" : "No") << std::endl;
        }
        else if (entry_casted->getBlockingAgentId() == -1) {
            std::cerr << (int)entry_casted->getTimeStep() << "\t\t\t" <<
                        entry_casted->getAuthorId() << "\t\t\t" << "BOX" <<
                        "\t\t" << entry_casted->getBlockingBoxId() << "\t\t\t" << 
                        (entry_casted->getProblemType() == Communication::HelpEntry::TakenCareOf ? "Yes" : "No") << std::endl;
        }
    }
}
