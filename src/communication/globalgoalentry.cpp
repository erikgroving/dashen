#include "globalgoalentry.h"

using Communication::GlobalGoalEntry;

GlobalGoalEntry::GlobalGoalEntry(): Communication::BlackboardEntry(), priority_(0)
{

}

GlobalGoalEntry* GlobalGoalEntry::create(const Coord &location, unsigned int timeStep, unsigned int priority, const SearchClient::Agent &author, Communication::Blackboard *parent)
{
    GlobalGoalEntry *entry = new GlobalGoalEntry;
    entry->setLocation(location);
    entry->setTimeStep(timeStep);
    entry->setAuthorId(author.getIndex());
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
