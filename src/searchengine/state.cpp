#include "state.h"
#include <iostream>
#include "predicate.h"

using SearchEngine::State;
using SearchEngine::Command;

using namespace SearchEngine::Predicate;

std::vector<std::vector<bool>> State::walls = std::vector< std::vector<bool> >();
std::vector<Goal> State::goals = std::vector< Goal >();
int State::numAgents = 0;

State::State(): agents(), boxes(), action(), 
                pathCost(0), children(), parent(0) {

}

State::~State() {
    for(auto ite = children.begin(); ite != children.end(); ite++) {
        delete *ite;
    }
    children.clear();
}

std::vector<State*> State::getExpandedNodes(int agentIndex) {
    std::vector<State*> result;
    
    AgentDescription &agt = agents[agentIndex];

    for(const Command &cmd: Command::possibleActions) {
        // std::cerr << "--" << cmd.toString() << std::endl;

        int newAgentRow = agt.loc.y + Command::rowToInt(cmd.d1());
        int newAgentCol = agt.loc.x + Command::colToInt(cmd.d1());
        
        if(!inBound(this, newAgentCol, newAgentRow))
            continue;



        if(cmd.action() == Action::MOVE) {
//             std::cerr << "Agent could move to (" << newAgentCol << "," << newAgentRow << ") (Command " << cmd.toString() << ") ?";

            if( isFree(this, newAgentCol, newAgentRow) ) {

                State *childNode = makeChild();
                childNode->setAction(cmd);
                childNode->getAgents()[agentIndex].loc.x = newAgentCol;
                childNode->getAgents()[agentIndex].loc.y = newAgentRow;

                result.push_back(childNode);
                // std::cerr << "YES";
            }
            /* else
                std::cerr << "NO";
            std::cerr << std::endl; */
        }
        else if( cmd.action() == Action::PUSH ) {

            //std::cerr << "Agent could push box at (" << newAgentCol << "," << newAgentRow << ") (Command " << cmd.toString() << ") ?";

            int boxIndex;
            if( boxAt(this, newAgentCol, newAgentRow, &boxIndex) ) {
                if (this->getBoxes()[boxIndex].color == agt.color) {
                    int newBoxRow = newAgentRow + Command::rowToInt(cmd.d2());
                    int newBoxCol = newAgentCol + Command::colToInt(cmd.d2());

                    if( isFree(this, newBoxCol, newBoxRow) ) {
                        State *childNode = makeChild();

                        childNode->setAction(cmd);
                        childNode->getAgents()[agentIndex].loc.x = newAgentCol;
                        childNode->getAgents()[agentIndex].loc.y = newAgentRow;

                        childNode->getBoxes()[boxIndex].loc.x = newBoxCol;
                        childNode->getBoxes()[boxIndex].loc.y = newBoxRow;

                        result.push_back(childNode);
                    }
                }
                //else 
                //    std::cerr << "(NO cause the box cannot be pushed)";
            }
            //else
            //    std::cerr << "(NO cause it does not exist)";

            //std::cerr << std::endl;
        }
        else if( cmd.action() == Action::PULL ) {
            int boxIndex;
            if( isFree(this, newAgentCol, newAgentRow)) {
                int boxRow = agt.loc.y + Command::rowToInt(cmd.d2());
                int boxCol = agt.loc.x + Command::colToInt(cmd.d2());

                if(boxAt(this, boxCol, boxRow, &boxIndex)) {
                    if (this->getBoxes()[boxIndex].color == agt.color) {
                        State *childNode = makeChild();
                        childNode->setAction(cmd);

                        childNode->getBoxes()[boxIndex].loc.x = childNode->getAgents()[agentIndex].loc.x;
                        childNode->getBoxes()[boxIndex].loc.y = childNode->getAgents()[agentIndex].loc.y;
                        
                        childNode->getAgents()[agentIndex].loc.x = newAgentCol;
                        childNode->getAgents()[agentIndex].loc.y = newAgentRow;

                        result.push_back(childNode);
                    }
                }
            }
        }        


    }
    std::random_shuffle(result.begin(), result.end());
    return result;
}

std::vector<State*> State::extractPlan() {

    //std::cerr << "Extracting plan" << std::endl;

    std::vector<State*> result;
    result.push_back(this);

    //std::cerr << "Look at parents" << std::endl;

    State *parentState = parent;

    while(parentState != NULL) {
        result.insert(result.begin(), parentState);
        parentState = parentState->parent;
    }
    result.erase(result.begin());
    //std::cerr << "Done";
    return result;
}

State::State(const State &src): agents(src.agents), boxes(src.boxes), action(src.action), parent(src.parent) {

}

bool State::operator==(const SearchEngine::State &compared) const {
    for(unsigned int i = 0; i < agents.size(); i++)
        if(agents[i].loc != compared.agents[i].loc  )
            return false;


    for(unsigned int i = 0; i < boxes.size(); i++)
        if(boxes[i].loc != compared.boxes[i].loc)
            return false;


    return true;
}

State *State::makeChild() {
    State *child = new State();

    child->setParent(this);
    child->setAgents(agents);
    child->setBoxes(boxes);
    child->setPathCost(getPathCost() + 1);
    
    addChildState(child);

    return child;
}

