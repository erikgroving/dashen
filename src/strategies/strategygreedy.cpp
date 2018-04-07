#include "strategygreedy.h"

using SearchEngine::State;
using Strategy::StrategyGreedy;

template<typename Greedy>
SearchEngine::State *StrategyGreedy<Greedy>::getNextLeafStrategy()
{
    State *result = queue.top();
    queue.pop();
    return result;
}

template<typename Greedy>
void StrategyGreedy<Greedy>::addToFrontierStrategy(SearchEngine::State *leaf)
{
    queue.emplace(leaf);
}
