#include "positionentry.h"

using Communication::PositionEntry;

PositionEntry* PositionEntry::create(const Coord &location, unsigned int timeStep, const SearchClient::Agent &author, Communication::Blackboard *parent)
{
    PositionEntry *entry = new PositionEntry;
    entry->setAuthorId(author.getIndex());
    entry->setLocation(location);
    entry->setTimeStep(timeStep);

    parent->addEntry(entry, Communication::Blackboard::PositionEntry);
    return entry;
}
