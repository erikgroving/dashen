#ifndef STRATEGY_STRATEGYGREEDY_H
#define STRATEGY_STRATEGYGREEDY_H

#include "../searchclient/strategy.h"
#include "../searchclient/heuristic.h"
#include "../heuristics/greedyheuristic.h"

#include <queue>
#include <unordered_map>

namespace Strategy {

template<typename Greedy>
class StrategyGreedy: public SearchEngine::Strategy
{
    static_assert(std::is_base_of<Heuristic::GreedyHeuristic, Greedy>::value, "");

public:
    StrategyGreedy(const Greedy &heuristic):
        SearchEngine::Strategy(), queue(heuristic) {}
public:
    std::string name() const { return "Greedy"; }
    void addToFrontierStrategy(SearchEngine::State *leaf);
    SearchEngine::State *getNextLeafStrategy();


private:
    std::priority_queue<
        SearchEngine::State*,
        std::vector<SearchEngine::State*>,
        Greedy
    > queue;

    std::unordered_map<SearchEngine::State*, int> frontierMap;
};

}

#endif // STRATEGYGREEDY_H
