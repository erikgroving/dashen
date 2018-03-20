#include "state.h"

using SearchEngine::State;
using SearchEngine::Command;

State::State(State *parentState): parent(parentState) {}

std::vector<State> State::getExpandedNodes(int agentIndex) {
    std::vector<State> result;
    
    Agent &agt = agents[agentIndex];

    for(const Command &cmd: Command::possibleActions) {

         int newAgentRow = agt.loc.x + Command::directionToInt(cmd.d1);
         int newAgentCol = agt.loc.y + Command::directionToInt(cmd.d1);
        
        if(cmd.action == Action::MOVE) {
            if( isFree(newAgentRow, newAgentCol) ) {

                State childNode = makeChild();
                childNode.action = cmd;
                childNode.agents[agentIndex].loc.x = newAgentRow;
                childNode.agents[agentIndex].loc.y = newAgentCol;

                result.push_back(childNode);
            }
        }
        else if( cmd.action == Action::PUSH ) {
            int boxIndex;
            if( boxAt(newAgentRow, newAgentCol, &boxIndex) ) {
                int newBoxRow = newAgentRow + Command::directionToInt(cmd.d2);
                int newBoxCol = newAgentCol + Command::directionToInt(cmd.d2);

                if( isFree(newBoxRow, newBoxCol) ) {
                    State childNode = makeChild();
                    childNode.action = cmd;
                    childNode.agents[agentIndex].loc.x = newAgentRow;
                    childNode.agents[agentIndex].loc.y = newAgentCol;

                    childNode.boxes[boxIndex].loc.x = newBoxRow;
                    childNode.boxes[boxIndex].loc.y = newBoxCol;

                    result.push_back(childNode);
                }
            }
        }
        else if( cmd.action == Action::PULL ) {
            int boxIndex;
            if( isFree(newAgentRow, newAgentCol)) {
                int boxRow = agt.loc.x + Command::directionToInt(cmd.d2);
                int boxCol = agt.loc.y + Command::directionToInt(cmd.d2);

                if(boxAt(boxRow, boxCol, &boxIndex)) {
                    State childNode = makeChild();
                    childNode.action = cmd;
                    childNode.agents[agentIndex].loc.x = newAgentRow;
                    childNode.agents[agentIndex].loc.y = newAgentCol;

                    childNode.boxes[boxIndex].loc.x = boxRow;
                    childNode.boxes[boxIndex].loc.y = boxCol;

                    result.push_back(childNode);
                }
            }
        }        


    }

    return result;
}

std::vector<State> State::extractPlan() {
    std::vector<State> result;
    result.push_back(this);

    State *parentState = parent;
    while(parentState != 0) {
        result.insert(result.begin(), *parentState);
        parentState = (*parentState).parent;
    }

    return result;
}

State::State(const State &src): agents(src.agents), boxes(src.boxes) {}
State State::makeChild() {
    State child(this);

    child.agents = agents;
    child.boxes = boxes;

    return child;
}

 bool State::isFree(int x, int y){
     for(const Agent &agent: agents ) {
         if ((agent.loc.x == x ) && (agent.loc.y == y ))
            return false;
     }
    if (boxAt(x,y))
        return false;

    if (walls[x][y])
       return false;
    return true;
 }

 bool State::boxAt(int x, int y, int *boxIndex){
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
