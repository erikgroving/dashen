#ifndef COMMUNICATION_GLOBALGOALENTRY_H
#define COMMUNICATION_GLOBALGOALENTRY_H

#include "blackboardentry.h"
#include "../searchclient/searchclient"

namespace Communication {

class GlobalGoalEntry : public BlackboardEntry
{
public:
    GlobalGoalEntry();
    GlobalGoalEntry(const GlobalGoalEntry& src);
    GlobalGoalEntry& operator=(const GlobalGoalEntry &src);

    unsigned int getPriority() const { return priority_; }
    void setPriority(unsigned int priority) { priority_ = priority; }

    static GlobalGoalEntry* create(const Coord &location, unsigned int timeStep, unsigned int priority, const SearchClient::Agent &author, Blackboard* parent);
    static GlobalGoalEntry* create(const Coord &location, unsigned int timeStep, unsigned int priority, Blackboard* parent);

private:
    unsigned int priority_;
};

}

#endif // GLOBALGOALENTRY_H
