#include "heuristic.h"

using SearchEngine::Heuristic;

Heuristic::Heuristic() {

}

bool Heuristic::operator()( /* const State &s1, const State &s2 */) {
    return /* ( heuristic(s1) - heuristic(s2) ) >= 0 */  false;
}