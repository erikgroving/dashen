#ifndef STRATEGYASTAR_H
#define STRATEGYASTAR_H

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
    bool inFrontier(SearchEngine::State *leaf) const {
        return frontierMap.find(leaf) != frontierMap.end();
    }
    bool frontierIsEmpty() const { return frontierMap.empty(); }
    std::size_t countFrontier() const { return frontierMap.size(); }

    SearchEngine::State *getAndRemoveLeaf();
    void addToFrontier(SearchEngine::State *leaf);

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
