#include "strategygreedy.h"

using Strat::StrategyGreedy;

void StrategyGreedy::addToFrontierStrategy(SearchEngine::State *state) {
      queue.push_back(state);
}

SearchEngine::State *StrategyGreedy::getNextLeafStrategy() {
    SearchEngine::State *result = queue.front();
    queue.erase(queue.begin());
    return result;
}
