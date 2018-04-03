#include "agent.h"

using SearchEngine::Agent;
using SearchEngine::SearchClient;
using namespace SearchEngine::Predicate;

SearchEngine::State *Agent::sharedState;

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

std::vector<SearchEngine::State*> Agent::search(const Goal &goal) {
    private_initialState = Agent::sharedState;
    
    Box toMove;
    for(const Box &box: movableBoxes) {
        if(box.letter != goal.letter)
            continue;
        
        toMove = box;
        break;
    }

    SearchClient searcher(private_initialState);
    searcher.setGoalStatePredicate([&toMove](const SearchEngine::State *currentState) {
        return boxOnGoal(currentState, toMove);
    });    

    ::Strategy::StrategyDFS strat;
    return searcher.search(strat,(int)num);
}

void Agent::setSharedState(SearchEngine::State *sharedState) {
    Agent::sharedState = sharedState;
}
