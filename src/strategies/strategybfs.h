#ifndef STRATEGY_STRATEGYBFS_H
#define STRATEGY_STRATEGYBFS_H

#include "../searchclient/strategy.h"
namespace Strategy {

class StrategyBFS : public SearchEngine::Strategy {

public:

    std::string name() const { return "BFS"; }

    void addToFrontierStrategy(SearchEngine::State *state);
    SearchEngine::State* getNextLeafStrategy();

    private:
      std::vector<SearchEngine::State*> queue; // TODO: Hash map to speed up the proess
      std::unordered_map<SearchEngine::State*, int, std::hash<SearchEngine::State*>, StateCompare> frontierMap;
};

}
#endif // STRATEGYBFS_H
