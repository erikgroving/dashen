#ifndef STRATEGYGREEDY_H
#define STRATEGYGREEDY_H

#include "src/searchclient/strategy.h"
#include "src/searchclient/heuristic.h"
#include "src/heuristics/greedyheuristic.h"

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
    bool frontierIsEmpty() const { return queue.empty(); }
    std::size_t countFrontier() const { return queue.size(); }

    bool inFrontier(SearchEngine::State *leaf) const {
        return frontierMap.find(leaf) != frontierMap.end();
    }

    void addToFrontier(SearchEngine::State *leaf);
    SearchEngine::State *getAndRemoveLeaf();


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
