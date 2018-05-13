#include "blackboardentry.h"
#include "globalgoalentry.h"
#include "positionentry.h"
#include "helpentry.h"
#include "boxpositionentry.h"

using Communication::BlackboardEntry;
using Communication::Blackboard;

BlackboardEntry::BlackboardEntry(): parent_(0), position_(), authorId_(-1), timeStep_(-1) {}

BlackboardEntry::BlackboardEntry(const BlackboardEntry &src): parent_(src.parent_),
    position_(src.position_), authorId_(src.authorId_), timeStep_(src.timeStep_) { }

BlackboardEntry& BlackboardEntry::operator=(const BlackboardEntry &src) {

    parent_ = src.parent_;
    authorId_ = src.authorId_;
    position_ = src.position_;
    timeStep_ = src.timeStep_;

    return *this;
}

bool BlackboardEntry::operator==(const BlackboardEntry &compared) const {
    if(compared.getParent() != parent_ ) return false;
    else if(compared.getAuthorId() != authorId_ ) return false;
    else if(compared.getLocation() != position_ ) return false;

    return true;
}

bool BlackboardEntry::accept(BlackboardEntry *entry, const SearchClient::Agent &receiver) {
    if(entry->getParent() == nullptr)
        return true;

    // Veeeeeeeeeeery bad practise, don't reproduce at home
    if(dynamic_cast<Communication::BoxPositionEntry*>(entry) != nullptr)
        entry->getParent()->removeEntry(entry, Blackboard::BoxPositionEntry);
    else if (dynamic_cast<Communication::PositionEntry*>(entry) != nullptr) {
        entry->getParent()->removeEntry(entry, Blackboard::PositionEntry);
    }
    else if (dynamic_cast<Communication::HelpEntry*>(entry) != nullptr) {
        entry->getParent()->removeEntry(entry, Blackboard::HelpEntry);
    }
    else if(dynamic_cast<Communication::GlobalGoalEntry*>(entry) != nullptr) {
        entry->getParent()->removeEntry(entry, Blackboard::GoalEntry);
    }

    return true;
}

bool BlackboardEntry::revoke(BlackboardEntry *entry, const SearchClient::Agent &sender) {
//    if(sender.getIndex() != entry->getAuthorId()) // Only the author can revoke a demand
//        return false;

    // Veeeeeeeeeeery bad practise, don't reproduce at home
    if(dynamic_cast<Communication::BoxPositionEntry*>(entry) != nullptr)
        entry->getParent()->removeEntry(entry, Blackboard::BoxPositionEntry);
    else if (dynamic_cast<Communication::PositionEntry*>(entry) != nullptr) {
        entry->getParent()->removeEntry(entry, Blackboard::PositionEntry);
    }
    else if (dynamic_cast<Communication::HelpEntry*>(entry) != nullptr) {
        entry->getParent()->removeEntry(entry, Blackboard::HelpEntry);
    }
    else if(dynamic_cast<Communication::GlobalGoalEntry*>(entry) != nullptr) {
        entry->getParent()->removeEntry(entry, Blackboard::GoalEntry);
    }
    return true;
}

