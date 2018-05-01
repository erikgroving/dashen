#include "globalgoalentry.h"
#include "../agent/searchagent.h"

using Communication::GlobalGoalEntry;

GlobalGoalEntry::GlobalGoalEntry(): Communication::BlackboardEntry(), priority_(0)
{

}

GlobalGoalEntry::GlobalGoalEntry(const Communication::GlobalGoalEntry &src): Communication::BlackboardEntry(src),
    priority_(src.priority_) {

}

GlobalGoalEntry& GlobalGoalEntry::operator=(const Communication::GlobalGoalEntry &src) {
    Communication::BlackboardEntry::operator=(src);
    priority_ = src.priority_;
    return *this;
}

GlobalGoalEntry* GlobalGoalEntry::create(const Coord &location, unsigned int timeStep, unsigned int priority, const Agent::SearchAgent *author, Communication::Blackboard *parent)
{
    GlobalGoalEntry *entry = new GlobalGoalEntry;
    entry->setLocation(location);
    entry->setTimeStep(timeStep);
    entry->setAuthorId(author->getIndex());
    entry->setPriority(priority);

    parent->addEntry(entry, Communication::Blackboard::GoalEntry);
    return entry;
}

GlobalGoalEntry* GlobalGoalEntry::create(const Coord &location, unsigned int timeStep, unsigned int priority, Communication::Blackboard *parent)
{
    GlobalGoalEntry *entry = new GlobalGoalEntry;
    entry->setLocation(location);
    entry->setTimeStep(timeStep);
    entry->setPriority(priority);

    parent->addEntry(entry, Communication::Blackboard::GoalEntry);

    return entry;
}
