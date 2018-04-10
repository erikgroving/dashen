#include "distance.h"

using SearchEngine::State;
using SearchEngine::Distance;


std::vector<std::vector<short int> > 
Distance::getDistanceFromPosition(const State &state, size_t x, size_t y) {

    std::vector<std::vector<short int> > distances;//n, std::vector<short int>(n, -1);

    //for each vector in walls
        //initialize vector to lenth of vector in walls
    for (std::vector<bool> wallvector : state.walls) {
        size_t lengthOfRow = wallvector.size();
        distances.push_back(std::vector<short int>(lengthOfRow, -1));
    }

}
