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
            std::vector<Goal> goals;

            static std::vector< std::vector<Goal> > goals;
            static std::vector< std::vector<bool> > walls;

            State makeChild();
        public:
            bool isGoalState();
            std::vector<State> getExpandedNodes();
    };
};

#endif