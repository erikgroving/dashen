#include "strategy.h"
#include <iostream>

using SearchEngine::Strategy;

Strategy::Strategy() {

}

void Strategy::addToExplored( State *leaf ) {
    std::cerr << "===================" << std::endl;
    std::cerr << "Add to explored" << std::endl;
    explored_.emplace_back( leaf );
    exploredMap_.insert({leaf, 0});
    std::cerr << "===================" << std::endl;
}

std::size_t Strategy::countExplored() const {
    return  explored_.size();
}

bool Strategy::isExplored(State *leaf) const {
    auto iterator = std::find(explored_.begin(), explored_.end(), leaf);
    
    if(iterator != explored_.end()) {
        std::cerr << "(Is explored)";
        return true;
    }
    return false;
}

