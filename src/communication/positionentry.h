#ifndef COMMUNICATION_POSITIONENTRY_H
#define COMMUNICATION_POSITIONENTRY_H

#include "../searchengine/typedefs.h"
#include "blackboardentry.h"

namespace Communication {

class PositionEntry: public BlackboardEntry
{
public:
    PositionEntry();
    PositionEntry(const PositionEntry &src);
    PositionEntry& operator=(const PositionEntry &src);

    static PositionEntry* create(const Coord &location, unsigned int timeStep, const Agent::Level0Agent &author, Blackboard* parent);

};

}
#endif // POSITIONENTRY_H
