#include "level2agent.h"
#include "../heuristics/distanceoracle.h"
#include <string>
#include <sstream>

using Agent::Level1Agent;
using Agent::Level2Agent;
using namespace SearchEngine::Predicate;

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

Level2Agent::Level2Agent(Color color, char num, Communication::Blackboard *blackboard): Level1Agent(color, num, blackboard)
{

}

bool Level2Agent::getNextGoal(bool *isHelpGoal) {

    if(!currentHelpGoal_.over) {
        *isHelpGoal = true;
        return true;
    }

    if(getHelpGoal()) {
        *isHelpGoal = true;
        return true;
    }

    // See if we have any unsatisfied goals
    bool satisfied = true;
    Goal unsatGoal = Goal();

    for (const Goal& g : takenGoals_) {
        if (!goalHasCorrectBox(sharedState, g)) {
            satisfied = false;
            unsatGoal = g;
            break;
        }
    }

    if (satisfied) {
        return getGlobalGoal();
    }
    else {
        currentSearchGoal_ = unsatGoal;
        return true;
    }
}

bool Level2Agent::getGlobalGoal()
{
    // If the blackboarg is empty, no goal can be retrieved
    if(getBlackboard()->getGoalEntries().size() <= 0) return false;

    Communication::GlobalGoalEntry *selectedEntry = nullptr;
    unsigned int priority = 0;

    int closestBoxIndex = -1;
    // Select the doable entry with the highest priority and find its associated box
    for (Communication::BlackboardEntry *entry : getBlackboard()->getGoalEntries()) {

        Communication::GlobalGoalEntry *entry_casted = (Communication::GlobalGoalEntry*) entry;
        if( priority < entry_casted ->getPriority()                      &&
            isEntryDoable(entry_casted , sharedState, &closestBoxIndex) ) {
            selectedEntry = entry_casted ;
            priority = entry_casted ->getPriority();
        }
    }

    // If no entry is doable, return an empty goal
    if(selectedEntry == nullptr)
        return false;


    /* Find the goal for that position */
    int goalIndex;
        goalAt(sharedState, selectedEntry->getLocation().x, selectedEntry->getLocation().y, &goalIndex);

    Goal &result = SearchEngine::State::goals[goalIndex];

    SearchEngine::State::takenBoxes[closestBoxIndex] = true;
    result.assignedBoxID = closestBoxIndex;
    takenGoals_.push_back(result);

    Communication::BlackboardEntry::accept(selectedEntry, this);

    currentSearchGoal_ = result;
    return true;
}

bool Level2Agent::getHelpGoal()
{
    for(Communication::BlackboardEntry* entry: getBlackboard()->getHelpEntries()) {
        Communication::HelpEntry* entry_casted = (Communication::HelpEntry*) entry;

        switch(entry_casted->getProblemType()) {

        case Communication::HelpEntry::Agent:
            if(entry_casted->getBlockingAgentId() != getIndex()) {

                entry_casted->setProblemType(Communication::HelpEntry::TakenCareOf);
                currentHelpEntry_ = entry_casted;
                currentHelpGoal_ = Agent::HelpGoal(
                    Agent::HelpGoal::Agent,
                    entry_casted->getBlockingAgentId(),
                    entry_casted->getLocation(),
                    entry_casted->getForbiddenPath(),
                    false
                );
                return true;
            }
        break;

        case Communication::HelpEntry::Box:
        {
            Box &box = sharedState->getBoxes()[entry_casted->getBlockingBoxId()];
            if(box.color == getColor()) {

                entry_casted->setProblemType(Communication::HelpEntry::TakenCareOf);
                currentHelpEntry_ = entry_casted;
                currentHelpGoal_ = Agent::HelpGoal(
                    Agent::HelpGoal::Box,
                    entry_casted->getBlockingBoxId(),
                    entry_casted->getLocation(),
                    entry_casted->getForbiddenPath(), false
                );
                return true;
            }
        }
        break;
        default:
        break;
        }
    }

    return false;
}

bool Level2Agent::waitingForHelp()
{
    bool result = false;

    for(auto ite = helpEntriesToMonitor_.begin(); ite != helpEntriesToMonitor_.end();){
        Communication::HelpEntry* entry = (Communication::HelpEntry*) *ite;
        if(entry->getProblemType() != Communication::HelpEntry::Done) {
            ite++;
            result = true;
        }
        else {
            Communication::BlackboardEntry::revoke(entry, this);
            ite = helpEntriesToMonitor_.erase(ite);
        }
    }

    return result;
}

bool Level2Agent::isEntryDoable(const Communication::BlackboardEntry *entry, const SearchEngine::State *state, int *boxIndex)
{
    Box closestBox = Box();
    int goalIndex = -1;
        goalAt(state, entry->getLocation().x, entry->getLocation().y, &goalIndex);
    Goal entryGoal = SearchEngine::State::goals[goalIndex];

    if (Heur::DistanceOracle::fetchDistFromCoord(entryGoal.loc, state->getAgents()[getIndex()].loc) == (unsigned long)-1) {
        return false;
    }

    unsigned long minDist = ULONG_MAX;
    for (const Box &b : state->getBoxes()) {

        if (b.color == getColor() && b.letter == entryGoal.letter &&
                !SearchEngine::State::takenBoxes[b.id]) {
            //unsigned long dist = Coord::distance(b.loc, entryGoal.loc);
            unsigned long dist = Heur::DistanceOracle::fetchDistFromCoord(entryGoal.loc, b.loc);
            if (dist < minDist) {
                minDist = dist;
                closestBox = b;
            }
        }

    }

    if (closestBox.id == -1) {
        return false;
    }

    if(boxIndex != nullptr) {
        *boxIndex = closestBox.id;
    }

    return true;
}

void Level2Agent::computeCorrectGoals()
{
    SearchEngine::Predicate::getCorrectGoals(sharedState);

}

void Level2Agent::askForHelp(const std::vector<SearchEngine::State *> &path)
{
    std::vector<Coord> forbiddenCoords;
    for(const SearchEngine::State* state: path) {
        forbiddenCoords.push_back(state->getAgents()[getIndex()].loc);
    }

    for(const SearchEngine::State* state: path) {
        Coord agentLoc = state->getAgents()[getIndex()].loc;
        std::string errorDescription;
        if(positionFree(agentLoc.x, agentLoc.y, state->getAction(), -1, state->getTimeStep(), errorDescription))
            continue;

        auto error = split(errorDescription, '.');
        if(error[0] ==  "AGENT") {
            auto entry = Communication::HelpEntry::create(agentLoc, Communication::HelpEntry::Agent, forbiddenCoords, sharedTime, this, getBlackboard());
            entry->setBlockingAgentId(std::stoi(error[1]));
            helpEntriesToMonitor_.push_back(entry);
        }
        else if(error[0] == "BOX") {
            auto entry = Communication::HelpEntry::create(agentLoc, Communication::HelpEntry::Box, forbiddenCoords, sharedTime, this, getBlackboard());
            entry->setBlockingBoxId(std::stoi(error[1]));
            helpEntriesToMonitor_.push_back(entry);
        }
    }
}
