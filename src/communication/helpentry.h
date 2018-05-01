#ifndef COMMUNICATION_HELPENTRY_H
#define COMMUNICATION_HELPENTRY_H

#include <vector>
#include "blackboardentry.h"
#include "../searchengine/typedefs.h"

namespace Communication {

class HelpEntry : public BlackboardEntry
{
public:
    enum ProblemType {
        Agent,
        Box,
        TakenCareOf,
        Done
    };

public:
    HelpEntry();
    HelpEntry(const HelpEntry& src);
    HelpEntry& operator=(const HelpEntry& src);

    unsigned int getPriority() const { return priority_; }
    void setPriority(unsigned int priority) { priority_ = priority; }

    void setForbiddenPath(const std::vector<Coord> &path) { forbiddenPath_ = path; }
    const std::vector<Coord>& getForbiddenPath() const { return forbiddenPath_; }

    void setProblemType(ProblemType pType) { pType_ = pType; }
    ProblemType getProblemType() const { return pType_; }

    int getBlockingBoxId() const { return blockingBoxId_; }
    void setBlockingBoxId(char idx) { blockingBoxId_ = idx; }

    int getBlockingAgentId() const { return blockingAgentId_; }
    void setBlockingAgentId(char idx) { blockingAgentId_ = idx; }

    static HelpEntry* create(const Coord &location, ProblemType pType, const std::vector<Coord> &forbiddenPath,
                             unsigned int timeStep, const Agent::Level0Agent *author, Blackboard* parent);

private:
    unsigned int priority_;
    std::vector<Coord> forbiddenPath_;
    ProblemType pType_;

    // Attributes for ProblemType == Agent
    int blockingAgentId_;

    // Attributes for ProblemType == Box
    int blockingBoxId_;
};

}

#endif // HELPENTRY_H
