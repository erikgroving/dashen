#ifndef SEARCHENGINE_STATE_H
#define SEARCHENGINE_STATE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include "typedefs.h"
#include "command.h"

namespace SearchEngine {

class State {
public:

    /* Constructors */
    State();
    ~State() {
        for(State *child: children)
            delete child;
    }
    State(const State &src);
    State* makeChild();

    /* Operators */
    bool operator==(const State &compared) const;

    /* Static public objects */
    static std::vector<Goal> goals;
    static std::vector< std::vector<bool> > walls;
    static int numAgents;

    /* Predicates */
    bool isFree(int x, int y) const;
    bool isGoalState();
    bool boxAt(int x, int y, int *boxIndex = 0) const ;
    bool goalAt(int x, int y, int *goalIndex = 0) const;
    bool agentAt(int x, int y, int *agentIndex = 0) const;


    /* Planning */
    std::vector<State*> getExpandedNodes(int agentIndex);
    std::vector<State*> extractPlan();

    /* Getters */
    State* getParent() { return parent; }
    std::vector<Box>& getBoxes() { return boxes; }
    std::vector<Agent>& getAgents()  { return agents; }
    Command& getAction() { return action; }

    /* Setters */
    void setParent(State &parentState) {
        if(&parentState == this)
            std::cout << "?????" << std::endl;

        parent = &parentState;
    }

    void setBoxes(std::vector<Box> value) {
        boxes = value;
    }

    void setAgents(std::vector<Agent> value) {
        agents = value;
    }

    void setAction(Command command) {
         action = command;
    }

    void addChildState(State *child) {
        children.push_back(child);
    }
    int idx;

private:

    std::vector<Box> boxes;
    std::vector<Agent> agents;

    Command action;
    State *parent;
    std::vector<State*> children;
};

};

#endif
