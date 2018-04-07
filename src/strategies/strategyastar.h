#ifndef STRATEGY_STRATEGYASTAR_H
#define STRATEGY_STRATEGYASTAR_H

#include "../searchclient/strategy.h"
#include "../searchclient/heuristic.h"
#include "../heuristics/astartheuristic.h"

#include <queue>

namespace Strategy {

template<typename AStar>
class StrategyAStar : public SearchEngine::Strategy
{
    static_assert(std::is_base_of<Heuristic::AStarHeuristic, AStar>::value, "");

public:
    StrategyAStar(const AStar &heuristic):
        SearchEngine::Strategy(), queue(heuristic) {}
public:
    std::string name() const { return "A*"; }

    SearchEngine::State *getNextLeafStrategy();
    void addToFrontierStrategy(SearchEngine::State *leaf);

private:
    std::priority_queue<
        SearchEngine::State*,
        std::vector<SearchEngine::State*>,
        AStar
    > queue;

    std::unordered_map<SearchEngine::State*, int> frontierMap;
};

}
#endif // STRATEGYASTAR_H
