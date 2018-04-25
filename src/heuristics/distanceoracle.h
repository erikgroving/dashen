#ifndef DISTANCEORACLE_H
#define DISTANCEORACLE_H

#include <utility>
#include <vector>
#include <climits>
#include "../searchengine/typedefs.h"
#include "../searchengine/state.h"
#include "../searchengine/predicate.h"

namespace Heur {

class DistanceOracle {

    using DistanceMatrix = std::vector< std::vector<unsigned long> >;

public:
    class CoordHash {
        public:
            std::size_t operator () (const Coord &A) const;
    };

public:
    static unsigned long fetchDistFromCoord(const Coord &A, const Coord &B);

private:
    static DistanceMatrix calculateDistancesFromPoint(const Coord &point, const SearchEngine::State* s);
    static std::unordered_map<Coord, DistanceMatrix, CoordHash > distances_;
};

}

#endif // DISTANCEORACLE_H
