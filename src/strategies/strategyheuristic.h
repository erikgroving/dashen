#ifndef STRATEGY_STRATEGYGREEDY_H
#define STRATEGY_STRATEGYGREEDY_H

#include <queue>
#include <unordered_map>

#include "../searchengine/strategy.h"
#include "../searchengine/heuristic.h"

namespace Strategy {

template<typename Heuristic>
class StrategyHeuristic: public SearchEngine::Strategy
{
public:
    StrategyHeuristic();
public:
    std::string name() const { return "Heuristic Strategy with " + heuristicName; }
    void addToFrontierStrategy(SearchEngine::State *leaf);
    SearchEngine::State *getNextLeafStrategy();

private:
    std::string heuristicName;
    std::priority_queue<
        SearchEngine::State*,
        std::vector<SearchEngine::State*>,
        Heuristic
    > queue;

    std::unordered_map<SearchEngine::State*, int> frontierMap;
};

}

#include "strategyheuristic.tpp"

#endif // STRATEGYGREEDY_H
