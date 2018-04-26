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
