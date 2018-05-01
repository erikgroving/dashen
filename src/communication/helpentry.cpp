#include "helpentry.h"
#include "../agent/level0agent.h"

using Communication::HelpEntry;

HelpEntry::HelpEntry() : Communication::BlackboardEntry(), priority_(0), forbiddenPath_(),
    blockingAgentId_(-1), blockingBoxId_(-1) {

}

HelpEntry::HelpEntry(const Communication::HelpEntry &src): Communication::BlackboardEntry(src),
    priority_(src.priority_), forbiddenPath_(src.forbiddenPath_), blockingAgentId_(-1), blockingBoxId_(-1) {
}

HelpEntry& HelpEntry::operator=(const Communication::HelpEntry &src)
{
    Communication::BlackboardEntry::operator=(src);
    priority_ = src.priority_;
    forbiddenPath_ = src.forbiddenPath_;
    return *this;
}

HelpEntry* HelpEntry::create(const Coord &location, ProblemType pType, const std::vector<Coord> &forbiddenPath, unsigned int timeStep, const Agent::Level0Agent *author, Communication::Blackboard *parent)
{
    HelpEntry *entry = new HelpEntry;
    entry->setLocation(location);
    entry->setTimeStep(timeStep);
    entry->setAuthorId(author->getIndex());
    entry->setProblemType(pType);
    entry->setForbiddenPath(forbiddenPath);

    parent->addEntry(entry, Communication::Blackboard::HelpEntry);
    return entry;
}
