#include "strategydfs.h"

using Strat::StrategyDFS;

void StrategyDFS::addToFrontierStrategy(SearchEngine::State *state) {
      queue.emplace_back(state);
}

SearchEngine::State *StrategyDFS::getNextLeafStrategy() {
    SearchEngine::State *result = queue.back();
    queue.pop_back();
    return result;
}
