#ifndef STRATEGY_STRATEGYHEURISTIC_H
#define STRATEGY_STRATEGYHEURISTIC_H

#include <queue>
#include <unordered_map>

#include "../searchengine/strategy.h"
#include "../searchengine/heuristic.h"
#include "../searchclient/searchclient"
namespace Strat {

template<typename Heuristic>
class StrategyHeuristic: public SearchEngine::Strategy
{
public:
    StrategyHeuristic(const SearchClient::Agent* agentPtr);
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

#endif // STRATEGYHEURISTIC_H
