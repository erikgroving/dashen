#ifndef STATE_COMPARE
#define STATE_COMPARE

#include "state.h"
#include "predicate.h"

struct StateCompare {
    bool operator()( SearchEngine::State* const& source, SearchEngine::State* const& compared) const;
};

#endif