#include "strategy.h"
#include <iostream>

using SearchEngine::Strategy;

Strategy::Strategy() {

}

void Strategy::addToExplored( State *leaf ) {
    exploredMap_.insert({leaf, 0});
}

std::size_t Strategy::countExplored() const {
    return  exploredMap_.size();
}

bool Strategy::isExplored(State *leaf) const {
    return exploredMap_.find(leaf) != exploredMap_.end();
}

bool Strategy::inFrontier(State *leaf) const {
    return frontierMap_.find(leaf) != frontierMap_.end();
}

std::size_t Strategy::countFrontier() const {
    return frontierMap_.size();
}

bool Strategy::frontierIsEmpty() const {
    return frontierMap_.empty();
}

void Strategy::addToFrontier(State *leaf) {
    frontierMap_.insert({leaf, 0});
    addToFrontierStrategy(leaf);
}

SearchEngine::State* Strategy::getAndRemoveLeaf() {
    State *nextState = getNextLeafStrategy();
    if(nextState)
        frontierMap_.erase( frontierMap_.find(nextState) );
    return nextState;
}