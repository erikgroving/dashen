#ifndef LEVEL1AGENT_H
#define LEVEL1AGENT_H

#include "level0agent.h"
#include "../searchengine/command.h"

namespace SearchEngine {
    class Strategy;
}

namespace Agent {

struct HelpGoal {

    enum HelpType {
        None,
        Agent,
        Box
    } type;

    HelpGoal(HelpType htype = None, int idd = -1, const Coord& hposition = Coord(), std::vector<Coord> hforbiddenPath = std::vector<Coord>(), bool hover = true):
        type(htype), id(idd), position(hposition), forbiddenPath(hforbiddenPath), over(hover){

    }
    HelpGoal(const HelpGoal &src):
        type(src.type), id(src.id), position(src.position), forbiddenPath(src.forbiddenPath), over(src.over) {}


    int id;
    Coord position;
    std::vector<Coord> forbiddenPath;
    bool over;
};


class Level1Agent: public Level0Agent
{

public: // Constructors
    Level1Agent(Color color, char num, Communication::Blackboard* blackboard = nullptr);
    Level1Agent(const Level1Agent& src);
    Level1Agent& operator=(const Level1Agent& src);
    ~Level1Agent();

public: // Search methods
    /**
     * Given a goal, return a sequence of action to accomplish it. Set ignoreOthers to true if you want to remove all the other agents and boxes from the state space.
     */
    std::vector<SearchEngine::State*> searchGoal(const Goal &goal, SearchEngine::Strategy& strategy, bool ignoreOthers = false);

    /**
     * Given a box, return a sequence of actions to get to the box. Set ignoreOthers to true if you want to remove all the other agents and boxes from the state space.
     */
    std::vector<SearchEngine::State*> searchBox(const Box& box, SearchEngine::Strategy& strategy, bool ignoreOthers = false);

    std::vector<SearchEngine::State*> searchHelpMoveBox(const HelpGoal &hg, SearchEngine::Strategy &strategy, bool ignoreOthers = false);

    std::vector<SearchEngine::State *> searchHelpMoveAgent(const HelpGoal &hg, SearchEngine::Strategy &strategy, bool ignoreOthers = false);

public: // Public helper methods
    /**
     * Extracts the plan from a subgoal search
     */
    std::vector<SearchEngine::Command> extractPlan(const std::vector<SearchEngine::State*>& ans) const;

    /**
     * Remove the entries from the blackboard corresponding to a given plan and a current action
     */
    void clearPlan(const SearchEngine::Command &currentAction, std::vector<SearchEngine::Command> &plan);

    /**
     * Post all PositionEntries and BoxPositionEntries described by plan on the blackboard
     */
    void postPlanOnBlackboard(const std::vector<SearchEngine::State*> &plan);
private: // Private helper methods
    /**
     * Prepare the private initial state for a new search. Must be called before any search to prevent overlapping searches (which honestly are quite useless).
     */
    void configurePrivateInitialState(const int interestingBoxId, bool ignoreOthers);


private:
    SearchEngine::State* private_initialState;
};

}
#endif // LEVEL1AGENT_H
