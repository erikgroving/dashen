#include "state.h"
#include <iostream>
#include "predicate.h"

using SearchEngine::State;
using SearchEngine::Command;

using namespace SearchEngine::Predicate;

std::vector<std::vector<bool>> State::walls = std::vector< std::vector<bool> >();
std::vector<Goal> State::goals = std::vector< Goal >();
int State::numAgents = 0;

State::State(): parent(0), children(), pathCost(0),
    agents(), boxes(), action() {

}

std::vector<State*> State::getExpandedNodes(int agentIndex) {
    std::vector<State*> result;
    
    AgentDescription &agt = agents[agentIndex];

    for(const Command &cmd: Command::possibleActions) {
        // std::cerr << cmd.toString() << std::endl;
        int newAgentRow = agt.loc.x + Command::rowToInt(cmd.d1);
        int newAgentCol = agt.loc.y + Command::colToInt(cmd.d1);
        
        if(!inBound(this, newAgentRow, newAgentCol))
            continue;

        if(cmd.action == Action::MOVE) {
            //std::cerr << "Agent could move to (" << newAgentRow << "," << newAgentCol << ") (Command " << cmd.toString() << ") ?";

            if( isFree(this, newAgentRow, newAgentCol) ) {

                State *childNode = makeChild();
                childNode->setAction(cmd);
                childNode->getAgents()[agentIndex].loc.x = newAgentRow;
                childNode->getAgents()[agentIndex].loc.y = newAgentCol;

                result.push_back(childNode);
                //std::cerr << "YES";
            }
            //else
                //std::cerr << "NO";
            //std::cerr << std::endl; 
        }
        else if( cmd.action == Action::PUSH ) {

            //std::cerr << "Agent could push box at (" << newAgentRow << "," << newAgentCol << ") (Command " << cmd.toString() << ") ?";

            int boxIndex;
            if( boxAt(this, newAgentRow, newAgentCol, &boxIndex) ) {
                int newBoxRow = newAgentRow + Command::rowToInt(cmd.d2);
                int newBoxCol = newAgentCol + Command::colToInt(cmd.d2);

                if( isFree(this, newBoxRow, newBoxCol) ) {
                    State *childNode = makeChild();

                    childNode->setAction(cmd);
                    childNode->getAgents()[agentIndex].loc.x = newAgentRow;
                    childNode->getAgents()[agentIndex].loc.y = newAgentCol;

                    childNode->getBoxes()[boxIndex].loc.x = newBoxRow;
                    childNode->getBoxes()[boxIndex].loc.y = newBoxCol;

                    result.push_back(childNode);
                }
                //else 
                    //std::cerr << "(NO cause the box cannot be pushed)";
            }
            //else
                //std::cerr << "(NO cause it does not exist)";

            //std::cerr << std::endl;
        }
        else if( cmd.action == Action::PULL ) {
            int boxIndex;
            if( isFree(this, newAgentRow, newAgentCol)) {
                int boxRow = agt.loc.x + Command::rowToInt(cmd.d2);
                int boxCol = agt.loc.y + Command::colToInt(cmd.d2);

                if(boxAt(this, boxRow, boxCol, &boxIndex)) {
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

    //std::cerr << "Extracting plan" << std::endl;

    std::vector<State*> result;
    result.push_back(this);

    //std::cerr << "Look at parents" << std::endl;

    State *parentState = parent;

    if(parent == this)
        //std::cerr << "I am my own parent" << std::endl;

    while(parentState != NULL) {
        // std::cerr << parentState;
        result.insert(result.begin(), parentState);
        parentState = parentState->parent;
    }

    //std::cerr << "Done";
    return result;
}

State::State(const State &src): agents(src.agents), boxes(src.boxes), action(src.action), parent(src.parent) {

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

State *State::makeChild() {
    State *child = new State();

    child->setParent(*this);
    child->setAgents(agents);
    child->setBoxes(boxes);
    child->setPathCost(getPathCost() + 1);
    
    addChildState(child);

    return child;
}

