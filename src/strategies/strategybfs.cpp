#include "strategybfs.h"

using Strategy::StrategyBFS;

void StrategyBFS::addToFrontier(SearchEngine::State *state) {
      queue.emplace_back(state);
      frontierMap.insert({state, 0});
}

SearchEngine::State *StrategyBFS::getAndRemoveLeaf() {
    SearchEngine::State *result = queue.front();
    queue.erase(queue.begin());
    frontierMap.erase( frontierMap.find(result) );
    return result;
}
