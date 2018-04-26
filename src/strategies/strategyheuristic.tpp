#include "strategyheuristic.h"

using SearchEngine::State;
using Strat::StrategyHeuristic;

template<typename Heuristic>
StrategyHeuristic<Heuristic>::StrategyHeuristic(SearchClient::Agent* agentPtr): SearchEngine::Strategy(), queue(Heuristic(agentPtr)) {
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
