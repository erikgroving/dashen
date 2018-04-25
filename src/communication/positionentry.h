#ifndef COMMUNICATION_POSITIONENTRY_H
#define COMMUNICATION_POSITIONENTRY_H

#include "../searchclient/searchclient"

#include "../searchengine/typedefs.h"
#include "blackboardentry.h"

namespace Communication {

class PositionEntry: public BlackboardEntry
{
public:
    PositionEntry(): Communication::BlackboardEntry() {}
    PositionEntry(const PositionEntry &src): Communication::BlackboardEntry(src) {}
    PositionEntry& operator=(const PositionEntry &src) {
        Communication::BlackboardEntry::operator=(src);
        return *this;
    }

    static PositionEntry* create(const Coord &location, unsigned int timeStep, const SearchClient::Agent &author, Blackboard* parent);

};

}
#endif // POSITIONENTRY_H
