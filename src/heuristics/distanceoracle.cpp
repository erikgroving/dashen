#include "distanceoracle.h"

#ifndef __LONG_MAX__
#define __LONG_MAX__ 234881234
#endif

using Heur::DistanceOracle;

std::unordered_map<Coord, DistanceOracle::DistanceMatrix, DistanceOracle::CoordHash > Heur::DistanceOracle::distances_ =
    std::unordered_map<Coord, DistanceOracle::DistanceMatrix, DistanceOracle::CoordHash >();

unsigned long DistanceOracle::fetchDistFromCoord(const Coord &A, const Coord &B) {

    if (DistanceOracle::distances_.find(A) != DistanceOracle::distances_.end()) {
        return DistanceOracle::distances_[A][B.y][B.x];
    }
    else if (DistanceOracle::distances_.find(B) != DistanceOracle::distances_.end()) {
        return DistanceOracle::distances_[B][A.y][A.x];
    }
    else {
        DistanceOracle::distances_[A] = calculateDistancesFromPoint(A, SearchClient::Agent::sharedState);
        return DistanceOracle::distances_[A][B.y][B.x];
    }

}


DistanceOracle::DistanceMatrix DistanceOracle::calculateDistancesFromPoint(const Coord &point, const SearchEngine::State *s) {

    DistanceMatrix distances;
    for (size_t i = 0; i < s->walls.size(); i++) {
        distances.push_back(std::vector<unsigned long>(s->walls[i].size(), ULONG_MAX));
    }

    std::queue<std::pair<Coord, unsigned long>> unexplored;
    unexplored.push(std::make_pair(point, 0));

    while (!unexplored.empty()) {
        auto currPos = unexplored.front();
        unexplored.pop();

        Coord currCoord;
        unsigned long dist;
        std::tie(currCoord, dist) = currPos;

        std::vector< Coord > coords = {
            Coord(currCoord.x - 1, currCoord.y), Coord(currCoord.x, currCoord.y + 1),
            Coord(currCoord.x + 1, currCoord.y), Coord(currCoord.x, currCoord.y - 1)
        };

        for (const Coord &c : coords) {
            if (SearchEngine::Predicate::inBound(s, c.x, c.y) &&
                !SearchEngine::Predicate::wallAt(s, c.x, c.y) &&
                distances[c.y][c.x] == ULONG_MAX ){

                distances[c.y][c.x] = dist;
                unexplored.push(std::make_pair(c, dist + 1));
            }
        }
    }

    return distances;
}

size_t DistanceOracle::CoordHash::operator()(const Coord &A) const {
    auto h1 = std::hash<int>{}(A.x);
    auto h2 = std::hash<int>{}(A.y);
    return (h1 ^ h2) * 11;
}

