#include "goalpriority.h"
#ifndef __INT_MAX__
#define __INT_MAX__ 2147483647
#endif
#include <queue>
#include <unordered_map>
#include "../heuristics/distanceoracle.h"
using std::queue;
using std::pair;
using std::vector;
using namespace SearchEngine::GoalPriorityComputation;
using namespace SearchEngine::Predicate;

vector<vector<int>> SearchEngine::StrictOrdering::strictOrderings;
void SearchEngine::StrictOrdering::calculateStrictOrderings(SearchEngine::State state) {
    std::vector<Coord> fakeWalls; // remove the fake walls after calculating the strict orderings
    std::unordered_map<Coord, int, Heur::DistanceOracle::CoordHash> fakeWallIds = 
        std::unordered_map<Coord, int, Heur::DistanceOracle::CoordHash>();

    for (size_t i = 0; i < SearchEngine::State::goals.size(); i++) {
        strictOrderings.push_back(std::vector<int>());
    }

    bool goalsUpdated = true;
    while (goalsUpdated) {
        goalsUpdated = false;
        for (size_t k = 0; k < SearchEngine::State::goals.size(); k++) {
            Coord x = SearchEngine::State::goals[k].loc;
            size_t j = x.x;
            size_t i = x.y;
            if (!wallAt(&state, j, i) && getSurroundingWalls(state, Coord(j, i)) == 3) {
                goalsUpdated = true;
                fakeWalls.push_back(Coord(j, i));
                SearchEngine::State::walls[i][j] = true;
                int gIdx = -1;
                if (goalAt(&state, j, i, &gIdx)) {
                    fakeWallIds.insert(std::make_pair(Coord(j, i), gIdx));
                    // Check if there are fake wall goals
                    Coord north = Coord(j, i - 1);
                    Coord east = Coord(j + 1, i);
                    Coord south = Coord(j, i + 1);
                    Coord west = Coord(j - 1, i);
                    vector<Coord> dirs = {north, east, south, west};
                    for (Coord c : dirs) {
                        if (wallAt(&state, c.x, c.y)) {
                            if (fakeWallIds.find(c) != fakeWallIds.end()) {
                                strictOrderings[gIdx].push_back(fakeWallIds[c]);
                            }
                        }
                    }
                } 
            }
        }
    }
    
    // Clear all the fake walls that we put in the state
    for (Coord c : fakeWalls) {
        SearchEngine::State::walls[c.y][c.x] = false;
    }
    fakeWalls = std::vector<Coord>(); // remove the fake walls after calculating the strict orderings
    fakeWallIds = std::unordered_map<Coord, int, Heur::DistanceOracle::CoordHash>();


    goalsUpdated = true;
    while (goalsUpdated) {
        goalsUpdated = false;
        for (size_t i = 0; i < height(&state); i++) {
            for (size_t j = 0; j < width(&state, i); j++) {
                if (!wallAt(&state, j, i) && getSurroundingWalls(state, Coord(j, i)) == 3) {
                    goalsUpdated = true;
                    fakeWalls.push_back(Coord(j, i));
                    SearchEngine::State::walls[i][j] = true;
                    
                    int gIdx = -1;
                    if (goalAt(&state, j, i, &gIdx)) {
                        fakeWallIds.insert(std::make_pair(Coord(j, i), gIdx));
                        // Check if there are fake wall goals
                        Coord north = Coord(j, i - 1);
                        Coord east = Coord(j + 1, i);
                        Coord south = Coord(j, i + 1);
                        Coord west = Coord(j - 1, i);
                        vector<Coord> dirs = {north, east, south, west};
                        for (Coord c : dirs) {
                            if (wallAt(&state, c.x, c.y)) {
                                if (fakeWallIds.find(c) != fakeWallIds.end()) {
                                    strictOrderings[gIdx].push_back(fakeWallIds[c]);
                                }
                            }
                        }
                    } 
                    // Insert walls until we hit a goal or no more 3 wall (Trim them)
                    bool insertedFakeWall; 
                    Coord currCoord = Coord(j, i);
                    do {
                        insertedFakeWall = false;
                        // Check surrounding coordinates to see if there is a new 3 waller 
                        Coord north = Coord(currCoord.x, currCoord.y - 1);
                        Coord east = Coord(currCoord.x + 1, currCoord.y);
                        Coord south = Coord(currCoord.x, currCoord.y + 1);
                        Coord west = Coord(currCoord.x - 1, currCoord.y);
                        vector<Coord> dirs = {north, east, south, west};
                        for (Coord c : dirs) {
                            if (!wallAt(&state, c.x, c.y) && getSurroundingWalls(state, c) == 3) {
                                if (!goalAt(&state, c.x, c.y)) {
                                    if (fakeWallIds.find(currCoord) != fakeWallIds.end()) {
                                        fakeWallIds.insert(std::make_pair(c, fakeWallIds[currCoord]));
                                    }
                                    insertedFakeWall = true;
                                    fakeWalls.push_back(c);
                                    SearchEngine::State::walls[c.y][c.x] = true;
                                    currCoord = c;
                                    break;
                                }
                            }
                        }
                    } while(insertedFakeWall);
                }
            }
        }
    }

    // Clear all the fake walls that we put in the state
    for (Coord c : fakeWalls) {
        SearchEngine::State::walls[c.y][c.x] = false;
    }

    return;
}

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


