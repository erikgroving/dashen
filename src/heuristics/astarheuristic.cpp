#include "astarheuristic.h"
#include <queue>
#include <unordered_map>
using Heur::AgentToBoxAStarHeuristic;
using Heur::BoxToGoalAStarHeuristic;
using Heur::DistanceOracle;
using Heur::CoordHash;
using std::vector;
using std::queue;
using std::pair;
using std::unordered_map;



std::size_t CoordHash::operator () (const Coord &A) const {
    auto h1 = std::hash<int>{}(A.x);
    auto h2 = std::hash<int>{}(A.y);
    return (h1 ^ h2) * 11;  
}

unordered_map<Coord, vector<vector<unsigned long> >, CoordHash > Heur::DistanceOracle::distances_ = 
    unordered_map<Coord, vector<vector<unsigned long> >, CoordHash >();

vector< vector<unsigned long> > Heur::calculateDistancesFromPoint(Coord point, const SearchEngine::State* s) {
    queue<pair<Coord, unsigned long>> unexplored;
    vector< vector<unsigned long> > distances;
    for (size_t i = 0; i < s->walls.size(); i++) {
        distances.push_back(vector<unsigned long>());
        for (size_t j = 0; j < s->walls[i].size(); j++) {
            distances[i].push_back(-1);
        }
    }

    unexplored.push(std::make_pair(point, 0));

    while (!unexplored.empty()) {
        pair<Coord, unsigned long> currPos = unexplored.front();
        unexplored.pop();

        Coord currCoord = currPos.first;
        unsigned long dist = currPos.second;

        Coord northCoord = Coord(currCoord.x - 1, currCoord.y);
        Coord eastCoord = Coord(currCoord.x, currCoord.y + 1);
        Coord southCoord = Coord(currCoord.x + 1, currCoord.y);
        Coord westCoord = Coord(currCoord.x, currCoord.y - 1);
        vector< Coord > coords = {northCoord, eastCoord, southCoord, westCoord};

        for (Coord c : coords) {
            if (SearchEngine::Predicate::inBound(s, c.x, c.y)) {
                if (!SearchEngine::Predicate::wallAt(s, c.x, c.y)) {
                    if (distances[c.y][c.x] == (unsigned long)-1) {
                        distances[c.y][c.x] = dist;
                        unexplored.push(std::make_pair(c, dist + 1));
                    }
                }
            }
        }
    }
/*
    for (size_t i = 0; i < s->walls.size(); i++) {
        for (size_t j = 0; j < s->walls[i].size(); j++) {
            if (SearchEngine::Predicate::wallAt(s, j, i)) {
                std::cerr << "+ ";
            }
            else {
                std::cerr << (int)distances[i][j] << " ";
            }
        }
        std::cerr << std::endl;
        std::cerr << std::endl;
    }
    exit(0);*/
    return distances;
}

unsigned long DistanceOracle::fetchDistFromCoord(Coord A, Coord B) {
    if (DistanceOracle::distances_.find(A) != DistanceOracle::distances_.end()) {
        return DistanceOracle::distances_[A][B.y][B.x];
    }
    else {
        DistanceOracle::distances_[A] = calculateDistancesFromPoint(A, SearchClient::Agent::sharedState);
        return DistanceOracle::distances_[A][B.y][B.x];
    }
}


AgentToBoxAStarHeuristic::AgentToBoxAStarHeuristic(const SearchClient::Agent* agentPtr)  : Heuristic(agentPtr), agentPtr_(agentPtr) {
}


unsigned long AgentToBoxAStarHeuristic::f(const SearchEngine::State* state) const {
     /*
      * Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
      */
    unsigned long result = 0;
    char agtIdx = agentPtr_->getIndex();
    vector<AgentDescription> agents = state->getAgents();
    Coord agentLoc = state->getAgents()[agtIdx].loc;    
    Goal searchGoal = *(agentPtr_->getCurrentSearchGoal());

    short currCorrectGoals = 0;
    short startSearchCorrectgoals = agentPtr_->getCorrectGoals();
    for (Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            currCorrectGoals += 1;
        }
    }

    result += (1 + startSearchCorrectgoals - currCorrectGoals) * 10;

    /* Find the box assigned to the goal */
    Box closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    /* Add the agent distance to box */
    result += DistanceOracle::fetchDistFromCoord(closestBox.loc, agentLoc);
    //result += Coord::distance(closestBox.loc, agentLoc);

    return result;
}

unsigned long AgentToBoxAStarHeuristic::heuristic(const SearchEngine::State* state) const {
    return f(state) + state->getPathCost();
}

std::string AgentToBoxAStarHeuristic::name() const {
    return "AStar Agent To Box";
}


BoxToGoalAStarHeuristic::BoxToGoalAStarHeuristic(const SearchClient::Agent* agentPtr)  : Heuristic(agentPtr), agentPtr_(agentPtr) {
}


unsigned long BoxToGoalAStarHeuristic::f(const SearchEngine::State* state) const {
     /*
      * Very simple heuristic. Closest box to goal tile plus distance of agent to that box 
      */
    unsigned long result = 0;
    vector<AgentDescription> agents = state->getAgents();
    Goal searchGoal = *(agentPtr_->getCurrentSearchGoal());

    short currCorrectGoals = 0;
    short startSearchCorrectgoals = agentPtr_->getCorrectGoals();
    for (Goal& g : SearchEngine::State::goals) {
        if (SearchEngine::Predicate::goalHasCorrectBox(state, g)) {
            currCorrectGoals += 1;
        }
    }

    result += 1 + startSearchCorrectgoals - currCorrectGoals;

    /* Find the box assigned to the goal */
    Box closestBox = state->getBoxes()[searchGoal.assignedBoxID];
    //IMPORTANT. Use the goal as the FIRST ARGUMENT. This way distances do not
    // need to be recalculated
    result += DistanceOracle::fetchDistFromCoord(searchGoal.loc, closestBox.loc);
    //result += abs(closestBox.loc.x - searchGoal.loc.x) + abs(closestBox.loc.y - searchGoal.loc.y);
    return result;
}

unsigned long BoxToGoalAStarHeuristic::heuristic(const SearchEngine::State* state) const {
    return f(state) + state->getPathCost();
}

std::string BoxToGoalAStarHeuristic::name() const {
    return "AStar - Box To Goal";
}
