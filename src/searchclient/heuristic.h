#ifndef SEARCHENGINE_HEURISTIC_H
#define SEARCHENGINE_HEURISTIC_H

#include <string>

namespace SearchEngine {

class Heuristic {

public:
    Heuristic();

    /**
     * Compares the heuristic value of two nodes.
     * Following the std::priority_queue requirements, compare must return false if 
     * s1 has a lower heuristic value than s2. 
     */
    bool compare(/* const State &s1, const State &s2 */);

    /**
     * Returns the heuristic value of state 
     */
    virtual int heuristic(/* const State state */) const = 0;

    virtual std::string name() const = 0;
};

};

#endif