#ifndef LEVEL3AGENT_H
#define LEVEL3AGENT_H

#include "level2agent.h"

namespace Agent {

class SearchAgent : public Level2Agent
{
public:
    SearchAgent(Color color, char num, Communication::Blackboard* blackboard = nullptr);


    /**
     * Returns the next move in the plan if there is one. otherwise
     * will generate a new plan and return head of that plan
     */
    SearchEngine::Command nextMove();

    std::vector<SearchEngine::State*> conductSubgoalSearch(bool *searchFailed);
    std::vector<SearchEngine::State*> conductHelpSubgoalSearch();

    bool isFirstMoveInPlan() const { return firstMoveInPlan_; }
    const std::vector<SearchEngine::Command>& getCurrentPlan() const { return plan_; }
    std::vector<SearchEngine::Command>& getCurrentPlan() { return plan_; }

private:
    bool positionFreePredicate(const SearchEngine::State* src) const;
    std::vector<SearchEngine::State*> recoverStrategy(const Box &targBox, const Goal &searchGoal);

private:
    // Plan attributes
    bool waitingForHelp_;
    bool firstMoveInPlan_;
    std::vector<SearchEngine::Command> plan_;
};

}

#endif // LEVEL3AGENT_H
