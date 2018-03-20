#ifndef SEARCHENGINE_STATE_H
#define SEARCHENGINE_STATE_H

#include <iostream>
#include <vector>
#include "typedefs.h"
#include "command.h"

namespace SearchEngine {
    class State {
        private:
            std::vector<Box> boxes;                  
            std::vector<Agent> agents;
            //std::vector<Goal> goals;

            static std::vector< std::vector<Goal> > goals;
            static std::vector< std::vector<bool> > walls;

            static int numAgents;

        public:
            bool isGoalState();
            static int getNumAgents() { return numAgents; };

            State makeChild();

            std::vector<State> getExpandedNodes();
            State makeChild();
            bool isFree(int x, int y);
            bool boxAt(int x, int y, int *boxIndex = 0);


        public:
            bool isGoalState();
            std::vector<State> getExpandedNodes(int agentIndex);
    };
};

#endif