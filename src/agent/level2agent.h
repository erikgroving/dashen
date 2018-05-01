#ifndef LEVEL2AGENT_H
#define LEVEL2AGENT_H

#include "level1agent.h"
#include "../communication/helpentry.h"
#include "../communication/globalgoalentry.h"

namespace Agent {

class Level2Agent : public Level1Agent
{
public:
    Level2Agent(Color color, char num, Communication::Blackboard* blackboard = nullptr);

    SearchEngine::Command nextMove();

    /**
     * Compute the next goal that the agent needs to solve
     *
     * isHelpGoal is mandatory. Its value is set to false if the computed goal is
     * not a help goal, true otherwise.
     * \return true if a goal is available, false otherwise.
     */
    bool getNextGoal(bool *isHelpGoal);

    /**
     * Look inside the blackboard for a doable GoalEntry. If one is available,
     * it is store in the private attribute currentSearchGoal_.
     * \return true if a doable GoalEntry has been found, false otherwise.
     */
    bool getGlobalGoal();

    bool getHelpGoal();

    bool waitingForHelp();
    /**
     * Checks to see if the entry is doable by the agent, this currently means
     * that the agent checks that there is a box with th same color as the
     * agent that has the same letter as the goal
     */
    bool isEntryDoable(const Communication::BlackboardEntry *entry, const SearchEngine::State* state, int *boxIndex = 0);

    void askForHelp(const std::vector<SearchEngine::State *> &path);

public: // Getters
    HelpGoal& getCurrentHelpGoal() { return currentHelpGoal_; }

    const Goal& getCurrentSearchGoal() const { return currentSearchGoal_; }
    Goal& getCurrentSearchGoal() { return currentSearchGoal_; }

    Communication::HelpEntry* getCurrentHelpEntry() { return currentHelpEntry_; }

    int getCorrectGoals() const { return correctGoals_; }
    void computeCorrectGoals();

private:

    // Search attributes
    Goal currentSearchGoal_;
    std::vector<Goal> takenGoals_;

    // Help attributes
    HelpGoal currentHelpGoal_;
    Communication::HelpEntry* currentHelpEntry_;
    std::vector<Communication::HelpEntry*> helpEntriesToMonitor_;

    // Helper for the search
    int correctGoals_;
};

}

#endif // LEVEL2AGENT_H
