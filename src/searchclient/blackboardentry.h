#ifndef SEARCHCLIENT_BLACKBOARDENTRY_H
#define SEARCHCLIENT_BLACKBOARDENTRY_H

#include "agent.h"

namespace SearchClient {

class Agent;
class Blackboard;
class BlackboardEntry {
    public:
        enum EntryType {
            NONE,
            POSITION_ENTRY,
            GLOBAL_GOAL_ENTRY,
            MOVE_HELP_GOAL_ENTRY
        };

        BlackboardEntry();
        BlackboardEntry(const BlackboardEntry& src);
        BlackboardEntry& operator=(const BlackboardEntry &src);
        
        bool operator==(const BlackboardEntry &compared) const;

        EntryType getType() const { return type_; }
        Coord     getLocation() const { return position_; }
        int       getAuthorId() const { return authorId_; }
        unsigned int getTimeStep() const { return timeStep_; }
        Blackboard* getParent() { return parent_; }
        const Blackboard* getParent() const { return parent_; }

        void setBlackboardParent(Blackboard *parent) { parent_ = parent; }
        void setAuthorId(int authorId) { authorId_ = authorId; }
        void setPosition(const Coord &position) { position_ = position; }
        void setType(EntryType type) { type_ = type; }
        void setTimeStep(int timeStep) { timeStep_ = timeStep; }
        
        static bool accept(BlackboardEntry *entry, const Agent &receiver);
        static bool revoke(BlackboardEntry *entry, const Agent &sender);
        
        static BlackboardEntry* create(EntryType type, const Agent &author, Blackboard* parent,  unsigned int timeStep = -1);
        static BlackboardEntry* create(EntryType type, Blackboard* parent, unsigned int timeStep = -1);

        unsigned int getPriority() const;
        void setPriority(unsigned int value);

private:
        Blackboard *parent_;
        EntryType type_; // Entry type (see the specification)
        Coord position_; // Position where the Entry 
        int authorId_;     // ID of the agent that created the Entry 
        unsigned int timeStep_;
        unsigned int priority_;
};

}
#endif
