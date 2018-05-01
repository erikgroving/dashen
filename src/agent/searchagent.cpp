#include "searchagent.h"
#include "../strategies/strategyheuristic.h"
#include "../heuristics/astarheuristic.h"
#include "../strategies/strategybfs.h"

#define PFPREDICATE

using Agent::SearchAgent;
using namespace SearchEngine::Predicate;

using AgentToBoxStrategy = Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic>;
using BoxToGoalStrategy  = Strat::StrategyHeuristic<Heur::BoxToGoalAStarHeuristic>;

SearchAgent::SearchAgent(Color color, char num, Communication::Blackboard *blackboard):
    Level2Agent(color, num, blackboard) {

}

SearchEngine::Command SearchAgent::nextMove()
{
    /*
    if(waitingForHelp()) {
        return SearchEngine::Command();
    }
    */

    if(!plan_.empty()) {
        SearchEngine::Command ret = plan_[0];
        plan_.erase(plan_.begin());

        firstMoveInPlan_ = false;
        return ret;
    }

    firstMoveInPlan_ = true;

    bool isHelpGoal;
    if (!getNextGoal(&isHelpGoal)) {
        return SearchEngine::Command(); // no goals so send a NoOp back
    }

    computeCorrectGoals();
    std::vector<SearchEngine::State*> ans;

    if(!isHelpGoal) {
        bool searchFailed;
        ans = conductSubgoalSearch(&searchFailed);
        if(searchFailed) {
            // askForHelp(ans);
            return SearchEngine::Command();
        }
        else {
            plan_ = extractPlan(ans);
            postPlanOnBlackboard(ans);
        }
    }
    else {
        ans = conductHelpSubgoalSearch();
    }

    if(plan_.empty())
        return SearchEngine::Command();

    SearchEngine::Command ret = plan_[0];
    plan_.erase(plan_.begin());
    return ret;
}

std::vector<SearchEngine::State*> SearchAgent::conductSubgoalSearch(bool *searchFailed) {

    std::vector<SearchEngine::State*> result;
    Box &targBox = Level0Agent::sharedState->getBoxes()[getCurrentSearchGoal().assignedBoxID];

    if(!agentNextToBox(Level0Agent::sharedState, targBox, this)) {
        AgentToBoxStrategy strat(this);
        strat.setAdditionalCheckPredicate([this] (const SearchEngine::State* src) {
            std::string errorDescription;
            return positionFree(src->getAgents()[getIndex()].loc.x, src->getAgents()[getIndex()].loc.y,
                                src->getAction() ,src->getTimeStep(), -1, errorDescription);
        });

        result = searchBox(targBox, strat);
    }
    else {
        BoxToGoalStrategy strat(this);
        strat.setAdditionalCheckPredicate([&targBox, this] (const SearchEngine::State* src) {
            std::string errorDescription;
            return positionFree(src->getAgents()[getIndex()].loc.x, src->getAgents()[getIndex()].loc.y,
                                src->getAction() ,src->getTimeStep(), targBox.id, errorDescription);
        });
        result = searchGoal(getCurrentSearchGoal(), strat);
    }

    if(result.size() == 0) {
        *searchFailed = true;

        // Recover strategy
        return recoverStrategy(targBox, getCurrentSearchGoal());
    }
    else {
        *searchFailed = false;
        return result;
    }
}

std::vector<SearchEngine::State *> SearchAgent::recoverStrategy(const Box &targBox, const Goal &goal) {
    if(!agentNextToBox(sharedState, targBox, this)) {
        AgentToBoxStrategy strat(this);
        return searchBox(targBox, strat, true);
    }
    else {
        BoxToGoalStrategy strat(this);
        return searchGoal(goal, strat, true);
    }
}

std::vector<SearchEngine::State*> SearchAgent::conductHelpSubgoalSearch()
{   std::vector<SearchEngine::State*> ans;

    Agent::HelpGoal &hg = getCurrentHelpGoal();
    Communication::HelpEntry* entry = getCurrentHelpEntry();

    if(hg.type == Agent::HelpGoal::Agent) {
        Strat::StrategyBFSMovePriority strat;

        ans = searchHelpMoveAgent(hg, strat, true);
        hg.over = true;

        if(ans.size() == 0 || isAgentNotOnForbiddenPath(ans.back(), getIndex(), getCurrentHelpGoal().forbiddenPath)) {
            // Cannot help the other agent
            entry->setProblemType(Communication::HelpEntry::Done);
        }
        else {
            entry->setProblemType(Communication::HelpEntry::Agent);
        }
    }

    else if(hg.type == Agent::HelpGoal::Box) {

        Box &box = Level0Agent::sharedState->getBoxes()[hg.id];
        if(!agentNextToBox(Level0Agent::sharedState, box, this)) {

            AgentToBoxStrategy strat(this);
            strat.setAdditionalCheckPredicate([&box, this] (const SearchEngine::State* src) {
                std::string errorDescription;
                return positionFree(src->getAgents()[getIndex()].loc.x, src->getAgents()[getIndex()].loc.y,
                                    src->getAction() ,src->getTimeStep(), box.id, errorDescription);
            });
            ans = searchBox(box, strat);
        }
        else {
            Strat::StrategyBFS strat;
            ans = searchHelpMoveBox(hg, strat, true);

            hg.over = true; // TODO: check that it has indeed worked
            entry->setProblemType(Communication::HelpEntry::Done);
        }
    }

    return ans;
}
