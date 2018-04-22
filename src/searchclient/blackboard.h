#ifndef SEARCHCLIENT_BLACKBOARD_H
#define SEARCHCLIENT_BLACKBOARD_H

#include <algorithm>
#include <vector>

#include "../searchengine/typedefs.h"

namespace SearchClient {

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
        void addEntry(BlackboardEntry *entry);

        std::vector<BlackboardEntry*>& getPositionEntries() { return positionEntries_; } 
        const std::vector<BlackboardEntry*>& getPositionEntries() const { return positionEntries_; }
        
        const std::vector<BlackboardEntry*>& getGoalEntries() const { return goalEntries_; }
        std::vector<BlackboardEntry*>& getGoalEntries() { return goalEntries_; }

        void removeEntry(const BlackboardEntry *entry);
        void removeEntriesByAuthor(char agentID);

        BlackboardEntry* findPositionEntry(unsigned int timeStep, int authorId);

    private:
        std::vector<BlackboardEntry*> positionEntries_;
        std::vector<BlackboardEntry*> goalEntries_;
};

}
#endif