std::vector<unsigned int> SearchEngine::GoalPriorityComputation::computeAllGoalPriorities(const SearchEngine::State* state) {
    SearchEngine::State s = *state;

    std::vector<Goal> goalsRemaining = s.goals;
    std::vector<unsigned long> dists;
    std::vector<unsigned int> priorities = std::vector<unsigned int>();

    for (size_t i = 0; i < s.goals.size(); i++) {
        unsigned long total_dists = 0;
        for (size_t j = 0; j < height(state); j++) {
            for (size_t k = 0; k < width(state, j); k++) {
                Coord currCoord = Coord(k, j);
                unsigned long dist = Heur::DistanceOracle::fetchDistFromCoord(s.goals[i].loc, currCoord);
                if (dist != (unsigned long) -1) {
                    total_dists += dist;
                }
            }
        }
        priorities.push_back(total_dists);
    }
    return priorities;
/*
    int idx = 0;
    for (auto dist : dists) {
        idx++;
    }*/
    // Greedy first:
    // If a box can be placed without hindering anything, it shall be placed first
/*
    size_t indexInPriorities = 0;
    for (size_t i=0; i<goalsRemaining.size(); i++) {
        
        Goal g = goalsRemaining[i];

        // If goal is free standing (using isFieldBlockable)
        bool isBlockable = Predicate::isFieldBlockable(&s, g.loc.x, g.loc.y);
        if (isBlockable) {
            priorities[indexInPriorities] = goalsRemaining.size();
            s.walls[g.loc.y][g.loc.x] = true;
            goalsRemaining.erase(goalsRemaining.begin() + i, goalsRemaining.begin() + i + 1);
            i--;
        }
        indexInPriorities++;
    } 


    while (!goalsRemaining.empty()) {

        int minWallCount = -1;
        int minGoalIdx;
        
        // Count surrounding walls. IF 3 then definitely next one
        for (size_t j = 0; j < goalsRemaining.size(); j++) {
            Goal g = goalsRemaining[j];
            int surroundingWalls = getSurroundingWalls(s, g.loc);
            if (surroundingWalls > minWallCount) {
                minWallCount = surroundingWalls;
                minGoalIdx = j;
                if (minWallCount == 3) {
                    break;
                }
            }
        }

        Goal priorityGoal = goalsRemaining[minGoalIdx];
        int goalID = -1;
        for (size_t j = 0; j < s.goals.size(); j++) {
            if (priorityGoal.loc == s.goals[j].loc) {
                goalID = j;
                break;
            }
        }
        s.walls[priorityGoal.loc.y][priorityGoal.loc.x] = true;
        if (!goalsStillAccessible(s, goalsRemaining, priorityGoal)) {
            // Choosing this goal made another goal inaccessible.
            // We then put it in the back so another goal is chosen
            s.walls[priorityGoal.loc.y][priorityGoal.loc.x] = false;
            std::rotate(goalsRemaining.begin(), goalsRemaining.begin() +1, goalsRemaining.end());
        }
        else {
            // Can still reach all goals! Give this goal a priority and continue
            // Add the min goal to have the top priority
            priorities[goalID] = goalsRemaining.size();
            goalsRemaining.erase(goalsRemaining.begin() + minGoalIdx, goalsRemaining.begin() + minGoalIdx + 1);
        }
    } 


    // De-wall the goal tiles.. since walls are a static object in the State class
    for (Goal& g: state->goals) {
        s.walls[g.loc.y][g.loc.x] = false;
    }
    return priorities; */
}

