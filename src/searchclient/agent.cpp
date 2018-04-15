#include "agent.h"
#include "../strategies/strategies"
using SearchClient::Agent;
using SearchClient::Client;
using namespace SearchEngine::Predicate;

SearchEngine::State *Agent::sharedState;

Agent::Agent(Color color, char num, Coord loc, 
                SearchEngine::Strategy *strategy, SearchClient::Client *client) :
            
            color(color), num(num), loc(loc),
            searchStrategy_(strategy), goalsToAchieve(), movableBoxes(), 
            private_initialState(), client_(client) {

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
    auto *initialSearchState = Agent::sharedState;

    SearchEngine::SearchCli searcher(initialSearchState);
    searcher.setGoalStatePredicate([&goal](const SearchEngine::State *currentState) {
        return goalHasCorrectBox(currentState, goal);
    });    

    return searcher.search(*strategy, (int)num);
}

std::vector<SearchEngine::State*> Agent::searchAllGoals(SearchEngine::Strategy &strategy) {
    auto *initialSearchState = Agent::sharedState;
    
    SearchEngine::SearchCli searcher(initialSearchState);
    searcher.setGoalStatePredicate([](const SearchEngine::State *currentState) {
        return isGoalState(currentState);
    });    

    return searcher.search(strategy, (int)num );
}

void Agent::setSharedState(SearchEngine::State *sharedState) {
    Agent::sharedState = sharedState;
}

void Agent::sendPlan(const std::vector<SearchEngine::State*> &plan) const {
    for(const SearchEngine::State* state: plan)
        client_->setAction(num, state->getAction());
}

void Agent::makeSearch() {
    if(client_->getProblemType() == SearchClient::Client::SingleAgent) {
        sendPlan(searchAllGoals(*searchStrategy_));
    }
    else if(client_->getProblemType() == SearchClient::Client::MultiAgent){
        std::cerr << "MultiAgent problem are not supported yet." << std::endl;
    }
}

SearchEngine::Command Agent::nextMove(SearchClient::Blackboard* b, SearchEngine::State s) {
    /* If plan is empty, need to construct a new plan */
    if (plan_.empty()) {
        bool satisfied = true;
        Goal unsatGoal('-', Coord(-1, -1));
        Goal potGoal('-', Coord(-1, -1));
        Goal search_goal('-', Coord(-1, -1));
        // See if we have any unsatisfied goals
        for (Goal& g : takenGoals) {
            if (!goalHasCorrectBox(&s, g)) {
                satisfied = false;
                unsatGoal = g;
                break;
            }
        }

        if (satisfied) {
            // Identify a suitable goal from the blackboard
            std::vector<SearchClient::Blackboard::Entry> entries = b->getGoalEntries();
            for (auto& entry : entries) {
                if (entry.getType() == SearchClient::Blackboard::Entry::EntryType::GLOBAL_GOAL_ENTRY) {
                    /* Find the goal for that position */
                    Coord entryLoc = entry.getPosition();
                    for (Goal& g : SearchEngine::State::goals) {
                        if (entryLoc == g.loc) {
                            potGoal = g;
                            break;
                        }
                    }
                    if (isEntryDoable(potGoal, s)) {
                        b->removeEntry(entry); 
                        search_goal = potGoal;
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

        // Search to find the answer for the goal
        SearchEngine::Strategy* strat = new Strat::StrategyBFS();
        std::vector<SearchEngine::State*> ans = searchGoal(search_goal, strat); //TODO reflect proper strategy
        // Construct a  plan for the answer
        for (auto a : ans) {
            plan_.push_back(a->getAction());
        }
        delete(strat);
    }

    SearchEngine::Command ret = plan_[0];
    plan_.erase(plan_.begin());
    return ret;
}

bool Agent::isEntryDoable(Goal g, SearchEngine::State& s) {
    // TODO make boxes queryable by color/letter
    for (Box b : s.getBoxes()) {
        if (b.color == color && b.letter == g.letter) {
            return true;
        }
    } 
    return false;
}
    