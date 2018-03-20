#include "searchclient.h"

using SearchEngine::SearchClient;
using SearchEngine::Strategy;
using SearchEngine::State;

SearchClient::SearchClient(const State &initialState): initialState_(initialState) {
    
}

std::vector<State> SearchClient::search(Strategy &strategy) {
    
    std::cerr << "The search is conducted with the Strategy " << strategy.name() << std::endl;

    int iterations = 0;
    strategy.addToFrontier(initialState_);

    while(true) {

        if(strategy.frontierIsEmpty())
            return std::vector<State>();

        State leaf = strategy.getAndRemoveLeaf();
        
        if(leaf.isGoalState())
            return leaf.extractPlan();
        
        strategy.addToExplored(leaf);
        
        /*
        for(State state: leaf.getExpandedNodes()) {
            if(!strategy.isExplored(state) && !strategy.inFrontier(n)) {
                strategy.addToFrontier(state);
            }
        } */

        iterations++;
    } 
}