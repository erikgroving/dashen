
#include "deadend.h"

using SearchEngine::State;
using SearchEngine::Deadend;
using namespace SearchEngine::GoalPriorityComputation;
using namespace SearchEngine::Predicate;

Deadend::Deadend(const SearchEngine::State &input): inputState_(input), deadendMatrix_() {
    deadendMatrix_ = Deadend::computeDeadend(input);
}

SearchEngine::Deadend &Deadend::operator=(const SearchEngine::Deadend &src)
{
    inputState_ = src.inputState_;
    deadendMatrix_ = src.deadendMatrix_;
    return *this;
}

Deadend::Deadend(const SearchEngine::Deadend &src): inputState_(src.inputState_), deadendMatrix_(src.deadendMatrix_)
{

}

Deadend::~Deadend() {

}


short int Deadend::isDeadend(size_t x, size_t y) const {
        if(!SearchEngine::Predicate::inBound(&inputState_, x, y) )
           return -1;

        return deadendMatrix_[y][x];
}

short int Deadend::isDeadend(Coord coord) const {
    return Deadend::isDeadend(coord.x, coord.y);
}

void Deadend::computeDeadendFromPositionRecursive(const State &state, std::vector<std::vector<bool> > *result, size_t x, size_t y) {
    if(!inBound(&state, x, y)) return;
    if((*result)[y][x] == true) return;
    if(wallAt(&state,x,y)) return;
    if(getSurroundingWalls(state, Coord(x,y)) < 2) return;

    (*result)[y][x] = true;
    if(inBound(&state, x + 1, y))
        computeDeadendFromPositionRecursive(state, result, x + 1, y);
    if(inBound(&state, x - 1, y))
        computeDeadendFromPositionRecursive(state, result, x - 1, y);
    if(inBound(&state, x, y + 1))
        computeDeadendFromPositionRecursive(state, result, x, y + 1);
    if(inBound(&state, x, y - 1))
        computeDeadendFromPositionRecursive(state, result, x, y - 1);
}

std::vector<std::vector<bool> >
Deadend::computeDeadend(const State &state) {

    // Initialization
    std::vector<std::vector<bool > > result;

     for (size_t y=0; y<state.walls.size(); y++) {
        result.push_back(std::vector<bool> (state.walls[y].size()));
        for (size_t x=0; x<state.walls[y].size(); x++) {
            result[y][x] = false;
        }
    }

     for (size_t y=0; y<state.walls.size(); y++) {
        for (size_t x=0; x<state.walls[y].size(); x++) {

            if(getSurroundingWalls(state, Coord(x,y)) >= 3)
                Deadend::computeDeadendFromPositionRecursive(state, &result, x, y);
        }
     }
     return result;
}

