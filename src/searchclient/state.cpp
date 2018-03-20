#include "state.h"
#include <iostream>

using SearchEngine::State;
using SearchEngine::Command;

static int id = 0;
std::vector<std::vector<bool>> State::walls = std::vector< std::vector<bool> >();
std::vector<Goal> State::goals = std::vector< Goal >();
int State::numAgents = 0;

State::State(): parent(0), children(),  agents(), boxes(), action() {
    std::cout << "State with no parent";
    idx = id++;
}

std::vector<State*> State::getExpandedNodes(int agentIndex) {
    std::vector<State*> result;
    
    Agent &agt = agents[agentIndex];

    for(const Command &cmd: Command::possibleActions) {
        // std::cout << cmd.toString() << std::endl;
         int newAgentRow = agt.loc.x + Command::rowToInt(cmd.d1);
         int newAgentCol = agt.loc.y + Command::colToInt(cmd.d1);
        
        if(cmd.action == Action::MOVE) {
            std::cout << "Agent could move to (" << newAgentRow << "," << newAgentCol << ") (Command " << cmd.toString() << ") ?";

            if( isFree(newAgentRow, newAgentCol) ) {

                State *childNode = makeChild();
                childNode->setAction(cmd);
                childNode->getAgents()[agentIndex].loc.x = newAgentRow;
                childNode->getAgents()[agentIndex].loc.y = newAgentCol;

                result.push_back(childNode);
                std::cout << "YES";
            }
            else
                std::cout << "NO";
            std::cout << std::endl; 
        }
        else if( cmd.action == Action::PUSH ) {

            std::cout << "Agent could push box at (" << newAgentRow << "," << newAgentCol << ") (Command " << cmd.toString() << ") ?";

            int boxIndex;
            if( boxAt(newAgentRow, newAgentCol, &boxIndex) ) {
                int newBoxRow = newAgentRow + Command::rowToInt(cmd.d2);
                int newBoxCol = newAgentCol + Command::colToInt(cmd.d2);

                if( isFree(newBoxRow, newBoxCol) ) {
                    State *childNode = makeChild();

                    childNode->setAction(cmd);
                    childNode->getAgents()[agentIndex].loc.x = newAgentRow;
                    childNode->getAgents()[agentIndex].loc.y = newAgentCol;

                    childNode->getBoxes()[boxIndex].loc.x = newBoxRow;
                    childNode->getBoxes()[boxIndex].loc.y = newBoxCol;

                    result.push_back(childNode);
                }
                else 
                    std::cout << "(NO cause the box cannot be pushed)";
            }
            else
                std::cout << "(NO cause it does not exist)";

            std::cout << std::endl;
        }
        else if( cmd.action == Action::PULL ) {
            int boxIndex;
            if( isFree(newAgentRow, newAgentCol)) {
                int boxRow = agt.loc.x + Command::rowToInt(cmd.d2);
                int boxCol = agt.loc.y + Command::colToInt(cmd.d2);

                if(boxAt(boxRow, boxCol, &boxIndex)) {
                    State *childNode = makeChild();
                    childNode->setAction(cmd);
                    childNode->getAgents()[agentIndex].loc.x = newAgentRow;
                    childNode->getAgents()[agentIndex].loc.y = newAgentCol;

                    childNode->getBoxes()[boxIndex].loc.x = boxRow;
                    childNode->getBoxes()[boxIndex].loc.y = boxCol;

                    result.push_back(childNode);
                }
            }
        }        


    }
    return result;
}

std::vector<State*> State::extractPlan() {

    std::cout << "Extracting plan" << std::endl;

    std::vector<State*> result;
    result.push_back(this);

    std::cout << "Look at parents" << std::endl;

    State *parentState = parent;

    if(parent == this)
        std::cout << "I am my own parent" << std::endl;

    while(parentState != NULL) {
        // std::cout << parentState;
        result.insert(result.begin(), parentState);
        parentState = parentState->parent;
    }

    std::cout << "Done";
    return result;
}

State::State(const State &src): agents(src.agents), boxes(src.boxes), action(src.action) {
    std::cout << "Copy from " << src.idx << std::endl;
    setParent(*src.parent);

    if(this == parent)
        std::cout << "I am my own parent --" << std::endl;
    idx = id++;
}

bool State::operator==(const SearchEngine::State &compared) const {
    for(int i = 0; i < agents.size(); i++)
        if(agents[i].loc != compared.agents[i].loc  )
            return false;


    for(int i = 0; i < boxes.size(); i++)
        if(boxes[i].loc != compared.boxes[i].loc)
            return false;


    return true;
}

bool State::isGoalState() {
    for(const Box &box: boxes) {

        auto goals = State::goals;
        auto ite = std::find_if(goals.begin(), goals.end(), [box](const Goal &goal) {
            if(goal.letter != box.letter)
                return false;

            if(box.loc.x != goal.loc.x || box.loc.y != goal.loc.y)
                return false;

            return true;
        });


        if(ite == goals.end())
            return false;
    }
    return true;
}

State *State::makeChild() {
    State *child = new State();

    child->setParent(*this);
    child->setAgents(agents);
    child->setBoxes(boxes);

    addChildState(child);

    return child;
}

bool State::isFree(int x, int y) const{
    if(x >= State::walls.size() || y >= State::walls[x].size() )
        return false;

    return !agentAt(x, y) && !boxAt(x, y) && !walls[x][y];
}

bool State::boxAt(int x, int y, int *boxIndex) const{
    for(int i = 0; i < boxes.size(); i++ ){
        if ((boxes[i].loc.x == x ) && (boxes[i].loc.y == y )) {
            if(boxIndex > 0)
                *boxIndex = i;
            return true;
        }
    }

    if(boxIndex > 0)
        *boxIndex = -1;
    return false;
}

 bool State::goalAt(int x, int y, int *goalIndex) const {
     for(int i = 0; i < State::goals.size(); i++ ){
         if ((State::goals[i].loc.x == x ) && (State::goals[i].loc.y == y )) {
             if(goalIndex > 0)
                *goalIndex = i;
             return true;
         }  
     }

     if(goalIndex > 0)
        *goalIndex = -1;
     return false;
 }


bool State::agentAt(int x, int y, int *agentIndex) const {
    for(int i = 0; i < agents.size(); i++ ){
         if ((agents[i].loc.x == x ) && (agents[i].loc.y == y )) {
             if(agentIndex > 0)
                *agentIndex = i;
             return true;
         }  
     }

     if(agentIndex > 0)
        *agentIndex = -1;
     return false;
}
