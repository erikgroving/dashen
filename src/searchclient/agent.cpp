#include "agent.h"

#include "../strategies/strategies"
#include "../heuristics/greedyheuristic.h"
#include "../heuristics/astarheuristic.h"
#include "../heuristics/distanceoracle.h"

#include "../communication/boxpositionentry.h"
#include "../communication/globalgoalentry.h"
#include "../communication/positionentry.h"
#include "../communication/helpentry.h"


using SearchClient::Agent;
using SearchClient::Client;
using namespace SearchEngine::Predicate;

SearchEngine::State *Agent::sharedState;
unsigned int Agent::sharedTime = 0;

Agent::Agent(Color color, char num, Coord loc, Communication::Blackboard *blackboard) :
            color(color), num(num), loc(loc), goalsToAchieve(), movableBoxes(),
            private_initialState(), currentSearchGoal_(Goal()), blackboard_(blackboard),
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

/*
void Agent::askForHelp(const Coord &helpPosition, const std::string &helpType, const std::vector<State *> &path)
{
    SearchClient::BlackboardEntry *entry = SearchClient::BlackboardEntry::create(SearchClient::BlackboardEntry::MOVE_HELP_GOAL_ENTRY, *this, blackboard_, sharedTime);
    entry->setPosition(helpPosition);

    if(helpPosition == "Agent")

}
*/

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

        // Find the next goal to complete
        currentSearchGoal_ = determineNextGoal();

        // If the search goal is '-', there are no goals available
        if (currentSearchGoal_.letter == '-') {
            return SearchEngine::Command(); // no goals so send a NoOp back
        }

        // Gets the current amount of goals satisfied in the current state
        correctGoals_ = SearchEngine::Predicate::getCorrectGoals(sharedState);
        
        // Search to find the answer for the goal
        std::vector<SearchEngine::State*> ans = conductSubgoalSearch();
        
        // Extract the plan from the search
        extractPlan(ans);

        // Post entries to the blackboard
        postAllPositionEntries(ans);
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

bool Agent::isEntryDoable(const Communication::BlackboardEntry *entry, const SearchEngine::State* state, int *boxIndex) {

    Box closestBox = Box();
    int goalIndex = -1;
        goalAt(state, entry->getLocation().x, entry->getLocation().y, &goalIndex);
    Goal entryGoal = SearchEngine::State::goals[goalIndex];

    if (Heur::DistanceOracle::fetchDistFromCoord(entryGoal.loc, state->getAgents()[num].loc) == (unsigned long)-1) {
        return false;
    }

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
    for(const Communication::BlackboardEntry *entry: positionEntries) {
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
        for (const Communication::BlackboardEntry* entry: boxPositionEntries) {
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

    Communication::GlobalGoalEntry *selectedEntry = nullptr;
    unsigned int priority = 0;

    int closestBoxIndex = -1;
    for (Communication::BlackboardEntry *entry : blackboard_->getGoalEntries()) {
        Communication::GlobalGoalEntry *entry_casted = static_cast<Communication::GlobalGoalEntry*>(entry);
        if( priority < entry_casted ->getPriority()                      &&
            isEntryDoable(entry_casted , sharedState, &closestBoxIndex) ) {
            selectedEntry = entry_casted ;
            priority = entry_casted ->getPriority();
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

    Communication::BlackboardEntry::accept(selectedEntry, *this);
    return result;
}

void Agent::clearPlan(SearchEngine::Command cmd) {
    if (cmd.action() == PUSH || cmd.action() == PULL) {
        int id = cmd.targBoxId();
        blackboard_->clear(Communication::Blackboard::BoxPositionEntry, id);
    }
    for (SearchEngine::Command cmd : plan_) {
        if (cmd.action() == PUSH || cmd.action() == PULL) {
            int id = cmd.targBoxId();
            blackboard_->clear(Communication::Blackboard::BoxPositionEntry, id);
        }
    }
    plan_.erase(plan_.begin(), plan_.end());
    blackboard_->removeEntriesByAuthor(num, Communication::Blackboard::PositionEntry);
}

void Agent::configurePrivateInitialState() {
    if(private_initialState != nullptr)
        delete private_initialState;

    private_initialState = new SearchEngine::State(*Agent::sharedState);
    private_initialState->setInitialTimeStep(Agent::sharedTime);

    std::vector<char> agentsInMotion = std::vector<char>();
    Communication::Blackboard* bbPtr = this->getBlackboard();


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

Goal Agent::determineNextGoal() {
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

    return search_goal;
}
    
    
std::vector<SearchEngine::State*> Agent::conductSubgoalSearch() {
    std::vector<SearchEngine::State*> ans = std::vector<SearchEngine::State*>();
    Box &targBox = sharedState->getBoxes()[currentSearchGoal_.assignedBoxID];
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
        std::cerr << "Satisfying goal (" << currentSearchGoal_.loc.x << ", " << currentSearchGoal_.loc.y << ") with box " << targBox.id << std::endl;
        strat = new Strat::StrategyHeuristic<Heur::BoxToGoalAStarHeuristic>(this);
        strat->setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
            return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                state->getAction(), state->getTimeStep());
        });
        ans = searchGoal(currentSearchGoal_, strat);
        delete strat;
    }

    return ans;
}

void Agent::extractPlan(const std::vector<SearchEngine::State*>& ans) {
    // Construct a  plan for the answer
    for (auto *a : ans) {
        plan_.push_back(a->getAction());
    }
}

void Agent::postAllPositionEntries(const std::vector<SearchEngine::State*>& ans) {
    if (!ans.empty())
        Communication::PositionEntry::create(private_initialState->getAgents()[num].loc, sharedTime - 1, *this, blackboard_);

    // Construct a  plan for the answer
    for (auto *a : ans) {
        Communication::PositionEntry::create(a->getAgents()[num].loc, a->getTimeStep(), *this, blackboard_ );

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

            Communication::BoxPositionEntry::create(
                    Coord(newBoxCol, newBoxRow), a->getTimeStep(), targBox.id, blackboard_ );
        }
    }
}
