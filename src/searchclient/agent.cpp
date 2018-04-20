#include "agent.h"
#include "../strategies/strategies"
#include "../heuristics/greedyheuristic.h"
#include "../heuristics/astarheuristic.h"
using SearchClient::Agent;
using SearchClient::Client;
using namespace SearchEngine::Predicate;

SearchEngine::State *Agent::sharedState;
unsigned int Agent::sharedTime = 0;

Agent::Agent(Color color, char num, Coord loc, 
                SearchEngine::Strategy *strategy, SearchClient::Blackboard *blackboard) :
            
            color(color), num(num), loc(loc),
            searchStrategy_(strategy), goalsToAchieve(), movableBoxes(), 
            private_initialState(), currentSearchGoal_(nullptr), blackboard_(blackboard), correctGoals_(0) {

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
    
std::vector<SearchEngine::State*> Agent::searchBox(const Box& box, SearchEngine::Strategy* strategy) {
    if(private_initialState != nullptr)
        delete private_initialState;

    private_initialState = new SearchEngine::State(*Agent::sharedState);
    private_initialState->setInitialTimeStep(Agent::sharedTime);

    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([&box, this](const SearchEngine::State *currentState) {
        return agentNextToBox(currentState, box, this);
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
        Goal unsatGoal = Goal();
        Goal search_goal = Goal();
        // See if we have any unsatisfied goals
        for (Goal& g : takenGoals_) {
            if (!goalHasCorrectBox(&s, g)) {
                satisfied = false;
                unsatGoal = g;
                break;
            }
        }

        if (satisfied) {
            search_goal = getGoalFromBlackboard(b, s);
        }
        else {
            search_goal = unsatGoal;
        }

        if (search_goal.letter == '-') {
            return SearchEngine::Command(); // no goals so send a NoOp back
        }

        currentSearchGoal_ = &search_goal;



        correctGoals_ = 0;
        for (Goal& g : SearchEngine::State::goals) {
            if (SearchEngine::Predicate::goalHasCorrectBox(&s, g)) {
                correctGoals_ += 1;
            }
        }
        
        // Search to find the answer for the goal
        std::vector<SearchEngine::State*> ans = std::vector<SearchEngine::State*>();
        Box targBox = s.getBoxes()[search_goal.assignedBoxID];
        SearchEngine::Strategy* strat;


        if (!agentNextToBox(&s, targBox, this)) {
            strat = new Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic>(this);
            strat->setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
                return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, state->getTimeStep());
            });
            ans = searchBox(targBox, strat); //TODO reflect proper strategy
            delete strat;
        }
        else {
            strat = new Strat::StrategyHeuristic<Heur::BoxToGoalAStarHeuristic>(this);
            strat->setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
                return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, state->getTimeStep());
            });
            ans = searchGoal(search_goal, strat);
            delete strat;
        }

        // Construct a  plan for the answer
        for (auto *a : ans) {
            plan_.push_back(a->getAction());
            auto *entry = SearchClient::BlackboardEntry::create(SearchClient::BlackboardEntry::POSITION_ENTRY, *this, blackboard_, a->getTimeStep());
            std::cerr << "Location (" << a->getAgents()[num].loc.x << ", " << a->getAgents()[num].loc.y << ")";
            entry->setPosition(a->getAgents()[num].loc);
        }

    }

    if (plan_.empty()) {
        return SearchEngine::Command();
    }

    SearchEngine::Command ret = plan_[0];
    plan_.erase(plan_.begin());

    SearchClient::BlackboardEntry::revoke(blackboard_->findPositionEntry(sharedTime, num), *this);

    return ret;
}

bool Agent::isEntryDoable(Goal& g, SearchEngine::State& s) {

    Box closestBox = Box();
    unsigned long minDist = 50000;
    for (Box b : s.getBoxes()) {
        if (b.color == color && b.letter == g.letter && !State::takenBoxes[b.id]) {
            unsigned long dist = abs(b.loc.x - g.loc.x) + abs(b.loc.y - g.loc.y);
            if (dist == (unsigned long)-1) dist = 50000;
            if (dist < minDist) {
                minDist = dist;
                closestBox = b;
            }
        }
    }

    if (closestBox.id == -1) {
        return false;
    }

    State::takenBoxes[closestBox.id] = true;
    g.assignedBoxID = closestBox.id;
    return true;
}
    

bool Agent::positionFree(size_t x, size_t y, unsigned int timeStep) {
    auto positionEntries = blackboard_->getPositionEntries();

    std::cerr << "(Agent " << (int) num << ") Size blackboard = " << positionEntries.size();

    for(const SearchClient::BlackboardEntry *entry: positionEntries) {
        if((size_t)entry->getPosition().x == x && (size_t)entry->getPosition().y == y) {
            std::cerr << "Refused" << std::endl;
            return false;
        }
    }
    return true;
}

Goal Agent::getGoalFromBlackboard(SearchClient::Blackboard* b, SearchEngine::State s) {
    // Identify a suitable goal from the blackboard
    std::vector<SearchClient::BlackboardEntry*> entries = b->getGoalEntries();
    for (auto *entry : entries) {
        if (entry->getType() == SearchClient::BlackboardEntry::GLOBAL_GOAL_ENTRY) {
            /* Find the goal for that position */
            Coord entryLoc = entry->getPosition();
            for (Goal& g : SearchEngine::State::goals) {
                if (entryLoc == g.loc && isEntryDoable(g, s)) {
                    SearchClient::BlackboardEntry::accept(entry, *this);
                    takenGoals_.push_back(g);
                    return g;
                }
            }
        }
    }
    return Goal();
}