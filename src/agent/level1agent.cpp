#include "level1agent.h"

#include "../searchengine/state.h"
#include "../searchengine/strategy.h"
#include "../searchengine/searchclient.h"
#include "../searchengine/predicate.h"
#include "../communication/positionentry.h"
#include "../communication/boxpositionentry.h"

using Agent::Level0Agent;
using Agent::Level1Agent;
using Agent::HelpGoal;
using namespace SearchEngine::Predicate;

Level1Agent::Level1Agent(Color color, char num, Communication::Blackboard *blackboard):
    Level0Agent(color, num, blackboard), private_initialState() {

}

Level1Agent::Level1Agent(const Agent::Level1Agent &src):
    Level0Agent(src), private_initialState(src.private_initialState) {
}

Agent::Level1Agent &Level1Agent::operator=(const Agent::Level1Agent &src)
{
    Level0Agent::operator=(src);
    private_initialState = src.private_initialState;
    return *this;
}

Level1Agent::~Level1Agent()
{
    if(private_initialState != nullptr)
        delete private_initialState;
}

std::vector<SearchEngine::State *> Level1Agent::searchGoal(const Goal &goal, SearchEngine::Strategy &strategy, bool ignoreOthers)
{
    configurePrivateInitialState(goal.assignedBoxID, ignoreOthers);

    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([&goal](const SearchEngine::State *currentState) {
        return goalHasCorrectBox(currentState, goal);
    });

    return searcher.search(strategy, (int) getIndex());
}

std::vector<SearchEngine::State *> Level1Agent::searchBox(const Box &box, SearchEngine::Strategy &strategy, bool ignoreOthers)
{
    configurePrivateInitialState(box.id, ignoreOthers);

    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([&box, this](const SearchEngine::State *currentState)       {
        return agentNextToBox(currentState, box, this);
    });

    return searcher.search(strategy, (int) getIndex());
}

std::vector<SearchEngine::State *> Level1Agent::searchHelpMoveAgent(const HelpGoal &hg, SearchEngine::Strategy &strategy, bool ignoreOthers)
{

    configurePrivateInitialState(-1, ignoreOthers);

    SearchEngine::SearchCli searcher(private_initialState);

    searcher.setGoalStatePredicate([&hg, this](const SearchEngine::State *currentState){
        return isAgentNotOnForbiddenPath(currentState, hg.id, hg.forbiddenPath);
    });

    return searcher.search(strategy, (int) getIndex());
}

std::vector<SearchEngine::State *> Level1Agent::searchHelpMoveBox(const HelpGoal &hg, SearchEngine::Strategy &strategy, bool ignoreOthers)
{
    configurePrivateInitialState(hg.id, ignoreOthers);

    SearchEngine::SearchCli searcher(private_initialState);

    searcher.setGoalStatePredicate([&hg, this](const SearchEngine::State *currentState){
        return isAgentNotOnForbiddenPath(currentState, getIndex(), hg.forbiddenPath) &&
               isBoxNotOnForbiddenPath(currentState, hg.id, hg.forbiddenPath);
    });

    return searcher.search(strategy, (int) getIndex());
}

std::vector<SearchEngine::Command> Level1Agent::extractPlan(const std::vector<SearchEngine::State *> &ans) const
{
    std::vector<SearchEngine::Command> result;
    // Construct a  plan for the answer
    for (auto *a : ans) {
        result.push_back(a->getAction());
    }
    return result;
}

void Level1Agent::postPlanOnBlackboard(const std::vector<SearchEngine::State *> &plan)
{
    if(getBlackboard() == nullptr)
        return;

    if(plan.empty())
        return;

    // Post the current entry
    Communication::PositionEntry::create(
        private_initialState->getAgents()[getIndex()].loc,
        sharedTime - 1,
        *this,
        getBlackboard()
    );

    for(const SearchEngine::State* state: plan) {

        // Add the PositionEntry corresponding to the agent position
        Communication::PositionEntry::create(
            state->getAgents()[getIndex()].loc,
            state->getTimeStep(),
            *this,
            getBlackboard()
        );

        // Add the BoxPositionEntry corresponding to the box position (if needed)
        auto &cmd = state->getAction();
        if(cmd.action() == PUSH || cmd.action() == PULL) {
            Box targBox = state->getBoxes()[cmd.targBoxId()];

            switch (cmd.action()) {
            case PUSH:
            {
                int newBoxCol = state->getAgents()[getIndex()].loc.x + SearchEngine::Command::colToInt(cmd.d2());
                int newBoxRow = state->getAgents()[getIndex()].loc.y + SearchEngine::Command::rowToInt(cmd.d2());

                Communication::BoxPositionEntry::create(
                    Coord(newBoxCol, newBoxRow),
                    state->getTimeStep(),
                    targBox.id,
                    *this,
                    getBlackboard()
                );
            }
            break;

            case PULL:
                Communication::BoxPositionEntry::create(
                    Coord(state->getAgents()[getIndex()].loc.x, state->getAgents()[getIndex()].loc.y),
                    state->getTimeStep(),
                    targBox.id,
                    *this,
                    getBlackboard()
                );
            break;
            default:
                break;
            }
        }
    }
}

void Level1Agent::configurePrivateInitialState(const int interestingBoxId, bool ignoreOthers)
{
    if(private_initialState != nullptr) {
        delete private_initialState;
    }
    private_initialState = new SearchEngine::State(*sharedState);
    private_initialState->setInitialTimeStep(sharedTime);

    std::vector<char> agentsInMotion = std::vector<char>();
    Communication::Blackboard* bbPtr = this->getBlackboard();


    // If everyone needs to be ignored, do so and stop the method
    if(ignoreOthers) {

        for(auto &agent: private_initialState->getAgents()) {
            if(agent.num != getIndex())
                agent.loc.x = -1;
        }

        for(auto &box: private_initialState->getBoxes()) {
            if(box.id != interestingBoxId)
                box.loc.x = -1;
        }

        return;
    }

    // Not everyone needs to be removed. So, we only remove the boxes and agents that are in motion.


    // -- Remove agents in motion. Will use blackboard instead
    for (auto& entry : bbPtr->getPositionEntries()) {
        if (std::find(agentsInMotion.begin(), agentsInMotion.end(), entry->getAuthorId())
                == agentsInMotion.end()) {
            agentsInMotion.push_back(entry->getAuthorId());
        }
    }

    // -- Blackboard used for boxes that are in motion.
    for (Box& b : private_initialState->getBoxes()) {
        if (bbPtr->getBoxEntries(b.id).size() > 1) {
            b.loc.x = -1;
        }
    }

    std::vector<AgentDescription> agents = private_initialState->getAgents();
    for (char& agentID : agentsInMotion) {
        agents[agentID].loc.x = -1;
    }
}

void Level1Agent::clearPlan(const SearchEngine::Command &currentAction, std::vector<SearchEngine::Command> &plan) {

    if(getBlackboard() == nullptr)
        return;

    const SearchEngine::Command *currentCommand = &currentAction;
    auto iterator = plan.begin();
    do {

        if (currentCommand->action() == PUSH || currentCommand->action() == PULL) {
            int id = currentCommand->targBoxId();
            getBlackboard()->clear(Communication::Blackboard::BoxPositionEntry, id);
        }

        iterator++;
        if( iterator != plan.end() )
            currentCommand = &(*iterator);
        else
            currentCommand = nullptr;

    } while(currentCommand != nullptr);

    plan.erase(plan.begin(), plan.end());
    getBlackboard()->removeEntriesByAuthor(getIndex(), Communication::Blackboard::PositionEntry);
}

