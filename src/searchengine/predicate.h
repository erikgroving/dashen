#ifndef SEARCHCLIENT_PREDICATE_H
#define SEARCHCLIENT_PREDICATE_H

#include "state.h"
#include "../searchclient/agent.h"
#include "../communication/communication"
#include <algorithm>
#include <cmath>

namespace SearchClient {
    class Agent;
}

namespace SearchEngine {
class State;
// Namespace SearchEngine::Predicate
namespace Predicate
{

    /**
    * Check if one can block this field (e.g. goal field, by putting a goal box on it).
    * \return true if one can walk around it freely.
    * 
    * Box is allowed to be at most in contact to one wall and no other goal tiles.
    */
    bool isFieldBlockable(const State *input, int x, int y);
    /**
     * Check if the tile at coordinates (x, y) is free.
     * A tile is free if neither an agent, a box nor a wall is on it.
     * \return true if tile at (x,y) is free in state input.
     */
    bool isFree(const State *input, int x, int y);
    bool isFreeBlackboard(const Communication::Blackboard* bb, const State *input, int x, int y);

    /**
     * TODO: MODIFY THIS DESCRIPTION. GOAL STATE IS GOALS ARE MATCHED, NOT BOXES ALL MATCHED
     * Check if input is a goal state. A state is the goal state if all the boxes are placed on their respective goal.
     * \return true if all the boxes are on their respective goals in state input.
     */
    bool isGoalState(const State *input);

    /**
     * Check if there is a box on tile (x, y).
     * boxIndex's purpose is to contain the box index that can be used to retrieve
     * the box found on (x, y). For instance:
     *
     *     State state;
     *     // ...
     *     int boxIndex;
     *     if( Predicate::boxAt(state, x, y, &boxIndex) ) {
     *       Box myBox = state.getBoxes()[boxIndex];
     *       std::cout << "Found box with color " << myBox.color << std::endl;
     *     }
     * \return true if there is a box on tile (x, y) in state input.
     */
    bool boxAt(const State *input, int x, int y, int *boxIndex = 0) ;

    /**
     * goalIndex serves like boxIndex in Predicate::boxAt.
     * \return true if there is a goal on tile (x, y) in state input.
     */
    bool goalAt(const State *input, int x, int y, int *goalIndex = 0);

    /**
     * agentIndex serves like boxIndex in Predicate::boxAt.
     * \return true if there is an agent on tile (x, y) in state input.
     */
    bool agentAt(const State *input, int x, int y, int *agentIndex = 0);

    bool wallAt(const State *input, int x, int y);
    
    bool boxOnGoal(const State *input, const Box &box);

    bool goalHasCorrectBox(const State* input, const Goal goal);

    bool agentNextToBox(const State* input, const Box &box, const SearchClient::Agent* agentPtr);

    unsigned long distanceToClosestGoal(const State *input, const Box &box, int *goalIndex = nullptr);
    /**
     * \return true if (x, y) is a valid coordinate for state input.
     */
    bool inBound(const State *input, size_t x, size_t y);

    short getCorrectGoals(const SearchEngine::State* state);

    size_t height(const State *input);

    size_t width(const State *input, size_t row);

    bool isAgentNotOnForbiddenPath(const State *input, int agentId, const std::vector<Coord> &forbiddenPath);

    bool isBoxNotOnForbiddenPath(const State *input, int boxId, const std::vector<Coord> &forbiddenPath);
    
    bool isCoordNotOnForbiddenPath(const State *input, Coord coord, const std::vector<Coord> &forbiddenPath);
    
    bool boxIsAtDestination(const State* input, int boxID, Coord target);
};


}


#endif // PREDICATE_H
