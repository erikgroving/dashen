#ifndef SEARCH_CLIENT_H
#define SEARCH_CLIENT_H

#include <vector>
#include <iostream>

#include "strategy.h"
#include "state.h"

namespace SearchEngine {

/**
 *  \brief A search client is in charge of supervising the search algorithm.
 */
class SearchClient {

public:
    /**
     * \brief Creates the search client with the given initial state.
     */
    SearchClient( const State &initialState );
    
    /**
     * Conduct a whole search with the given search strategy.
     * \return a planning from the initial state to the goal state if one exists, 
     *         otherwise an empty vector of State.
     */
    std::vector<State*> search(Strategy &strategy, int agentIndex);
    
private:
    State initialState_;
};

};
#endif
