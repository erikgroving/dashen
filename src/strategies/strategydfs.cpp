#include "strategydfs.h"

using Strategy::StrategyDFS;

void StrategyDFS::addToFrontier(SearchEngine::State *state) {
      queue.emplace_back(state);
      frontierMap.insert({state, 0});
}

SearchEngine::State *StrategyDFS::getAndRemoveLeaf() {
    SearchEngine::State *result = queue.back();
    queue.pop_back();
    frontierMap.erase( frontierMap.find(result) );
    return result;
}
