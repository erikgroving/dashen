#include "heuristic.h"

using SearchEngine::Heuristic;

Heuristic::Heuristic(SearchClient::Agent *agentPtr): agentPtr_(agentPtr) {

}

bool Heuristic::operator()(const SearchEngine::State *s1, const SearchEngine::State *s2) {
    return  heuristic(s1) >= heuristic(s2);
}
