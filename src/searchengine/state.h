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
    State(unsigned int timeStep = 0);
    ~State();
    State(const State &src);
    State* makeChild();

    /* Operators */
    bool operator==(const State &compared) const;
    bool operator<(const State &compared) const;

    /* Static public objects */
    static std::vector<Goal> goals;
    static std::vector< std::vector<bool> > walls;
    static std::vector< bool > takenBoxes;
    static int numAgents;
    /* Predicates */



    /* Planning */
    std::vector<State*> getExpandedNodes(int agentIndex);
    std::vector<State*> extractPlan();

    /* Getters */
    State* getParent() const { return parent; }
    const State* getParent() { return parent; }

    std::vector<Box>& getBoxes() { return boxes; }
    const std::vector<Box>& getBoxes() const { return boxes; }

    std::vector<AgentDescription>& getAgents() { return agents; }
    const std::vector<AgentDescription>& getAgents() const { return agents; }

    unsigned int getTimeStep() const { return initialTime_ + getPathCost(); }
    Command& getAction() { return action; }
    const Command& getAction() const { return action; }

    unsigned long getPathCost() const { return pathCost; }
    void setPathCost(unsigned long value) { pathCost = value; }
    
    /* Setters */
    void setParent(State* const &parentState) {
        /*
        if(&parentState == this)
            std::cout << "?????" << std::endl;
        */
        parent = parentState;
    }

    void setBoxes(std::vector<Box> value) { boxes = value; }
    void setAgents(std::vector<AgentDescription> value) { agents = value;}
    void setAction(Command command) { action = command; }
    void addChildState(State *child) { children.push_back(child); }

    void setInitialTimeStep(unsigned int timeStep) { initialTime_ = timeStep; }

private:


    std::vector<AgentDescription> agents;
    std::vector<Box> boxes;
    Command action;

    unsigned long pathCost;

    std::vector<State*> children;
    State *parent;

    unsigned int initialTime_;
};

}
#endif
