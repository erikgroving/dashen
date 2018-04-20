#ifndef STATE_COMPARE
#define STATE_COMPARE

#include "state.h"
#include <iostream>

struct StateCompare {
    StateCompare(bool debug = false) :debug_(debug) {}
    bool operator()( SearchEngine::State* const& source, SearchEngine::State* const& compared) const;

    private: 
        bool debug_;
};

struct hashBox {
    std::size_t operator()(Box const& source) const {
        std::size_t prime = 31;
        return prime * ( source.loc.x + source.loc.y ) * source.letter * source.color;
    }

};


struct hashAgentDescription {
    std::size_t operator()(AgentDescription const& source) const {
        std::size_t prime = 31;
        return prime * ( source.loc.x + source.loc.y ) * source.num * source.color;
    }
};


struct hashState {
    std::size_t operator()(SearchEngine::State* const& source) const;
};

#endif