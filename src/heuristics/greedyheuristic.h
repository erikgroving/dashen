#ifndef HEURISTICS_GREEDYHEURISTIC_H
#define HEURISTICS_GREEDYHEURISTIC_H

#include "../searchengine/heuristic.h"
#include <string>

namespace Heur {

class GreedyHeuristic: public SearchEngine::Heuristic
{
public:    
    GreedyHeuristic(Agent::SearchAgent* agentPtr);
    unsigned long f(const SearchEngine::State *state) const;
    std::string name() const;
};

}
#endif // GREEDYHEURISTIC_H
