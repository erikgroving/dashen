#ifndef SEARCHENGINE_STATE_H
#define SEARCHENGINE_STATE_H

#include <iostream>
#include <vector>
#include <utility>
#include "typedefs.h"

namespace SearchEngine {
    class State {
        private:
            std::vector<Box> boxes;                  
            std::vector<Agent> agents;
            std::vector<Goal> goals;
            static std::vector < std::vector<StaticTile> > tiles;
            std::vector < std::vector<NonStaticTile> > nsTiles;
        public:
            bool isGoalState();
    };
};

#endif