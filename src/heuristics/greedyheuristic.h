#ifndef HEURISTICS_GREEDYHEURISTIC_H
#define HEURISTICS_GREEDYHEURISTIC_H

#include "../searchengine/searchengine"
#include "../searchclient/searchclient"
#include <string>
namespace Heur {

class GreedyHeuristic: public SearchEngine::Heuristic
{
public:    
    GreedyHeuristic(SearchClient::Agent* agentPtr);
    unsigned long f(const SearchEngine::State *state) const;
    std::string name() const;
};

}
#endif // GREEDYHEURISTIC_H
