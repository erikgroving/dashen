#include "strategy.h"
#include "state.h"
#include <iostream>

using SearchEngine::Strategy;
using SearchEngine::Command;
using namespace SearchEngine::Predicate;

Strategy::Strategy(): maxIterations_(1000), exploredMap_(), frontierMap_() {
    additionalCheckPredicate_ = [ ](const SearchEngine::State* /*input */) {
        return true;
    };
}
Strategy::~Strategy() { }

void Strategy::addToExplored( State *leaf ) {
    exploredMap_.insert({leaf, 0});
}

std::size_t Strategy::countExplored() const {
    return  exploredMap_.size();
}

bool Strategy::isExplored(State *leaf) const {
    auto ite = exploredMap_.find(leaf); 
    return ite != exploredMap_.end();
}

bool Strategy::inFrontier(State *leaf) const {
    auto ite = frontierMap_.find(leaf); 
    return ite != exploredMap_.end();
}

std::size_t Strategy::countFrontier() const {
    return frontierMap_.size();
}

void Strategy::doShufflePolicy(std::vector<SearchEngine::State *> &nodes)
{
    std::random_shuffle(nodes.begin(), nodes.end());
}

bool Strategy::frontierIsEmpty() const {
    return frontierMap_.empty();
}

void Strategy::addToFrontier(State *leaf) {
    frontierMap_.insert({leaf, 0});
    addToFrontierStrategy(leaf);
}

SearchEngine::State* Strategy::getAndRemoveLeaf() {
    State *nextState = getNextLeafStrategy();
    if(nextState)
        frontierMap_.erase( nextState );
    return nextState;
}
    
void Strategy::linkBlackboard(Communication::Blackboard* blackboard) {
    blackboard_ = blackboard;
}

void Strategy::setMaxIterations(unsigned int it) {
    maxIterations_ = it;
}

std::vector<SearchEngine::State*> Strategy::expandState(SearchEngine::State* state, int agentIndex) {
    std::vector<State*> result;

    AgentDescription &agt = state->getAgents()[agentIndex];
    for(Command cmd: Command::possibleActions) {
        // std::cerr << "--" << cmd.toString() << std::endl;

        int newAgentRow = agt.loc.y + Command::rowToInt(cmd.d1());
        int newAgentCol = agt.loc.x + Command::colToInt(cmd.d1());

        if(!inBound(state, newAgentCol, newAgentRow))
            continue;

        if(cmd.action() == Action::MOVE) {
//             std::cerr << "Agent could move to (" << newAgentCol << "," << newAgentRow << ") (Command " << cmd.toString() << ") ?";

            if( /*isFree(state, newAgentCol, newAgentRow) && */isFreeBlackboard(blackboard_, state, newAgentCol, newAgentRow) ) {

                SearchEngine::State *childNode = state->makeChild();
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
            if( boxAt(state, newAgentCol, newAgentRow, &boxIndex) ) {
                if (state->getBoxes()[boxIndex].color == agt.color) {
                    int newBoxRow = newAgentRow + Command::rowToInt(cmd.d2());
                    int newBoxCol = newAgentCol + Command::colToInt(cmd.d2());

                    if(/* isFree(state, newBoxCol, newBoxRow) &&*/ isFreeBlackboard(blackboard_, state, newBoxCol, newBoxRow)) {
                        SearchEngine::State *childNode = state->makeChild();

                        cmd.setTargBoxId(boxIndex);
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
            if( /*isFree(state, newAgentCol, newAgentRow) &&*/ isFreeBlackboard(blackboard_, state, newAgentCol, newAgentRow)) {
                int boxRow = agt.loc.y + Command::rowToInt(cmd.d2());
                int boxCol = agt.loc.x + Command::colToInt(cmd.d2());

                if(boxAt(state, boxCol, boxRow, &boxIndex)) {
                    if (state->getBoxes()[boxIndex].color == agt.color) {
                        SearchEngine::State *childNode = state->makeChild();
                        cmd.setTargBoxId(boxIndex);
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

    doShufflePolicy(result);
    return result;
}




std::vector<SearchEngine::State*> Strategy::expandStateDependencies(SearchEngine::State* state, int boxIndex) {
    std::vector<State*> result;

    Box &box = state->boxes[boxIndex];
    for(Command cmd: Command::possibleActions) {
        // std::cerr << "--" << cmd.toString() << std::endl;

        int newBoxRow = agt.loc.y + Command::rowToInt(cmd.d1());
        int newBoxCol = agt.loc.x + Command::colToInt(cmd.d1());

        if(!inBound(state, newBoxCol, newBoxRow))
            continue;

        if(cmd.action() == Action::MOVE) {
//             std::cerr << "Box could move to (" << newBoxCol << "," << newBoxRow << ") (Command " << cmd.toString() << ") ?";

            if( /*isFree(state, newBoxCol, newBoxRow) && */isFreeBlackboard(blackboard_, state, newBoxCol, newBoxRow) ) {

                SearchEngine::State *childNode = state->makeChild();
                childNode->setAction(cmd);
                childNode->boxes[boxIndex].loc.x = newBoxCol;
                childNode->boxes[boxIndex].loc.y = newBoxRow;

                result.push_back(childNode);
                // std::cerr << "YES";
            }
            /* else
                std::cerr << "NO";
            std::cerr << std::endl; */
        }
    }
    doShufflePolicy(result);
    return result;
}
