#include "heuristic.h"

using SearchEngine::Heuristic;

Heuristic::Heuristic() {

}

Heuristic::Heuristic(const SearchClient::Agent* agentPtr) {

}

bool Heuristic::operator()(const SearchEngine::State *s1, const SearchEngine::State *s2) {
    return  heuristic(s1) < heuristic(s2);
}
