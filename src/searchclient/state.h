#ifndef SEARCHENGINE_STATE_H
#define SEARCHENGINE_STATE_H

#include <iostream>
#include <vector>
#include "typedefs.h"
#include "command.h"

namespace SearchEngine {
    class State {
        public:
            std::vector<Box> boxes;                  
            std::vector<Agent> agents;
            //std::vector<Goal> goals;

            Command action;
            State *parent;
            static std::vector< std::vector<Goal> > goals;
            static std::vector< std::vector<bool> > walls;

            static int numAgents;

            bool operator==(State &compared) {
                for(int i = 0; i < agents.size(); i++)
                    if(compared.agents[i].loc != agents[i].loc)
                        return false;

                for(int i = 0; i < boxes.size(); i++)
                    if(compared.boxes[i].loc != boxes[i].loc)
                        return false;


                return true;
            }

        public:
            bool isGoalState();
            static int getNumAgents() { return numAgents; };

            State makeChild();

<<<<<<< HEAD
            std::vector<State> getExpandedNodes();            
            std::vector<State> getExpandedNodes(int agentIndex);

            bool isFree(int x, int y);
            bool boxAt(int x, int y, int *boxIndex = 0);

=======
            std::vector<State> getExpandedNodes();
            std::vector<State> extractPlan();

            State makeChild();
            bool isFree(int x, int y);
            bool boxAt(int x, int y, int *boxIndex = 0);

            State(State *parentState = 0);
            State(const State &src);

        public:
            bool isGoalState();
            std::vector<State> getExpandedNodes(int agentIndex);
>>>>>>> f59798c79979c78ed9e189f3211c78b5dd7451b0
    };
};

#endif