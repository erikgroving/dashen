#include "blackboard.h"
#include "blackboardentry.h"

#include "../communication/boxpositionentry.h"
#include "../communication/globalgoalentry.h"
#include "../communication/positionentry.h"
#include "../communication/helpentry.h"

using Communication::Blackboard;
using Communication::BlackboardEntry;

Blackboard::Blackboard(): positionEntries_(), boxPositionEntries_(), goalEntries_(), helpEntries_() {

}

Blackboard::~Blackboard() {
    for(BlackboardEntry* entry: positionEntries_)
            delete entry;

    for(BlackboardEntry* entry: goalEntries_)
            delete entry;

    for(auto &vector: boxPositionEntries_)
        for(BlackboardEntry* entry: vector)
            delete entry;

    for(BlackboardEntry* entry: helpEntries_)
        delete entry;
}

void Blackboard::addEntry(BlackboardEntry *entry, Registry registry) {

    switch(registry) {
        case PositionEntry:
            entry->setBlackboardParent(this);
            positionEntries_.push_back(entry);
            break;
        case BoxPositionEntry:
            entry->setBlackboardParent(this);
            // Bad practise but who cares :p ???
            boxPositionEntries_[static_cast<Communication::BoxPositionEntry*>(entry)->getBoxId()].push_back(entry);
            break;
        case GoalEntry:
            entry->setBlackboardParent(this);
            goalEntries_.push_back(entry);
            break;
        case HelpEntry:
            entry->setBlackboardParent(this);
            helpEntries_.push_back(entry);
            break;
    }
}

void Blackboard::setBoxEntryRegistrySize(size_t numBoxes) {
    boxPositionEntries_.resize(numBoxes);
}

void Blackboard::removeEntry(const BlackboardEntry *entry, Registry registry) {

    switch(registry) {
        case BoxPositionEntry:
            std::cerr << "Remove entry not supported for registry of type BoxPositionEntry" << std::endl;
            break;
        case PositionEntry:
            positionEntries_.erase(
                std::remove(positionEntries_.begin(), positionEntries_.end(), entry)
            );
            delete entry;
            break;
        case GoalEntry:
            goalEntries_.erase(
                std::remove(goalEntries_.begin(), goalEntries_.end(), entry)
            );
            delete entry;
            break;
        case HelpEntry:
            helpEntries_.erase(
                std::remove(helpEntries_.begin(), helpEntries_.end(), entry)
            );
            delete entry;
            break;
        default:
            std::cerr << "Tried to remove an invalid entry" << std::endl;
            break;
    }
}

void Blackboard::removeEntriesByAuthor(char agentID, Registry registry) {
    std::vector<BlackboardEntry*> *container =nullptr;
    switch(registry) {
        case HelpEntry:
            container = &helpEntries_;
            break;
        case PositionEntry:
            container = &positionEntries_;
            break;
        case GoalEntry:
            container = &goalEntries_;
            break;
        case BoxPositionEntry:
            std::cerr << "Cannot use removeEntryiesByAuthor for registry BoxPositionEntry" << std::endl;
            return;
    }

    for (size_t i = 0; i < container->size(); i++) {
        if ((*container)[i]->getAuthorId() == agentID) {

            delete (*container)[i];
            (*container)[i] = (*container).back();
            (*container).pop_back();
            i -= 1;
        }
    }
}

void Blackboard::erase_front(Registry registry, short id) {
    Communication::BlackboardEntry *entry = nullptr;

    switch(registry) {
        case HelpEntry:
            if(helpEntries_.size() > 0) {
                entry = *helpEntries_.begin();
                helpEntries_.erase(helpEntries_.begin());
                delete entry;
            }
            break;

        case PositionEntry:
            if(positionEntries_.size() > 0) {
                entry = *positionEntries_.begin();
                positionEntries_.erase(positionEntries_.begin());
                delete entry;
            }
            break;

        case GoalEntry:
            if(goalEntries_.size() > 0) {
                entry = *goalEntries_.begin();
                goalEntries_.erase(goalEntries_.begin());
                delete entry;
            }
            break;

        case BoxPositionEntry:
            if(id >= 0 && boxPositionEntries_[id].size() > 0) {
                entry = *boxPositionEntries_[id].begin();
                boxPositionEntries_[id].erase(boxPositionEntries_[id].begin());
                delete entry;
            }
            break;
    }
}

void Blackboard::clear(Registry registry, short id) {
    switch(registry) {
        case HelpEntry:
            for(Communication::BlackboardEntry *entry: helpEntries_)
                delete entry;
            helpEntries_.resize(0);
            break;

        case PositionEntry:
            for(Communication::BlackboardEntry *entry: positionEntries_)
                delete entry;
            positionEntries_.resize(0);
            break;

        case GoalEntry:
            for(Communication::BlackboardEntry *entry: goalEntries_)
                delete entry;
            goalEntries_.resize(0);
            break;

        case BoxPositionEntry:
            if(id >= 0) {
                for(Communication::BlackboardEntry *entry: boxPositionEntries_[id])
                    delete entry;
                boxPositionEntries_[id].resize(0);
            }
            break;
    }
}

BlackboardEntry* Blackboard::findPositionEntry(unsigned int timeStep, int authorId) {
    return *std::find_if(positionEntries_.begin(), positionEntries_.end(), [timeStep, authorId](const BlackboardEntry *entry){
        if(entry->getTimeStep() == timeStep && entry->getAuthorId() == authorId)
            return true;
        return false;
    });
}
