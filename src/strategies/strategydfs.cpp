#include "strategydfs.h"

using Strategy::StrategyDFS;

void StrategyDFS::addToFrontierStrategy(SearchEngine::State *state) {
      queue.emplace_back(state);
}

SearchEngine::State *StrategyDFS::getNextLeafStrategy() {
    SearchEngine::State *result = queue.back();
    queue.pop_back();
    return result;
}
