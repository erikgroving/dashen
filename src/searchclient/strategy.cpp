#include "strategy.h"

using SearchEngine::Strategy;

Strategy::Strategy() {

}

void Strategy::addToExplored( /* const State &leaf */) {
    /* explored_.insert( std::pair<int, State>(leaf.hash(), leaf)); */
}

std::size_t Strategy::countExplored() const {
    return /* explored_.size(); */ 0;
}

bool Strategy::frontierIsEmpty() const {
    return /* frontier_.isEmpty(); */ true;
}

bool Strategy::isExplored(/* const State &leaf */) const {
    return /* exploredStates_.contains(leaf) */ true;
}

bool Strategy::inFrontier(/* const State &leaf */) const {
    return /* frontier_.contains(leaf) */ true;
}