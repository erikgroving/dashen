#include "blackboard.h"
#include "blackboardentry.h"

using SearchClient::Blackboard;
using SearchClient::BlackboardEntry;

Blackboard::Blackboard(): positionEntries_(), boxPositionEntries_(), goalEntries_() { }

Blackboard::~Blackboard() {
    for(BlackboardEntry* entry: positionEntries_)
            delete entry;

    for(BlackboardEntry* entry: goalEntries_)
            delete entry;
}

void Blackboard::addEntry(BlackboardEntry *entry) {

    switch(entry->getType()) {
        case BlackboardEntry::POSITION_ENTRY:
            entry->setBlackboardParent(this);
            positionEntries_.push_back(entry);
            break;
        case BlackboardEntry::BOX_POSITION_ENTRY:
            entry->setBlackboardParent(this);
            boxPositionEntries_[entry->getAuthorId()].push_back(entry);
            break;
        case BlackboardEntry::GLOBAL_GOAL_ENTRY:
        case BlackboardEntry::MOVE_HELP_GOAL_ENTRY:
            entry->setBlackboardParent(this);
            goalEntries_.push_back(entry);
            break;

        default:
            std::cerr << "An invalid entry was tried to be put in the blackboard" << std::endl;
            return;
    }
}

void Blackboard::initBoxEntries(size_t numBoxes) {
    for (size_t i = 0; i < numBoxes; i++) {
        boxPositionEntries_.push_back(std::vector<BlackboardEntry*>());
    }
}

void Blackboard::popBoxPosEntry(short ID) {
    boxPositionEntries_[ID].erase(boxPositionEntries_[ID].begin(), boxPositionEntries_[ID].begin() + 1);
}

void Blackboard::clearBoxEntries(short ID) {
    boxPositionEntries_[ID].erase(boxPositionEntries_[ID].begin() + 1, boxPositionEntries_[ID].end());
}

void Blackboard::removeEntry(const BlackboardEntry *entry) {

    switch(entry->getType()) {
        case BlackboardEntry::POSITION_ENTRY:
            positionEntries_.erase(
                std::remove(positionEntries_.begin(), positionEntries_.end(), entry)
            );
            delete entry;
            break;
        case BlackboardEntry::GLOBAL_GOAL_ENTRY:
        case BlackboardEntry::MOVE_HELP_GOAL_ENTRY:
            goalEntries_.erase(
                std::remove(goalEntries_.begin(), goalEntries_.end(), entry)
            );
            delete entry;
            break;
        default:
            std::cerr << "Tried to remove an invalid entry" << std::endl;
            break;
    }
}

void Blackboard::removeEntriesByAuthor(char agentID) {
    for (size_t i = 0; i < positionEntries_.size(); i++) {
        if (positionEntries_[i]->getAuthorId() == agentID) {
            positionEntries_[i] = positionEntries_.back();
            i -= 1;
            positionEntries_.pop_back(); 
        }
    }
}


BlackboardEntry* Blackboard::findPositionEntry(unsigned int timeStep, int authorId) {
    return *std::find_if(positionEntries_.begin(), positionEntries_.end(), [timeStep, authorId](const BlackboardEntry *entry){
        if(entry->getTimeStep() == timeStep && entry->getAuthorId() == authorId)
            return true;
        return false;
    });
}
