#include "agent.h"
#include "../strategies/strategies"
#include "../heuristics/greedyheuristic.h"
#include "../heuristics/astarheuristic.h"
#include <limits.h>
#ifndef __LONG_MAX__
#define __LONG_MAX__ 2147483647
#endif
using SearchClient::Agent;
using SearchClient::Client;
using namespace SearchEngine::Predicate;

SearchEngine::State *Agent::sharedState;
unsigned int Agent::sharedTime = 0;

Agent::Agent(Color color, char num, Coord loc, 
                SearchEngine::Strategy *strategy, SearchClient::Blackboard *blackboard) :
            
            color(color), num(num), loc(loc),
            searchStrategy_(strategy), goalsToAchieve(), movableBoxes(), 
            private_initialState(), currentSearchGoal_(nullptr), blackboard_(blackboard), 
            correctGoals_(0), firstMoveInPlan_(false) {

}

Agent::~Agent() {
    if(private_initialState != nullptr)
        delete private_initialState;
}
void Agent::updateBoxesList(const SearchEngine::State &initialState) {
    movableBoxes.clear();

    for( const Box &box: initialState.getBoxes() ) {
        if(box.color == color)
            movableBoxes.push_back(box);
    }   
}

void Agent::updateGoalsList(const SearchEngine::State &initialState) {
    goalsToAchieve.clear();

    for( const Goal &goal: SearchEngine::State::goals ) {
           for(const Box &box: movableBoxes)
            if(goal.letter == box.letter)  
                goalsToAchieve.push_back(goal);
    }
}

Goal Agent::chooseGoal() {
    Goal result = goalsToAchieve.back();
    goalsToAchieve.pop_back();
    return result;
}

std::vector<SearchEngine::State*> Agent::searchGoal(const Goal &goal, SearchEngine::Strategy* strategy) {
    configurePrivateInitialState();

    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([&goal](const SearchEngine::State *currentState) {
        return goalHasCorrectBox(currentState, goal);
    }); 

    return searcher.search(*strategy, (int)num);
}
    
std::vector<SearchEngine::State*> Agent::searchBox(const Box& box, SearchEngine::Strategy* strategy) {
    configurePrivateInitialState();
    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([&box, this](const SearchEngine::State *currentState) {
        return agentNextToBox(currentState, box, this);
    }); 

    return searcher.search(*strategy, (int)num);

}

std::vector<SearchEngine::State*> Agent::searchAllGoals(SearchEngine::Strategy &strategy) {
    configurePrivateInitialState();
    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([](const SearchEngine::State *currentState) {
        return isGoalState(currentState);
    });    

    return searcher.search(strategy, (int)num );
}

void Agent::setSharedState(SearchEngine::State *sharedState) {
    Agent::sharedState = sharedState;
}

