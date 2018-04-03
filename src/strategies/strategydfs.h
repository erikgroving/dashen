#ifndef STRATEGYDFS_H
#define STRATEGYDFS_H

#include "../searchclient/strategy.h"

namespace Strategy {

class StrategyDFS : public SearchEngine::Strategy {

public:

    std::string name() const { return "DFS"; }
    std::size_t countFrontier() const { return queue.size(); }
    bool frontierIsEmpty() const { return queue.size() == 0; }
    bool inFrontier(SearchEngine::State *state) const {
      return frontierMap.find(state) != frontierMap.end();
    }

    void addToFrontier(SearchEngine::State *state);
    SearchEngine::State* getAndRemoveLeaf();

    private:
      std::vector<SearchEngine::State*> queue; // TODO: Hash map to speed up the proess
      std::unordered_map<SearchEngine::State*, int> frontierMap;
};

}
#endif // STRATEGYDFS_H