/* Are all the remaining goals still accessible by a box with correct letter ? */
bool SearchEngine::GoalPriorityComputation::goalsStillAccessible(const SearchEngine::State& state, std::vector<Goal> remGoals, Goal prospectiveGoal) {
    for (Goal g : remGoals) {
        if (g.loc == prospectiveGoal.loc) {
            continue;
        }

        bool reached = false;
        for (Box box : state.getBoxes()) {
            if (box.letter == g.letter) {
                if (canReach(state, box.loc, g.loc)) {
                    reached = true;
                    break;
                }
            }
        }
        if (!reached) {
            for (auto agent : state.getAgents()) {
                if (canReach(state, agent.loc, g.loc)) {
                    reached = true;
                    break;
                }
            }
        }
        if (!reached) {
            return false;
        }
    }

    return true;
}

/* Can we reach the goal from the box's current position? only looking at walls */
bool SearchEngine::GoalPriorityComputation::canReach(const SearchEngine::State& state, Coord box, Coord goal) {
    queue<Coord> unexplored;

    vector< vector<bool> > seen;

    for (size_t i = 0; i < state.walls.size(); i++) {
        seen.push_back(vector<bool>());
        for (size_t j = 0; j < state.walls[i].size(); j++) {
            seen[i].push_back(false);
        }
    }

    unexplored.push(box);
    seen[box.y][box.x] = true;

    while (!unexplored.empty()) {
        Coord currCoord  = unexplored.front();
        unexplored.pop();

        if (currCoord == goal) {
            return true;
        }

        Coord northCoord = Coord(currCoord.x - 1, currCoord.y);
        Coord eastCoord = Coord(currCoord.x, currCoord.y + 1);
        Coord southCoord = Coord(currCoord.x + 1, currCoord.y);
        Coord westCoord = Coord(currCoord.x, currCoord.y - 1);
        vector< Coord > coords = {northCoord, eastCoord, southCoord, westCoord};

        for (Coord c : coords) {
            if (SearchEngine::Predicate::inBound(&state, c.x, c.y)) {
                if (!seen[c.y][c.x] && !SearchEngine::Predicate::wallAt(&state, c.x, c.y)) {
                    unexplored.push(c);
                    seen[c.y][c.x] = true;
                }
            }
        }
    }
    return false;
}

int SearchEngine::GoalPriorityComputation::getSurroundingWalls(const SearchEngine::State& state, Coord c) {
    Coord northCoord = Coord(c.x - 1, c.y);
    Coord eastCoord = Coord(c.x, c.y + 1);
    Coord southCoord = Coord(c.x + 1, c.y);
    Coord westCoord = Coord(c.x, c.y - 1);
    std::vector< Coord > coords = {northCoord, eastCoord, southCoord, westCoord};

    int res = 0;
    for (Coord c : coords) {
        if (wallAt(&state, c.x, c.y)) {
            res++;
        }
    }

    return res;
}