SearchEngine::Command Agent::nextMove() {
    /* If plan is empty, need to construct a new plan */
    if (plan_.empty()) {
        firstMoveInPlan_ = true;
        bool satisfied = true;
        Goal unsatGoal = Goal();
        Goal search_goal = Goal();

        // See if we have any unsatisfied goals
        for (const Goal& g : takenGoals_) {
            if (!goalHasCorrectBox(sharedState, g)) {
                satisfied = false;
                unsatGoal = g;
                break;
            }
        }

        if (satisfied) {
            search_goal = getGoalFromBlackboard();
        }
        else {
            search_goal = unsatGoal;
        }

        if (search_goal.letter == '-') {
            return SearchEngine::Command(); // no goals so send a NoOp back
        }

        currentSearchGoal_ = &search_goal;

        correctGoals_ = 0;
        for (const Goal& g : SearchEngine::State::goals) {
            if (SearchEngine::Predicate::goalHasCorrectBox(sharedState, g)) {
                correctGoals_ += 1;
            }
        }
        
        // Search to find the answer for the goal
        std::vector<SearchEngine::State*> ans = std::vector<SearchEngine::State*>();
        Box &targBox = sharedState->getBoxes()[search_goal.assignedBoxID];
        SearchEngine::Strategy* strat;


        if (!agentNextToBox(sharedState, targBox, this)) {
            std::cerr << "Getting to box " << targBox.id << " (" << targBox.loc.x << ", " << targBox.loc.y << ")" << std::endl;
            strat = new Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic>(this);
            strat->setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
                return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                    state->getAction() ,state->getTimeStep());
            });
            ans = searchBox(targBox, strat); //TODO reflect proper strategy
            delete strat;
        }
        else {
            //std::cerr << "Satisfying goal (" << search_goal.loc.x << ", " << search_goal.loc.y << ") with box " << targBox.id << std::endl;
            strat = new Strat::StrategyHeuristic<Heur::BoxToGoalAStarHeuristic>(this);
            strat->setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
                return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                    state->getAction(), state->getTimeStep());
            });
            ans = searchGoal(search_goal, strat);
            delete strat;
        }

        if (!ans.empty()) {
            auto *entry = SearchClient::BlackboardEntry::create(SearchClient::BlackboardEntry::POSITION_ENTRY, *this, blackboard_, sharedTime - 1);
            entry->setPosition(private_initialState->getAgents()[num].loc);

        }
        // Construct a  plan for the answer
        for (auto *a : ans) {
            plan_.push_back(a->getAction());
            auto *entry = SearchClient::BlackboardEntry::create(SearchClient::BlackboardEntry::POSITION_ENTRY, *this, blackboard_, a->getTimeStep());
            //std::cerr << "Location (" << a->getAgents()[num].loc.x << ", " << a->getAgents()[num].loc.y << ")\n";
            entry->setPosition(a->getAgents()[num].loc);

            SearchEngine::Command cmd = a->getAction();

            // Add the box position entries
            if (cmd.action() == PUSH || cmd.action() == PULL) { 
                Box targBox;
                targBox = a->getBoxes()[cmd.targBoxId()];
                int newBoxCol, newBoxRow;
                if (cmd.action() == PUSH) {
                    newBoxCol = a->getAgents()[num].loc.x + SearchEngine::Command::colToInt(cmd.d2());
                    newBoxRow = a->getAgents()[num].loc.y + SearchEngine::Command::rowToInt(cmd.d2());
                }
                else if (cmd.action() == PULL) {
                    targBox = a->getBoxes()[cmd.targBoxId()];
                    newBoxCol = a->getAgents()[num].loc.x;
                    newBoxRow = a->getAgents()[num].loc.y;
                }
                Coord newBoxLoc = Coord(newBoxCol, newBoxRow);
                auto* boxEntry = SearchClient::BlackboardEntry::create(
                        SearchClient::BlackboardEntry::BOX_POSITION_ENTRY, blackboard_, targBox.id, a->getTimeStep());
                boxEntry->setPosition(newBoxLoc);
            }
        }
    }
    else {
        firstMoveInPlan_ = false;
    }

    if (plan_.empty()) {
        return SearchEngine::Command();
    }

    SearchEngine::Command ret = plan_[0];
    plan_.erase(plan_.begin());


    return ret;
}

bool Agent::isEntryDoable(const SearchClient::BlackboardEntry *entry, const SearchEngine::State* state, int *boxIndex) {

    Box closestBox = Box();
    int goalIndex = -1;
        goalAt(state, entry->getLocation().x, entry->getLocation().y, &goalIndex);
    Goal entryGoal = SearchEngine::State::goals[goalIndex];

    unsigned long minDist = ULONG_MAX;
    for (const Box &b : state->getBoxes()) {

        if (b.color == color && b.letter == entryGoal.letter && !State::takenBoxes[b.id]) {
            //unsigned long dist = Coord::distance(b.loc, entryGoal.loc);
            unsigned long dist = Heur::DistanceOracle::fetchDistFromCoord(entryGoal.loc, b.loc);
            if (dist < minDist) {
                minDist = dist;
                closestBox = b;
            }
        }

    }

    if (closestBox.id == -1) {
        return false;
    }

    if(boxIndex != nullptr) {
        *boxIndex = closestBox.id;
    }

    return true;
}
    

