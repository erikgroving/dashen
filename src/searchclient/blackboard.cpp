#include "blackboard.h"

using SearchClient::Blackboard;

Blackboard::Entry::Entry(): parent_(0), type_(Entry::NONE), position_(), authorId_(-1) {}

Blackboard::Entry::Entry(const Entry &src): parent_(src.parent_), type_(src.type_), 
    position_(src.position_), authorId_(src.authorId_) { }

Blackboard::Entry& Blackboard::Entry::operator=(const Entry &src) {
    parent_ = src.parent_;
    type_ = src.type_;
    authorId_ = src.authorId_;
    position_ = src.position_;

    return *this;
}

bool Blackboard::Entry::operator==(const Blackboard::Entry &compared) const {
    if(compared.getParent() != parent_ ) return false;
    else if(compared.getAuthorId() != authorId_ ) return false;
    else if(compared.getType() != type_ ) return false;
    else if(compared.getPosition() != position_ ) return false;

    return true;
}

bool Blackboard::Entry::accept(Blackboard::Entry &entry, const SearchClient::Agent &receiver) {
    if(entry.getParent() != nullptr)
        entry.getParent()->removeEntry(entry);
    return true;
}

bool Blackboard::Entry::revoke(Blackboard::Entry &entry, const SearchClient::Agent &sender) {
    if(sender.getIndex() != entry.getAuthorId()) // Only the author can erevoke a demand
        return false;

    entry.getParent()->removeEntry(entry);
    return true;
}

Blackboard::Entry Blackboard::Entry::create(Blackboard::Entry::EntryType type, const SearchClient::Agent &author) {
    Blackboard::Entry newEntry;

    newEntry.setType(type);
    newEntry.setAuthorId(author.getIndex());

    return newEntry;
}

Blackboard::Entry Blackboard::Entry::create(Blackboard::Entry::EntryType type) {
    Blackboard::Entry newEntry;

    newEntry.setType(type);
    return newEntry;
}

Blackboard::Blackboard(): positionEntries_(), goalEntries_() { }

void Blackboard::addEntry(Blackboard::Entry &entry) {

    switch(entry.getType()) {
        case Blackboard::Entry::POSITION_ENTRY:
            entry.setBlackboardParent(this);
            positionEntries_.push_back(entry);
            break;
        case Blackboard::Entry::GLOBAL_GOAL_ENTRY:
        case Blackboard::Entry::MOVE_HELP_GOAL_ENTRY:
            entry.setBlackboardParent(this);
            goalEntries_.push_back(entry);
            break;

        default:
            std::cerr << "An invalid entry was tried to be put in the blackboard" << std::endl;
            return;
    }
}

void Blackboard::removeEntry(const Blackboard::Entry &entry) {

    switch(entry.getType()) {
        case Blackboard::Entry::POSITION_ENTRY:
            positionEntries_.erase(
                std::remove(positionEntries_.begin(), positionEntries_.end(), entry)
            );
            break;
        case Blackboard::Entry::GLOBAL_GOAL_ENTRY:
        case Blackboard::Entry::MOVE_HELP_GOAL_ENTRY:
            goalEntries_.erase(
                std::remove(goalEntries_.begin(), goalEntries_.end(), entry)
            );
            break;
        default:
            std::cerr << "Tried to remove an invalid entry" << std::endl;
            break;
    }
}