#include "searchclient.h"
#include <iostream>
#include <string>

using SearchEngine::SearchClient;
using SearchEngine::Strategy;
using SearchEngine::State;
using namespace SearchEngine::Predicate;


void printMap(const State *state, int height, int width) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int index;
            if(State::walls[i][j])
                std::cerr << "+"; 
            else if(agentAt(state, j, i, &index))
                std::cerr << state->getAgents()[index].num;
            else if(boxAt(state, j, i, &index))
                std::cerr << state->getBoxes()[index].letter;
            else if(goalAt(state, j, i, &index))
                std::cerr << SearchEngine::State::goals[index].letter;
            else
                std::cerr << " ";
        }

        std::cerr << std::endl;
    }
}

SearchClient::SearchClient(State *initialState): initialState_(initialState) {
    
}

std::vector<State*> SearchClient::search(SearchEngine::Strategy &strategy, int agentIndex) {
    
    std::cerr << "The search is conducted with the Strategy " << strategy.name() << std::endl;

    int iterations = 0;
    strategy.addToFrontier(initialState_);

    while(true) {
        
        if(strategy.frontierIsEmpty())
            return std::vector<State*>();

        State *leaf = strategy.getAndRemoveLeaf();
        
        std::cerr << "Get leaf " << iterations + 1 << "(" << leaf->getAction().toString() << ")";
        if(leaf->getParent())
            std::cerr << " - Parent action: " << leaf->getParent()->getAction().toString();
        std::cerr << std::endl;

        printMap(leaf, State::walls.size(), State::walls[0].size());
        
        if(goalStatePredicate(leaf)) {
            return leaf->extractPlan();
        }
        
        strategy.addToExplored(leaf);
        
        std::cerr << "Expanded actions:" << std::endl;
        
        std::cerr << "===================" << std::endl;
        std::cerr << "getExpendedNodes" << std::endl;
        auto vector  = leaf->getExpandedNodes(agentIndex);
        std::cerr << "===================" << std::endl;

        for(State *state: vector) {
            std::cerr << state->getAction().toString() << "(parent action = " << state->getParent()->getAction().toString();
            if(!strategy.isExplored(state) && !strategy.inFrontier(state)) {
                std::cerr << "(Valid)";
                strategy.addToFrontier(state);
            }
            std::cerr << std::endl;
        } 

        iterations++;
    } 
}
