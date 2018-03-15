#ifndef SEARCHENGINE_HEURISTIC_H
#define SEARCHENGINE_HEURISTIC_H

#include <string>
#include <queue>
namespace SearchEngine {

/**
 * Base class representing every possible heuristic. The methods have been studied
 * such that Heuristic can be used on a std::priority_queue like this:
 * 
 *     Heuristic myHeuristic;
 *     std::priority_queue<State, std::vector<State>, Heuristic> queue(myHeuristic);
 *     State nextState = queue.pop();
 */
class Heuristic {

public:
    Heuristic();

    /**
     * Compares the heuristic value of two nodes.
     * Following the std::priority_queue requirements, compare must return false if 
     * s1 has a lower heuristic value than s2. 
     * 
     * \return true if s2 has a greater heuristic value than s1
     */
    bool operator()(/* const State &s1, const State &s2 */);

    /**
     * \return The heuristic value of state 
     */
    virtual int heuristic(/* const State state */) = 0;

    /**
     *  \return String representation of the Heuristic object
     */
    virtual std::string name() const = 0;
};

};

#endif