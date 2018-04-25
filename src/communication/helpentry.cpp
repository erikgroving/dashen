#include "helpentry.h"

using Communication::HelpEntry;

HelpEntry::HelpEntry() : Communication::BlackboardEntry(), priority_(0), forbiddenPath_() {

}

HelpEntry* HelpEntry::create(const Coord &location, unsigned int timeStep, const SearchClient::Agent &author, Communication::Blackboard *parent)
{
    HelpEntry *entry = new HelpEntry;
    entry->setLocation(location);
    entry->setTimeStep(timeStep);
    entry->setAuthorId(author.getIndex());

    parent->addEntry(entry, Communication::Blackboard::HelpEntry);
    return entry;
}
