#include "strategy.h"
#include <iostream>

using SearchEngine::Strategy;

Strategy::Strategy() {

}

void Strategy::addToExplored( State *leaf ) {
    std::cout << "===================" << std::endl;
    std::cout << "Add to explored" << std::endl;
    explored_.emplace_back( leaf );
    std::cout << "===================" << std::endl;
}

std::size_t Strategy::countExplored() const {
    return  explored_.size();
}

bool Strategy::isExplored(State *leaf) const {
    auto iterator = std::find(explored_.begin(), explored_.end(), leaf);
    
    if(iterator != explored_.end()) {
        std::cout << "(Is explored)";
        return true;
    }
    return false;
}

