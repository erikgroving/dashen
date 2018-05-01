#include "positionentry.h"
#include "../agent/level0agent.h"

using Communication::PositionEntry;

PositionEntry::PositionEntry(): Communication::BlackboardEntry() {}

PositionEntry::PositionEntry(const Communication::PositionEntry &src): Communication::BlackboardEntry(src) {}

Communication::PositionEntry &PositionEntry::operator=(const Communication::PositionEntry &src) {
    Communication::BlackboardEntry::operator=(src);
    return *this;
}

PositionEntry* PositionEntry::create(const Coord &location, unsigned int timeStep, const Agent::Level0Agent &author, Communication::Blackboard *parent)
{
    PositionEntry *entry = new PositionEntry;
    entry->setAuthorId(author.getIndex());
    entry->setLocation(location);
    entry->setTimeStep(timeStep);

    parent->addEntry(entry, Communication::Blackboard::PositionEntry);
    return entry;
}
