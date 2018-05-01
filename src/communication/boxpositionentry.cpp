#include "boxpositionentry.h"
#include "../agent/level0agent.h"

using Communication::BoxPositionEntry;

BoxPositionEntry::BoxPositionEntry(): Communication::BlackboardEntry() {}

BoxPositionEntry::BoxPositionEntry(const Communication::BoxPositionEntry &src): Communication::BlackboardEntry(src),
    boxId_(src.boxId_) {
}

BoxPositionEntry& BoxPositionEntry::operator=(const BoxPositionEntry& src) {
    Communication::BlackboardEntry::operator=(src);
    boxId_ = src.boxId_;
    return *this;
}


BoxPositionEntry* BoxPositionEntry::create(const Coord &location, unsigned int timeStep, short boxId, const Agent::Level0Agent &author, Communication::Blackboard *parent)
{
    BoxPositionEntry *entry = new BoxPositionEntry;
    entry->setLocation(location);
    entry->setTimeStep(timeStep);
    entry->setAuthorId(author.getIndex());
    entry->setBoxId(boxId);

    parent->addEntry(entry, Communication::Blackboard::BoxPositionEntry);
    return entry;
}

BoxPositionEntry* BoxPositionEntry::create(const Coord &location, unsigned int timeStep, short boxId, Communication::Blackboard *parent)
{
    BoxPositionEntry *entry = new BoxPositionEntry;
    entry->setLocation(location);
    entry->setTimeStep(timeStep);
    entry->setBoxId(boxId);

    parent->addEntry(entry, Communication::Blackboard::BoxPositionEntry);
    return entry;
}
