#ifndef SEARCHENGINE_HEURISTIC_H
#define SEARCHENGINE_HEURISTIC_H

#include <string>
#include <queue>
#include "state.h"

namespace Agent {
    class SearchAgent;
}

namespace SearchEngine {

class State;

/**
 * Base class representing every possible heuristic. The methods have been studied
 * such that Heuristic can be used on a std::priority_queue like this:
 * 
 *     Heuristic myHeuristic;
 *     std::priority_queue<State, std::vector<State>, Heuristic> queue(myHeuristic);
 *     State nextState = queue.pop();
 *
 * A agent pointer can be referenced to the heuristic.
 */
class Heuristic {

public:
    Heuristic(Agent::SearchAgent* agentPtr = nullptr);

    /**
     * Compares the heuristic value of two nodes.
     * Following the std::priority_queue requirements, compare must return false if 
     * s1 has a lower heuristic value than s2. 
     * 
     * \return true if s2 has a greater heuristic value than s1
     */
    bool operator()(const State *s1, const State *s2);

    /**
     * \return The heuristic value of state 
     */
    virtual unsigned long heuristic(const State *state) const = 0;

    /**
     *  \return String representation of the Heuristic object
     */
    virtual std::string name() const = 0;


    /**
     * \return the reference agent registered by the heuristic
     */
    Agent::SearchAgent* getReferenceAgent() { return agentPtr_; }
    const Agent::SearchAgent* getReferenceAgent() const { return agentPtr_; }

    /**
     * \brief Sets the reference agent so it can be used by the heuristic
     */
    void setReferenceAgent(Agent::SearchAgent* refAgent) { agentPtr_ = refAgent; }

private:
    Agent::SearchAgent *agentPtr_;
};

}

#endif
