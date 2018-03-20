#include "state.h"

using SearchEngine::State;
using SearchClient::Command;

std::vector<State> State::getExpandedNodes() {
    std::vector<State> result;

    for(Command::Action action: Command::possibleActions) {
        Command cmd = Command::toCommand(action);

        // int newAgentRow = agent.row + Command::direction(cmd.dir1);
        // int newAgentCol = agent.col + Command::direction(cmd.dir1);
        
        if(cmd.action == Action::MOVE) {
            if( isFree(newAgentRow, newAgentCol) ) {

                State childNode = makeChild();

                childNode.agents.loc.x = newAgentRow;
                childNode.agents.loc.y = newAgentCol;

                result.push_back(childNode);
            }
        }
        else if( cmd.action == Action::PUSH ) {
            if( boxAt(newAgentRow, newAgentCol) ) {
                int newBoxRow = newAgentRow + Command::direction(cmd.dir2);
                int newBoxCol = newAgentCol + Command::direction(cmd.dir2);

                if( isFree(newBoxRow, newBoxCol) ) {
                    State childNode = makeChild();

                    childNode.agents.loc.x = newAgentRow;
                    childNode.agents.loc.y = newAgentCol;

                    childNode.boxes.loc.x = newBoxRow;
                    childNode.boxes.loc.y = newBoxCol;

                    result.push_back(childNode);
                }
            }
        }
        else if( cmd.action == Action::Pull ) {
            if( isFree(newAgentRow, newAgentCol)) {
                int boxRow = agent.row + Command::direction(cmd.dir2);
                int boxCol = agent.col + Command::direction(cmd.dir2);

                if(boxAt(boxRow, boxCol)) {
                    State childNode = makeChild();
                    
                    childNode.agents.loc.x = newAgentRow;
                    childNode.agents.loc.y = newAgentCol;

                    childNode.boxes.loc.x = boxRow;
                    childNode.boxes.loc.y = boxCol;

                    result.push_back(childNode);
                }
            }
        }        


    }

    return result;
}

State State::makeChild() {
    State child;

    child.agents = agents;
    child.boxes = boxes;
    child.goals = goals;

    return child;
}