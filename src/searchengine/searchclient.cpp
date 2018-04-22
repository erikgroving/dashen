#include "searchclient.h"
#include <iostream>
#include <string>

using SearchEngine::SearchCli;
using SearchEngine::Strategy;
using SearchEngine::State;
using namespace SearchEngine::Predicate;


void printMap(const State *state) {
    for(int i = 0; inBound(state, 0, i); i++) {
        for(int j = 0; inBound(state, j, i); j++) {
            int index;
            if(wallAt(state, j, i))
                std::cerr << "+"; 
            else if(agentAt(state, j, i, &index))
                std::cerr << (char)(state->getAgents()[index].num + '0');
            else if(boxAt(state, j, i, &index))
                std::cerr << state->getBoxes()[index].letter;
            else if(goalAt(state, j, i, &index))
                std::cerr << (char)(SearchEngine::State::goals[index].letter - 'A' + 'a');
            else
                std::cerr << " ";
        }
        std::cerr << std::endl;
    }
}

SearchCli::SearchCli(State *initialState, bool printInitialState): initialState_(initialState) {
    if(printInitialState) {
        std::cerr << "Initial state:" << std::endl;

        std::cerr << "Boxes: " << initialState->getBoxes().size() << std::endl;
        std::cerr << "Goals: " << initialState->goals.size() << std::endl;
        std::cerr << "Agents: " << initialState->getAgents().size() << std::endl;
        for(AgentDescription &agent: initialState->getAgents())
            std::cerr << agent.num << "(" << agent.loc.x << ", " << agent.loc.y << ")" << std::endl;
        printMap(initialState);
    }

    goalStatePredicate = Predicate::isGoalState;
        
}

std::vector<State*> SearchCli::search(SearchEngine::Strategy &strategy, int agentIndex) {
    
    std::cerr << "The search is conducted with the Strategy " << strategy.name() << std::endl;

    int iterations = 0;
    strategy.addToFrontier(initialState_);
    while(true) {
        
        if(strategy.frontierIsEmpty()) {
            return std::vector<State*>();
        }

        State *leaf = strategy.getAndRemoveLeaf();
        
        if(iterations % 10000 == 0) {
            std::cerr << "Iteration " << iterations + 1 << ", Explored: " << strategy.countExplored() << ", Frontier: " << strategy.countFrontier() << std::endl;
            printMap(leaf);
        }
        
                           
        if(goalStatePredicate(leaf)) {
            return leaf->extractPlan();
        }
        
        strategy.addToExplored(leaf);
        
  /*      std::cerr << "Expanded actions:" << std::endl;
        
        std::cerr << "===================" << std::endl;
        std::cerr << "getExpendedNodes" << std::endl;*/
        auto expandedNodes = leaf->getExpandedNodes(agentIndex);
        /*std::cerr << "===================" << std::endl;*/
        for(State *state: expandedNodes) {

             //std::cerr << state->getAction().toString() << "(parent action = " << state->getParent()->getAction().toString() << ")";
            if(!strategy.isExplored(state) && !strategy.inFrontier(state) && strategy.additionalCheckPredicate()(state)) {           
               //  std::cerr << "(Valid)";
                strategy.addToFrontier(state);
            }
             //std::cerr << std::endl;
        } 

        iterations++;
    } 
}
