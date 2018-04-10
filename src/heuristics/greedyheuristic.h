#ifndef HEURISTICS_GREEDYHEURISTIC_H
#define HEURISTICS_GREEDYHEURISTIC_H

#include "../searchengine/searchengine"

namespace Heuristic {

class GreedyHeuristic: public SearchEngine::Heuristic
{

public:
    virtual unsigned long f(const SearchEngine::State *state) const = 0;
    virtual unsigned long heuristic(const SearchEngine::State *state) const {
        return f(state);
    }
};

}
#endif // GREEDYHEURISTIC_H
