#include "searchclient.h"
#include <iostream>
#include <string>

using SearchEngine::SearchClient;
using SearchEngine::Strategy;
using SearchEngine::State;


void printMap(SearchEngine::State &state, int height, int width) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int index;
            if(SearchEngine::State::walls[i][j])
                std::cout << "+"; 
            else if(state.boxAt(j, i, &index))
                std::cout << state.getBoxes()[index].letter;
            else if(state.agentAt(j, i, &index))
                std::cout << state.getAgents()[index].num;
            else if(state.goalAt(j, i, &index))
                std::cout << SearchEngine::State::goals[index].letter;
            else
                std::cout << " ";
        }

        std::cout << std::endl;
    }
}

SearchClient::SearchClient(const State &initialState): initialState_(initialState) {
    
}

std::vector<State*> SearchClient::search(Strategy &strategy, int agentIndex) {
    
    std::cerr << "The search is conducted with the Strategy " << strategy.name() << std::endl;

    int iterations = 0;
    strategy.addToFrontier(&initialState_);

    while(true) {
        
        if(strategy.frontierIsEmpty())
            return std::vector<State*>();

        State *leaf = strategy.getAndRemoveLeaf();
        
        std::cout << "Get leaf " << iterations + 1 << "(" << leaf->getAction().toString() << ")";
        if(leaf->getParent())
            std::cout << " - Parent action: " << leaf->getParent()->getAction().toString();
        std::cout << std::endl;

        printMap(*leaf, State::walls.size(), State::walls[0].size());
        
        if(leaf->isGoalState()) {
            return leaf->extractPlan();
        }
        
        strategy.addToExplored(leaf);
        
        std::cout << "Expanded actions:" << std::endl;
        
        std::cout << "===================" << std::endl;
        std::cout << "getExpendedNodes" << std::endl;
        auto vector  = leaf->getExpandedNodes(agentIndex);
        std::cout << "===================" << std::endl;
        for(State *state: vector) {
            std::cout << state->getAction().toString() << "(parent action = " << state->getParent()->getAction().toString();
            if(!strategy.isExplored(state) && !strategy.inFrontier(state)) {
                std::cout << "(Valid)";
                strategy.addToFrontier(state);
            }
            std::cout << std::endl;
        } 

        iterations++;
    } 
}
