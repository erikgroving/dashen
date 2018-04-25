#include "boxpositionentry.h"

using Communication::BoxPositionEntry;

BoxPositionEntry* BoxPositionEntry::create(const Coord &location, unsigned int timeStep, short boxId, const SearchClient::Agent &author, Communication::Blackboard *parent)
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
