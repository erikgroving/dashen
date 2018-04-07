#include "predicate.h"

using SearchEngine::State;
using namespace SearchEngine::Predicate;

bool SearchEngine::Predicate::isGoalState(const State *input) {
    for(const Box &box: input->getBoxes()) {
        // Find if the goal is on the right location
        if(!boxOnGoal(input, box))
            return false;
    }
    return true;
}

bool SearchEngine::Predicate::isFree(const State *input, int x, int y) {

    return inBound(input, x, y)  &&
           !agentAt(input, x, y) &&
           !boxAt(input, x, y)   &&
           !wallAt(input, x, y);
}

bool SearchEngine::Predicate::boxAt(const State *input, int x, int y, int *boxIndex){

    for(size_t i = 0; i < input->getBoxes().size(); i++ ){

        if ( input->getBoxes()[i].loc.x == x &&
             input->getBoxes()[i].loc.y == y ) {

            if(boxIndex > 0) *boxIndex = i;
            return true;
        }
    }

    return false;
}

bool SearchEngine::Predicate::goalAt(const State *input, int x, int y, int *goalIndex) {
     for(size_t i = 0; i < input->goals.size(); i++ ){
         if (input->goals[i].loc.x == x &&
             input->goals[i].loc.y == y ) {

             if(goalIndex > 0) *goalIndex = i;
             return true;
         }
     }

     return false;
}


bool SearchEngine::Predicate::agentAt(const State *input, int x, int y, int *agentIndex) {
    for(size_t i = 0; i < input->getAgents().size(); i++ ){
         if (input->getAgents()[i].loc.x == x &&
             input->getAgents()[i].loc.y == y ) {

             if(agentIndex > 0) *agentIndex = i;
             return true;
         }
     }
     return false;
}

bool SearchEngine::Predicate::wallAt(const State *input, int x, int y) {
    return inBound(input, x, y) && State::walls[y][x]; // if (x, y) is a coordinate representing (col, row), an inversion is necessary.
}
bool SearchEngine::Predicate::boxOnGoal(const State *input, const Box &box) {
    for(const Goal &goal: input->goals) {
        
        if(goal.letter != box.letter) continue;    
        if(box.loc.x != goal.loc.x || box.loc.y != goal.loc.y) continue;

        return true;
    }
    return false;
}

bool SearchEngine::Predicate::inBound(const State *input, int x, int y) {
    return y < input->walls.size() && x < input->walls[y].size(); // if (x, y) is a coordinate representing (col, row), an inversion is necessary.
}
