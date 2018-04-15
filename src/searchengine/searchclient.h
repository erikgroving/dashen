#ifndef SEARCH_CLIENT_H
#define SEARCH_CLIENT_H

#include <vector>
#include <iostream>
#include <functional>

#include "strategy.h"
#include "state.h"
#include "predicate.h"

void printMap(const SearchEngine::State*);

namespace SearchEngine {

/**
 *  \brief A search client is in charge of supervising the search algorithm.
 */
class SearchCli {

public:
    /**
     * \brief Creates the search client with the given initial state.
     */
    SearchCli(SearchEngine::State *initialState, bool printInitialState = false);
    
    /**
     * Conduct a whole search with the given search strategy.
     * \return a planning from the initial state to the goal state if one exists, 
     *         otherwise an empty vector of State.
     */
    std::vector<SearchEngine::State*> search(Strategy &strategy, int agentIndex);
    void setGoalStatePredicate(const std::function< bool(SearchEngine::State*) > &newPredicate) {
        goalStatePredicate = newPredicate;
    }
    
private:
    State *initialState_;
    std::function< bool(SearchEngine::State*) > goalStatePredicate;
};

};
#endif
