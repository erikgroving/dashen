#ifndef COMMUNICATION_BOXPOSITIONENTRY_H
#define COMMUNICATION_BOXPOSITIONENTRY_H

#include "../searchengine/typedefs.h"
#include "../searchclient/agent.h"

#include "blackboardentry.h"

namespace Communication {

class BoxPositionEntry : public BlackboardEntry
{
public:
    BoxPositionEntry();
    BoxPositionEntry(const BoxPositionEntry& src);
    BoxPositionEntry& operator=(const BoxPositionEntry& src);

    short getBoxId() const { return boxId_; }
    void setBoxId(short boxId) { boxId_ = boxId; }

    static BoxPositionEntry* create(const Coord &location, unsigned int timeStep, short boxId, const SearchClient::Agent &author, Blackboard* parent);
    static BoxPositionEntry* create(const Coord &location, unsigned int timeStep, short boxId, Communication::Blackboard *parent);
private:
    short boxId_;
};

}

#endif // BOXPOSITIONENTRY_H
