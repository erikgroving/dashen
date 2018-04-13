#ifndef STRATEGY_STRATEGYBFS_H
#define STRATEGY_STRATEGYBFS_H

#include "../searchengine/strategy.h"
namespace Strat {

class StrategyBFS : public SearchEngine::Strategy {

public:

    std::string name() const { return "BFS"; }

    void addToFrontierStrategy(SearchEngine::State *state);
    SearchEngine::State* getNextLeafStrategy();

    private:
      std::vector<SearchEngine::State*> queue; // TODO: Hash map to speed up the proess
};

}
#endif // STRATEGYBFS_H
