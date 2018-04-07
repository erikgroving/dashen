#include "strategyastar.h"

using SearchEngine::State;
using Strategy::StrategyAStar;

template<typename AStar>
SearchEngine::State *StrategyAStar<AStar>::getNextLeafStrategy()
{
    State *result = queue.top();
    queue.pop();
    frontierMap.erase( frontierMap.find(result) );
    return result;
}

template<typename AStar>
void StrategyAStar<AStar>::addToFrontierStrategy(SearchEngine::State *leaf)
{
    queue.emplace(leaf);
    frontierMap.insert({leaf, 0});
}
