#include "predicate.h"

using SearchEngine::State;
using namespace SearchEngine::Predicate;

bool SearchEngine::Predicate::isGoalState(const State *input) {
    for (const Goal& g : State::goals) {
        bool satisfied = false;
        for (const Box& b : input->getBoxes()) {
            if (g.loc == b.loc && g.letter == b.letter) {
                satisfied = true;
            }
        }
        if (!satisfied) {
            return false;
        }
    }
    return true;
}

bool SearchEngine::Predicate::isFree(const State *input, int x, int y) {
    //std::cerr << "inBound: " << inBound(input, x, y) << " agentAt: " << agentAt(input,x,y) <<
    //        " boxAt" << boxAt(input, x, y) << " wallAt " << wallAt(input, x, y) << std::endl;
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
    std::vector<AgentDescription> agentDescs = input->getAgents();
    for(size_t i = 0; i < agentDescs.size(); i++ ){
        if (agentDescs[i].loc.x == x &&
            agentDescs[i].loc.y == y ) {
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
        if (goal.letter == box.letter && box.loc == goal.loc) {
            return true;
        } 
    }
    return false;
}

bool SearchEngine::Predicate::goalHasCorrectBox(const State* input, const Goal goal) {
    for (const Box &b : input->getBoxes()) {
        if (b.letter == goal.letter && b.loc == goal.loc && b.id == goal.assignedBoxID) {
            return true;
        }
    }
    return false;
}

bool SearchEngine::Predicate::agentNextToBox(
       const State* input, const Box box, const SearchClient::Agent* agentPtr) {
    
    /* can simply subtract the absolute value of the coordinates of the box and agent */
    AgentDescription agent = input->getAgents()[agentPtr->getIndex()];
    unsigned int dist = abs(agent.loc.x - box.loc.x) + abs(agent.loc.y - box.loc.y);
    return dist == 1;
}

bool SearchEngine::Predicate::inBound(const State *input, size_t x, size_t y) {
    return y < input->walls.size() && x < input->walls[y].size(); // if (x, y) is a coordinate representing (col, row), an inversion is necessary.
}

unsigned long SearchEngine::Predicate::distanceToClosestGoal(const State *input, const Box &box, int *goalIndex) {
    if(input->goals.size() == 0)
        return -1;

    unsigned long min = abs(box.loc.x - input->goals[0].loc.x) + 
                        abs(box.loc.y - input->goals[0].loc.y);
    if(goalIndex != nullptr) *goalIndex = 0;

    int i = 0;
    for(const Goal &goal: input->goals) {
        unsigned long min2 = abs(box.loc.x - goal.loc.x) + 
                             abs(box.loc.y - goal.loc.y);

        if(min2 < min) {
            min = min2;
            if(goalIndex != nullptr) *goalIndex = i;
        }
        i++;
    }
    return min;

    
}