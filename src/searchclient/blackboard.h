#ifndef SEARCHCLIENT_BLACKBOARD_H
#define SEARCHCLIENT_BLACKBOARD_H

#include <algorithm>
#include <vector>

#include "agent.h"
#include "../searchengine/typedefs.h" 

namespace SearchClient {

/**
 * A blackboard is an environment in which different types of informations can be disposed
 * by agents.
 * The entries can have different types:
 * -> PositionEntry: indicates that an agent is going to be at a certain position at a certain timestep
 * -> GoalEntry: indicates open goals that can be retrieved by agents.
 *      |_ GlobalGoalEntry: a goal tile needs a box on it
 *      |_ MoveHelpGoalEntry: a box needs to be moved
 * 
 * More entry types can be added. Don't forget to update this little specification when designing a new one !
 * 
 * An entry MUST contain the following information:
 * -> Entry type
 * -> Position in the entry (coordinates of the goal for a GlobalGoalEntry, coordinates of the box that needs to 
 *    be moved for a MoveHelpGoalEntry, coordinates of the agent at a specific time step for a PositionEntry)
 * -> Primary key (practically using the index) of the agent that created the entry
 */
class Blackboard {
    public: 
        Blackboard();

    public:
        class Entry;

        void addEntry(Entry &entry);

        std::vector<Entry>& getPositionEntries() { return positionEntries_; } 
        const std::vector<Entry>& getPositionEntries() const { return positionEntries_; }
        
        const std::vector<Entry>& getGoalEntries() const { return goalEntries_; }
        std::vector<Entry>& getGoalEntries() { return goalEntries_; }

        void removeEntry(const Entry &entry);
    private:
        std::vector<Entry> positionEntries_;
        std::vector<Entry> goalEntries_;
};

class Blackboard::Entry {
    public:
        enum EntryType {
            None,
            PositionEntry,
            GlobalGoalEntry,
            MoveHelpGoalEntry
        };

        Entry();
        Entry(const Entry& src);
        Entry& operator=(const Entry &src);
        
        bool operator==(const Entry &compared) const;

        EntryType getType() const { return type_; }
        Coord     getPosition() const { return position_; }
        int       getAuthorId() const { return authorId_; }
        
        Blackboard* getParent() { return parent_; }
        const Blackboard* getParent() const { return parent_; }

        void setBlackboardParent(Blackboard *parent) { parent_ = parent; }
        void setAuthorId(int authorId) { authorId_ = authorId_; }
        void setPosition(const Coord &position) { position_ = position; }
        void setType(EntryType type) { type_ = type; }
        
        static bool accept(Entry &entry, const Agent &receiver);
        static bool revoke(Entry &entry, const Agent &sender);
        static Entry create(EntryType type, const Agent &author);

    private:
        Blackboard *parent_;
        EntryType type_; // Entry type (see the specification)
        Coord position_; // Position where the Entry 
        int authorId_;     // ID of the agent that created the Entry 
};

}
#endif