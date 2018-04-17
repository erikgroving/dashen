#include "blackboardentry.h"

using SearchClient::BlackboardEntry;
using SearchClient::Blackboard;

BlackboardEntry::BlackboardEntry(): parent_(0), type_(BlackboardEntry::NONE), position_(), authorId_(-1), timeStep_(-1) {}

BlackboardEntry::BlackboardEntry(const BlackboardEntry &src): parent_(src.parent_), type_(src.type_), 
    position_(src.position_), authorId_(src.authorId_), timeStep_(src.timeStep_) { }

BlackboardEntry& BlackboardEntry::operator=(const BlackboardEntry &src) {
    parent_ = src.parent_;
    type_ = src.type_;
    authorId_ = src.authorId_;
    position_ = src.position_;
    timeStep_ = src.timeStep_;

    return *this;
}

bool BlackboardEntry::operator==(const BlackboardEntry &compared) const {
    if(compared.getParent() != parent_ ) return false;
    else if(compared.getAuthorId() != authorId_ ) return false;
    else if(compared.getType() != type_ ) return false;
    else if(compared.getPosition() != position_ ) return false;

    return true;
}

bool BlackboardEntry::accept(BlackboardEntry *entry, const SearchClient::Agent &receiver) {
    if(entry->getParent() != nullptr)
        entry->getParent()->removeEntry(entry);
    return true;
}

bool BlackboardEntry::revoke(BlackboardEntry *entry, const SearchClient::Agent &sender) {
    std::cerr << entry;
    if(sender.getIndex() != entry->getAuthorId()) // Only the author can erevoke a demand
        return false;

    entry->getParent()->removeEntry(entry);
    return true;
}

BlackboardEntry* BlackboardEntry::create(BlackboardEntry::EntryType type, const SearchClient::Agent &author, Blackboard *parent,  unsigned int timeStep) {
    BlackboardEntry *newEntry = new BlackboardEntry();

    newEntry->setType(type);
    newEntry->setAuthorId(author.getIndex());
    newEntry->setTimeStep(timeStep);

    std::cerr << "Creating entry with author id " << newEntry->getAuthorId() << std::endl;

    parent->addEntry(newEntry);

    
    return newEntry;
}

BlackboardEntry* BlackboardEntry::create(BlackboardEntry::EntryType type, Blackboard *parent, unsigned int timeStep) {
    BlackboardEntry *newEntry = new BlackboardEntry();

    newEntry->setType(type);
    newEntry->setTimeStep(timeStep);

    parent->addEntry(newEntry);
    return newEntry;
}

