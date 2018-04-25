#ifndef COMMUNICATION_HELPENTRY_H
#define COMMUNICATION_HELPENTRY_H

#include <vector>
#include "blackboardentry.h"
#include "../searchengine/typedefs.h"
#include "../searchclient/searchclient"

namespace Communication {

class HelpEntry : public BlackboardEntry
{
public:
    HelpEntry();
    HelpEntry(const HelpEntry& src);
    HelpEntry& operator=(const HelpEntry& src);

    unsigned int getPriority() const { return priority_; }
    void setPriority(unsigned int priority) { priority_ = priority; }

    void setForbiddenPath(const std::vector<Coord> &path) { forbiddenPath_ = path; }
    const std::vector<Coord>& getForbiddenPath() const { return forbiddenPath_; }

    static HelpEntry* create(const Coord &location, unsigned int timeStep, const SearchClient::Agent &author, Blackboard* parent);
private:
    unsigned int priority_;
    std::vector<Coord> forbiddenPath_;
};

}

#endif // HELPENTRY_H
