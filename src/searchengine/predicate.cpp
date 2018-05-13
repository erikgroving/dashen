#include "predicate.h"

using SearchEngine::State;
using namespace SearchEngine::Predicate;

bool SearchEngine::Predicate::isFieldBlockable(const State *input, int x, int y) {
    int wallAndGoalCount = 0;
    
    Coord c = Coord(x, y);
    Coord coordN = Coord(c.x - 1, c.y);
    Coord coordE = Coord(c.x, c.y + 1);
    Coord coordS = Coord(c.x + 1, c.y);
    Coord coordW = Coord(c.x, c.y - 1);

    Coord coordNE = Coord(c.x - 1, c.y + 1);
    Coord coordSE = Coord(c.x + 1, c.y + 1);
    Coord coordSW = Coord(c.x + 1, c.y - 1);
    Coord coordNW = Coord(c.x - 1, c.y - 1);
    
    std::vector< Coord > vecN = {coordSW, coordSE};
    std::vector< Coord > vecE = {coordNW, coordSW};
    std::vector< Coord > vecS = {coordNW, coordNE};
    std::vector< Coord > vecW = {coordNE, coordSE};

    std::vector< Coord > vecAll = {coordN, coordNE, coordE, coordSE, coordS, coordSW, coordW, coordNW};
    
    c = coordN;
    if (wallAt(input, c.x, c.y) || goalAt(input, c.x, c.y)) {
        wallAndGoalCount++;
        for (Coord diagCoord : vecN) {
            if (wallAt(input, diagCoord.x, diagCoord.y) || goalAt(input, diagCoord.x, diagCoord.y)) {
                return false;
            }
        }
    } 
    c = coordE;
    if (wallAt(input, c.x, c.y) || goalAt(input, c.x, c.y)) {
        wallAndGoalCount++;
        for (Coord diagCoord : vecE) {
            if (wallAt(input, diagCoord.x, diagCoord.y) || goalAt(input, diagCoord.x, diagCoord.y)) {
                return false;
            }
        }
    }
    c = coordS;
    if (wallAt(input, c.x, c.y) || goalAt(input, c.x, c.y)) {
        wallAndGoalCount++;
        for (Coord diagCoord : vecS) {
            if (wallAt(input, diagCoord.x, diagCoord.y) || goalAt(input, diagCoord.x, diagCoord.y)) {
                return false;
            }
        }
    }
    c = coordW;
    if (wallAt(input, c.x, c.y) || goalAt(input, c.x, c.y)) {
        wallAndGoalCount++;
        for (Coord diagCoord : vecW) {
            if (wallAt(input, diagCoord.x, diagCoord.y) || goalAt(input, diagCoord.x, diagCoord.y)) {
                return false;
            }
        }
    }

    if (wallAndGoalCount > 1) {
        return false;
    } 
    // check diagonal fields
    else if (wallAndGoalCount == 0) {
        for (Coord anyCoord : vecAll) {
            if (wallAt(input, anyCoord.x, anyCoord.y) || goalAt(input, anyCoord.x, anyCoord.y)) {
                wallAndGoalCount++;
            }
        }
        // return false if more than one diagonal field is blocked off
        if (wallAndGoalCount > 1) {
            return false;
        }
    }
    return true;
}


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

bool SearchEngine::Predicate::isFreeBlackboard(const Communication::Blackboard* bb, const State *input, int x, int y) {
    return isFree(input, x, y);
    /*if (!inBound(input, x, y) || wallAt(input, x, y)) {
        return false;
    }
    if (bb == nullptr) {
        return true;
    }

    Coord check = Coord(x, y);
    unsigned int timeStep = input->getTimeStep();
    for (auto agentPosVec : bb->getPositionEntries()) {
        if (agentPosVec.size() == 1) {
            if (agentPosVec[0]->getLocation() == check) {
                return false;
            }       
        }
        else {
            for (auto entry : agentPosVec) {
                if (entry->getLocation() == check) {
                    int timeDiff = timeStep - entry->getTimeStep();
                    if (abs(timeDiff) <= 1) {
                        return false;
                    }
                }
            }
        }
    }
    
    for (auto boxPosVec : bb->getBoxEntries()) {
        if (boxPosVec.size() == 1) {
            if (boxPosVec[0]->getLocation() == check) {
                return false;
            }       
        }
        else {
            for (auto entry : boxPosVec) {
                if (entry->getLocation() == check) {
                    int timeDiff = timeStep - entry->getTimeStep();
                    if (abs(timeDiff) <= 1) {
                        return false;
                    }
                }
            }
        }
    }
    return true;*/
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
       const State* input, const Box &box, const SearchClient::Agent* agentPtr) {
    
    /* can simply subtract the absolute value of the coordinates of the box and agent */
    AgentDescription agent = input->getAgents()[agentPtr->getIndex()];
    return Coord::distance(agent.loc, box.loc) == 1;
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

short SearchEngine::Predicate::getCorrectGoals(const SearchEngine::State* state) {
    short correctGoals = 0;
    for (const Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            correctGoals += 1;
        }
    }
    return correctGoals;
}

size_t SearchEngine::Predicate::height(const SearchEngine::State *input)
{
    return input->walls.size();
}

size_t SearchEngine::Predicate::width(const SearchEngine::State *input, size_t row)
{
    if(inBound(input, 0, row))
        return input->walls[row].size();
    return -1;
}

bool SearchEngine::Predicate::isAgentNotOnForbiddenPath(const SearchEngine::State *input, int agentId, const std::vector<Coord> &forbiddenPath)
{
    Coord agentLoc = input->getAgents()[agentId].loc;
    for(const Coord& tile: forbiddenPath) {
        if(agentLoc == tile)
            return false;
    }
    return true;
}

bool SearchEngine::Predicate::boxIsAtDestination(const SearchEngine::State* input, int boxID, Coord target) {
    return input->getBoxes()[boxID].loc == target;
}
    
bool SearchEngine::Predicate::isCoordNotOnForbiddenPath(const State *input, Coord coord, const std::vector<Coord> &forbiddenPath) {
    for (const Coord& c : forbiddenPath) {
        if (c == coord) {
            return false;
        }
    }
    return true;
}

bool SearchEngine::Predicate::isBoxNotOnForbiddenPath(const SearchEngine::State *input, int boxId, const std::vector<Coord> &forbiddenPath)
{
    if (boxId == -1) { return true; }
    Coord boxLoc = input->getBoxes()[boxId].loc;
    for(const Coord& tile: forbiddenPath) {
        if(boxLoc == tile)
            return false;
    }
    return true;
}
