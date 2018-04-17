#include "agent.h"
#include "../strategies/strategies"
#include "../heuristics/greedyheuristic.h"
using SearchClient::Agent;
using SearchClient::Client;
using namespace SearchEngine::Predicate;

SearchEngine::State *Agent::sharedState;
unsigned int Agent::sharedTime = 0;

Agent::Agent(Color color, char num, Coord loc, 
                SearchEngine::Strategy *strategy, SearchClient::Blackboard *blackboard) :
            
            color(color), num(num), loc(loc),
            searchStrategy_(strategy), goalsToAchieve(), movableBoxes(), 
            private_initialState(), currentSearchGoal_(nullptr), blackboard_(blackboard) {

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
    if(private_initialState != nullptr)
        delete private_initialState;

    private_initialState = new SearchEngine::State(*Agent::sharedState);
    private_initialState->setInitialTimeStep(Agent::sharedTime);

    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([&goal](const SearchEngine::State *currentState) {
        return goalHasCorrectBox(currentState, goal);
    });    

    return searcher.search(*strategy, (int)num);
}

std::vector<SearchEngine::State*> Agent::searchAllGoals(SearchEngine::Strategy &strategy) {
    auto initialSearchState = *Agent::sharedState;
    
    SearchEngine::SearchCli searcher(&initialSearchState);
    searcher.setGoalStatePredicate([](const SearchEngine::State *currentState) {
        return isGoalState(currentState);
    });    

    return searcher.search(strategy, (int)num );
}

void Agent::setSharedState(SearchEngine::State *sharedState) {
    Agent::sharedState = sharedState;
}

SearchEngine::Command Agent::nextMove(SearchClient::Blackboard* b, SearchEngine::State s) {
    /* If plan is empty, need to construct a new plan */
    if (plan_.empty()) {
        bool satisfied = true;
        Goal unsatGoal('-', Coord(-1, -1));
        Goal potGoal('-', Coord(-1, -1));
        Goal search_goal('-', Coord(-1, -1));
        // See if we have any unsatisfied goals
        for (Goal& g : takenGoals_) {
            if (!goalHasCorrectBox(&s, g)) {
                satisfied = false;
                unsatGoal = g;
                break;
            }
        }

        if (satisfied) {
            // Identify a suitable goal from the blackboard
            std::vector<SearchClient::BlackboardEntry*> entries = b->getGoalEntries();
            for (auto *entry : entries) {
                if (entry->getType() == SearchClient::BlackboardEntry::GLOBAL_GOAL_ENTRY) {
                    /* Find the goal for that position */
                    Coord entryLoc = entry->getPosition();
                    for (Goal& g : SearchEngine::State::goals) {
                        if (entryLoc == g.loc) {
                            potGoal = g;
                            break;
                        }
                    }
                    if (isEntryDoable(potGoal, s)) {
                        SearchClient::BlackboardEntry::accept(entry, *this);
                        search_goal = potGoal;
                        takenGoals_.push_back(search_goal);
                        break;
                    }
                }
            }
        }
        else {
            search_goal = unsatGoal;
        }

        if (search_goal.letter == '-') {
            return SearchEngine::Command(); // no goals so send a NoOp back
        }

        currentSearchGoal_ = &search_goal;

        // Search to find the answer for the goal
        //SearchEngine::Strategy* strat = new Strat::StrategyBFS();
        SearchEngine::Strategy* strat = new Strat::StrategyHeuristic<Heur::GreedyHeuristic>(this);
        strat->setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
            return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, state->getTimeStep());
        });
        std::vector<SearchEngine::State*> ans = searchGoal(search_goal, strat); //TODO reflect proper strategy

        // Construct a  plan for the answer
        for (auto *a : ans) {
            plan_.push_back(a->getAction());
            auto *entry = SearchClient::BlackboardEntry::create(SearchClient::BlackboardEntry::POSITION_ENTRY, *this, blackboard_, a->getTimeStep());
            std::cerr << "Location (" << a->getAgents()[num].loc.x << ", " << a->getAgents()[num].loc.y << ")";
            entry->setPosition(a->getAgents()[num].loc);
        }

        delete strat;
    }

    std::cerr << "Plan computed"; 
    if (plan_.empty()) {
        return SearchEngine::Command();
    }

    SearchEngine::Command ret = plan_[0];
    plan_.erase(plan_.begin());

    SearchClient::BlackboardEntry::revoke(blackboard_->findPositionEntry(sharedTime, num), *this);

    return ret;
}

bool Agent::isEntryDoable(Goal g, SearchEngine::State& s) {
    // TODO make boxes queryable by color/letter
    AgentDescription agent = s.getAgents()[num];
    if (SearchEngine::State::distance.getDistance(agent.loc, g.loc) == -1) {
        return false;
    }
    for (Box b : s.getBoxes()) {
        if (b.color == color && b.letter == g.letter) {
            return true;
        }
    } 
    return false;
}
    

bool Agent::positionFree(size_t x, size_t y, unsigned int timeStep) {
    auto positionEntries = blackboard_->getPositionEntries();

    std::cerr << "(Agent " << (int) num << ") Size blackboard = " << positionEntries.size();

    for(const SearchClient::BlackboardEntry *entry: positionEntries) {
        if(entry->getPosition().x == x && entry->getPosition().y == y) {
            std::cerr << "Refused" << std::endl;
            return false;
        }
    }
    return true;
}
