#include "state.h"
#include <iostream>
#include "predicate.h"

using SearchEngine::State;
using SearchEngine::Command;

using namespace SearchEngine::Predicate;

std::vector<std::vector<bool>> State::walls = std::vector< std::vector<bool> >();
std::vector<Goal> State::goals = std::vector< Goal >();
std::vector<bool> State::takenBoxes = std::vector< bool >();
int State::numAgents = 0;

State::State(unsigned int initialTimeStep): agents(), boxes(), action(), 
                pathCost(0), children(), parent(0), initialTime_(initialTimeStep) {

}

State::State(const State &src): agents(src.agents), boxes(src.boxes), action(src.action),
    pathCost(src.pathCost), children(src.children), parent(src.parent), initialTime_(src.initialTime_) {}

State::~State() {
    for(auto ite = children.begin(); ite != children.end(); ite++) {
        delete *ite;
    }
    children.clear();
}

std::vector<State*> State::getExpandedNodes(int agentIndex) {
    return std::vector<State*>();
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
    State *child = new State(initialTime_);

    child->setParent(this);
    child->setAgents(agents);
    child->setBoxes(boxes);
    if(parent != nullptr)
        child->setPathCost(getPathCost() + 1);
    else
        child->setPathCost(getPathCost());
    
    addChildState(child);

    return child;
}

