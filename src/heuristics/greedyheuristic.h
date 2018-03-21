#ifndef GREEDYHEURISTIC_H
#define GREEDYHEURISTIC_H

#include "src/searchclient/heuristic.h"

namespace Heuristic {

class GreedyHeuristic: public SearchEngine::Heuristic
{

public:
    virtual unsigned long f(const SearchEngine::State *state) const = 0;
    unsigned long heuristic(const SearchEngine::State *state) const {
        return f(state);
    }
};

}
#endif // GREEDYHEURISTIC_H