bool Agent::positionFree(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep) {
    size_t box_x = x;  // checks that both an agent and boxes new position is valid with timestep
    size_t box_y = y;
    if (cmd.action() == PUSH) {
        box_x = x + SearchEngine::Command::colToInt(cmd.d2());
        box_y = y + SearchEngine::Command::rowToInt(cmd.d2());
    }
    Coord newAgentPos = Coord(x, y);
    Coord newBoxPos = Coord(box_x, box_y);

    auto positionEntries = blackboard_->getPositionEntries();

    // check against agents
    for(const SearchClient::BlackboardEntry *entry: positionEntries) {
        Coord entryPos = entry->getLocation();
        if(newAgentPos == entryPos || newBoxPos == entryPos) {
            // Okay... so an agent/box will go to this entry. But.. WHEN?
            int timeDiff = entry->getTimeStep() - timeStep;
            if (abs(timeDiff) <= 1) {
                return false;
            }
        }
    }

    // check against boxes
    for (size_t id = 0; id < private_initialState->getBoxes().size(); id++) {
        auto boxPositionEntries = blackboard_->getBoxEntries(id);
        for (const SearchClient::BlackboardEntry* entry: boxPositionEntries) {
            Coord entryPos = entry->getLocation();
            if(newAgentPos == entryPos || newBoxPos == entryPos) {
                // Okay... so an agent will go to this entry. But.. WHEN?
                int timeDiff = entry->getTimeStep() - timeStep;
                if (abs(timeDiff) <= 1) {
                    return false;
                }
            }
        }
    }

    return true;
}

Goal Agent::getGoalFromBlackboard() {
    // Identify a suitable goal from the blackboard
    if(blackboard_->getGoalEntries().size() <= 0) return Goal();

    SearchClient::BlackboardEntry *selectedEntry = nullptr;
    unsigned int priority = 0;

    int closestBoxIndex = -1;
    for (SearchClient::BlackboardEntry *entry : blackboard_->getGoalEntries()) {
        if (entry->getType() == SearchClient::BlackboardEntry::GLOBAL_GOAL_ENTRY &&
            priority < entry->getPriority()                                      &&
            isEntryDoable(entry, sharedState, &closestBoxIndex)                        ) {
            selectedEntry = entry;
            priority = entry->getPriority();
        }
    }

    if(selectedEntry == nullptr)
        return Goal();

    /* Find the goal for that position */
    int goalIndex;
        goalAt(sharedState, selectedEntry->getLocation().x, selectedEntry->getLocation().y, &goalIndex);

    Goal &result = SearchEngine::State::goals[goalIndex];



    State::takenBoxes[closestBoxIndex] = true;
    result.assignedBoxID = closestBoxIndex;
    takenGoals_.push_back(result);

    SearchClient::BlackboardEntry::accept(selectedEntry, *this);
    return result;
}

void Agent::clearPlan(SearchEngine::Command cmd) {
    if (cmd.action() == PUSH || cmd.action() == PULL) {
        int id = cmd.targBoxId();
        blackboard_->clearBoxEntries(id);
    }
    for (SearchEngine::Command cmd : plan_) {
        if (cmd.action() == PUSH || cmd.action() == PULL) {
            int id = cmd.targBoxId();
            blackboard_->clearBoxEntries(id);
        }
    }
    plan_.erase(plan_.begin(), plan_.end());
    blackboard_->removeEntriesByAuthor(num);
}

void Agent::configurePrivateInitialState() {
    if(private_initialState != nullptr)
        delete private_initialState;

    private_initialState = new SearchEngine::State(*Agent::sharedState);
    private_initialState->setInitialTimeStep(Agent::sharedTime);

    std::vector<char> agentsInMotion = std::vector<char>();
    SearchClient::Blackboard* bbPtr = this->getBlackboard();


    // Remove agents in motion. Will use blackboard instead 
    for (auto& entry : bbPtr->getPositionEntries()) {
        if (std::find(agentsInMotion.begin(), agentsInMotion.end(), entry->getAuthorId()) 
                == agentsInMotion.end()) {
            agentsInMotion.push_back(entry->getAuthorId());
        }
    }

    // Blackboard used for boxes that are in motion.
    auto boxes = private_initialState->getBoxes();
    for (Box& b : boxes) {
        if (bbPtr->getBoxEntries(b.id).size() > 1) {
            b.loc.x = -1;
        }
    }
    private_initialState->setBoxes(boxes);

    std::vector<AgentDescription> agents = private_initialState->getAgents();
    for (char& agentID : agentsInMotion) {
        agents[agentID].loc.x = -1;
    }
    private_initialState->setAgents(agents);
}