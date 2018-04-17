#ifndef PRINTMAP
#define PRINTMAP

#include <iostream>
#include "searchengine/searchengine"

void printMap(SearchEngine::State *state) {
    using namespace SearchEngine::Predicate;
    for(int i = 0; inBound(state, 0, i); i++) {
        for(int j = 0; inBound(state, j, i); j++) {
            int index;
            if(wallAt(state, j, i))
                std::cerr << "+"; 
            else if(agentAt(state, j, i, &index))
                std::cerr << '0' + (state->getAgents()[index].num - 96);
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

void printDistances(std::vector<std::vector<short int> >  dist){
    for (const auto& inner : dist) {
        for (auto value : inner) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
}


#endif