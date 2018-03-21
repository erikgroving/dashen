#include "strategygreedy.h"

using SearchEngine::State;
using Strategy::StrategyGreedy;

template<typename Greedy>
SearchEngine::State *StrategyGreedy<Greedy>::getAndRemoveLeaf()
{
    State *result = queue.top();
    queue.pop();
    frontierMap.erase( frontierMap.find(result) );
    return result;
}

template<typename Greedy>
void StrategyGreedy<Greedy>::addToFrontier(SearchEngine::State *leaf)
{
    queue.emplace(leaf);
    frontierMap.insert({leaf, 0});
}
