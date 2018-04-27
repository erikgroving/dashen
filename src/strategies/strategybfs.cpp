#include "strategybfs.h"

using Strat::StrategyBFS;

void StrategyBFS::addToFrontierStrategy(SearchEngine::State *state) {
      queue.push_back(state);
}

SearchEngine::State *StrategyBFS::getNextLeafStrategy() {
    SearchEngine::State *result = queue.front();
    queue.erase(queue.begin());
    return result;
}

void Strat::StrategyBFSMovePriority::doShufflePolicy(std::vector<SearchEngine::State *> &nodes)
{
    std::vector<SearchEngine::State*> moves;
    std::vector<SearchEngine::State*> notMoves;
    for(SearchEngine::State* state: nodes) {
        if(state->getAction().action() == MOVE)
            moves.push_back(state);
        else
            notMoves.push_back(state);
    }

    std::random_shuffle(moves.begin(), moves.end());
    std::random_shuffle(notMoves.begin(), notMoves.end());
    moves.insert(moves.end(), notMoves.begin(), notMoves.end());
    nodes = moves;
}
