#include "strategyheuristic.h"

using SearchEngine::State;
using Strategy::StrategyHeuristic;

template<typename Heuristic>
StrategyHeuristic<Heuristic>::StrategyHeuristic(): SearchEngine::Strategy(), queue(Heuristic()) {
    static_assert(std::is_base_of<SearchEngine::Heuristic, Heuristic>::value, "");
}

template<typename Heuristic>
SearchEngine::State *StrategyHeuristic<Heuristic>::getNextLeafStrategy()
{
    State *result = queue.top();
    queue.pop();
    return result;
}

template<typename Heuristic>
void StrategyHeuristic<Heuristic>::addToFrontierStrategy(SearchEngine::State *leaf)
{
    queue.emplace(leaf);
}
