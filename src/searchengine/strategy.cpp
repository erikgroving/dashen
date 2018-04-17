#include "strategy.h"
#include <iostream>

using SearchEngine::Strategy;

Strategy::Strategy(): exploredMap_(), frontierMap_() {
    additionalCheckPredicate_ = [ ](const State* /*input */) {
        return true;
    };
}
Strategy::~Strategy() { }

void Strategy::addToExplored( State *leaf ) {
    exploredMap_.insert({leaf, 0});
}

std::size_t Strategy::countExplored() const {
    return  exploredMap_.size();
}

bool Strategy::isExplored(State *leaf) const {
    auto ite = exploredMap_.find(leaf); 
    return ite != exploredMap_.end();
}

bool Strategy::inFrontier(State *leaf) const {
    auto ite = frontierMap_.find(leaf); 
    return ite != exploredMap_.end();
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
        frontierMap_.erase( nextState );
    return nextState;
}