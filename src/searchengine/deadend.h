#ifndef DEADEND_H
#define DEADEND_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "typedefs.h"
#include "predicate.h"
#include "state.h"
#include "goalpriority.h"
#include <queue>

namespace SearchEngine {
class State;
class Deadend {
public:

    /* Constructors */
    Deadend() {};
    Deadend(State* input);
    ~Deadend();
    //Deadend(const State &state);

    std::vector<std::vector<bool> > getDeadendMatrix() const {
        return deadendMatrix_;
    }
        std::vector<std::vector<bool> > getDEMatrix() const {
        return deadendMatrix_;
    }

    short int isDeadend(size_t x, size_t y) const;
    short int isDeadend(Coord coord) const;
    
    static void
        computeDeadendFromPositionRecursive(const State *state, std::vector<std::vector<bool> > *result, size_t x, size_t y);

    static std::vector<std::vector<bool> > computeDeadend(const State *state);
private:
    State *inputState_;
    std::vector<std::vector<bool> > deadendMatrix_;

};

}
#endif
