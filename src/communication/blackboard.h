#ifndef COMMUNICATION_BLACKBOARD_H
#define COMMUNICATION_BLACKBOARD_H

#include <algorithm>
#include <vector>

#include "../searchengine/typedefs.h"

namespace Communication {

/**
 * A blackboard is an environment in which different types of informations can be disposed
 * by agents.
 * The entries can have different types:
 * -> POSITIONENTRY: indicates that an agent is going to be at a certain position at a certain timestep
 * -> GoalEntry: indicates open goals that can be retrieved by agents.
 *      |_ GLOBAL_GOAL_ENTRY: a goal tile needs a box on it
 *      |_ MOVE_HELP_GOAL_ENTRY: a box needs to be moved
 * 
 * More entry types can be added. Don't forget to update this little specification when designing a new one !
 * 
 * An entry MUST contain the following information:
 * -> Entry type
 * -> Position in the entry (coordinates of the goal for a GLOBAL_GOAL_ENTRY, coordinates of the box that needs to 
 *    be moved for a MOVE_HELP_GOAL_ENTRY, coordinates of the agent at a specific time step for a POSITIONENTRY)
 * -> Primary key (practically using the index) of the agent that created the entry
 */
class BlackboardEntry;
class Blackboard {
    public: 
        Blackboard();
        ~Blackboard();

    public:
        enum Registry {
            PositionEntry,
            BoxPositionEntry,
            GoalEntry,
            HelpEntry
        };

    public:
        void addEntry(BlackboardEntry *entry, Registry registry);

        std::vector<BlackboardEntry*>& getPositionEntries() { return positionEntries_; } 
        const std::vector<BlackboardEntry*>& getPositionEntries() const { return positionEntries_; }
        
        const std::vector<BlackboardEntry*>& getGoalEntries() const { return goalEntries_; }
        std::vector<BlackboardEntry*>& getGoalEntries() { return goalEntries_; }
        
        const std::vector<BlackboardEntry*>& getBoxEntries(int boxID) const { return boxPositionEntries_[boxID]; }
        std::vector<BlackboardEntry*>& getBoxEntries(int boxID) { return boxPositionEntries_[boxID]; }

        const std::vector<BlackboardEntry*>& getHelpEntries() const { return helpEntries_; }
        std::vector<BlackboardEntry*>& getHelpEntries() { return helpEntries_; }

        void setBoxEntryRegistrySize(size_t numBoxes);

        void removeEntry(const BlackboardEntry *entry, Registry registry);
        void removeEntriesByAuthor(char agentID, Registry registry);

        void clear(Registry registry, short id = -1);
        void erase_front(Registry registry, short id = -1);

        BlackboardEntry* findPositionEntry(unsigned int timeStep, int authorId);

    private:
        std::vector<BlackboardEntry*> positionEntries_;
        std::vector<std::vector<BlackboardEntry*>> boxPositionEntries_;
        std::vector<BlackboardEntry*> goalEntries_;
        std::vector<BlackboardEntry*> helpEntries_;
};

}
#endif
