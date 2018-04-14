#ifndef STRATEGY_STRATEGYGREEDY_H
#define STRATEGY_STRATEGYGREEDY_H

#include <queue>
#include "../searchengine/strategy.h"


namespace Strat {

class StrategyGreedy : public SearchEngine::Strategy {

public:

    std::string name() const { return "GREEDY"; }

    void addToFrontierStrategy(SearchEngine::State *state);
    SearchEngine::State* getNextLeafStrategy();

    private:
      std::vector<SearchEngine::State*> queue;
};

}
#endif // STRATEGYGREEDY_H
