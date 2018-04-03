#include "agent.h"

using SearchEngine::Agent;
using SearchEngine::SearchClient;

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

Goal& Agent::chooseGoal() {
    Goal result = goalsToAchieve.back();
    goalsToAchieve.pop_back();
    return result;
}

void Agent::search(const Goal &goal) {
    private_initialState = sharedState;
    
    Box toMove;
    for(const Box &box: movableBoxes) {
        if(box.letter != goal.letter)
            continue;
        
        toMove = box;
        break;
    }

    SearchClient searcher(private_initialState);
    searcher.setGoalStatePredicate([&toMove](const State *currentState) {
        return Predicate::boxOnGoal(currentState, toMove);
    });    

    return searcher.search(Strategy::StrategyDFS());
}

void Agent::setSharedState(const State &sharedState) {
    Agent::sharedState = sharedState;
}