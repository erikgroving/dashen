#ifndef STRATEGY_STRATEGYDFS_H
#define STRATEGY_STRATEGYDFS_H

#include "../searchengine/strategy.h"
#include "../searchengine/state.h"

namespace Strategy {

class StrategyDFS : public SearchEngine::Strategy {

public:

    std::string name() const { return "DFS"; }
    
    void addToFrontierStrategy(SearchEngine::State *state);
    SearchEngine::State* getNextLeafStrategy();

    private:
      std::vector<SearchEngine::State*> queue; // TODO: Hash map to speed up the proess
};

}
#endif // STRATEGYDFS_H
