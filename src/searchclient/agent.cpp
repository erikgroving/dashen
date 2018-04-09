#include "agent.h"

using SearchClient::Agent;
using SearchEngine::SearchClient;
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

std::vector<SearchEngine::State*> Agent::searchGoal(const Goal &goal, SearchEngine::Strategy &strategy) {
    auto *initialSearchState = Agent::sharedState;
    
    Box toMove;
    for(const Box &box: movableBoxes) {
        if(box.letter != goal.letter)
            continue;
        
        toMove = box;
        break;
    }

    SearchEngine::SearchClient searcher(initialSearchState);
    searcher.setGoalStatePredicate([&toMove](const SearchEngine::State *currentState) {
        return boxOnGoal(currentState, toMove);
    });    

    return searcher.search(strategy, (int)num);
}

std::vector<SearchEngine::State*> Agent::searchAllGoals(SearchEngine::Strategy &strategy) {
    auto *initialSearchState = Agent::sharedState;
    
    SearchEngine::SearchClient searcher(initialSearchState);
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