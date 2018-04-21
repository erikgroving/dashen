#include "goalpriority.h"

using namespace SearchEngine::Predicate;

unsigned int SearchEngine::GoalPriorityComputation::computeTilePriority(const SearchEngine::State *state, size_t x, size_t y, std::vector<Coord> &visitedTiles) {

    // Not goal tile --> 0
    visitedTiles.push_back(Coord(x, y));
    if(!goalAt(state, x, y))
        return 0;

    // Goal priority = min(neighbors goal priorities) + 1
    unsigned int neighbors = UINT_MAX;
    if(inBound(state, x - 1, y) && !wallAt(state, x - 1, y) &&
            std::find(visitedTiles.begin(), visitedTiles.end(), Coord(x - 1, y)) == visitedTiles.end() )
        neighbors = std::min(neighbors, computeTilePriority(state, x -1, y, visitedTiles));
    if(inBound(state, x + 1, y) && !wallAt(state, x + 1, y) &&
            std::find(visitedTiles.begin(), visitedTiles.end(), Coord(x + 1, y)) == visitedTiles.end() )
        neighbors = std::min(neighbors, computeTilePriority(state, x +1, y, visitedTiles));
    if(inBound(state, x, y -1) && !wallAt(state, x, y - 1) &&
            std::find(visitedTiles.begin(), visitedTiles.end(), Coord(x, y - 1)) == visitedTiles.end())
        neighbors = std::min(neighbors, computeTilePriority(state, x, y - 1, visitedTiles));
    if(inBound(state, x, y + 1) && !wallAt(state, x, y + 1) &&
            std::find(visitedTiles.begin(), visitedTiles.end(), Coord(x, y + 1)) == visitedTiles.end())
        neighbors = std::min(neighbors, computeTilePriority(state, x, y + 1, visitedTiles));

    // Surrounded by three walls --> + 1
    int threeWalls = wallAt(state, x + 1, y) + wallAt(state, x -1, y) + wallAt(state, x, y-1) + wallAt(state, x, y+1);
    if(threeWalls >= 3)
        return neighbors + 1 + 1;
    return neighbors + 1;

}

unsigned int SearchEngine::GoalPriorityComputation::computeGoalPriority(const SearchEngine::State *state, const Goal &goal)
{
    std::vector<Coord> visitedTiles;
    return computeTilePriority(state, goal.loc.x, goal.loc.y, visitedTiles);
}
