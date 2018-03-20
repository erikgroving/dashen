#ifndef SEARCHENGINE_STRATEGY_H
#define SEARCHENGINE_STRATEGY_H

#include <cstddef>
#include <map>
#include <string>

#include "state.h"

namespace SearchEngine
{

/**
 * Abstract class representing a given search strategy. All the methods about the explored
 * states are provided and one only needs to override the methods conserning the frontier.
 */
class Strategy
{

  public:
    Strategy();

    /**
     * \return true if leaf has been already explored by the given strategy
     */
    bool isExplored( const State &leaf ) const;

    /**
     * Add leaf to the set of already explored states
     */
    void addToExplored( const State &leaf );
    
    /**
     * \return number of leaves that have been already explored
     */ 
    std::size_t countExplored() const;
    
    /**
     * \return The next leaf to analyse according to the strategy
     */
    virtual State getAndRemoveLeaf() = 0;

    /**
     * \return true if leaf is already part of the frontier
     */
    virtual bool inFrontier( const State &leaf ) const = 0;
    
    /**
     * \return true if the frontier is empty
     */
    virtual bool frontierIsEmpty() const = 0;
    
    /**
     * Add leaf to the frontier
     */
    virtual void addToFrontier( const State &leaf ) = 0;

    /**
     * \return The current size of the frontier
     */
    virtual std::size_t countFrontier() const = 0;

    /**
     * \return String representation of the Strategy.
     */
    virtual std::string name() const = 0;

  private:
     std::map<int, State> explored_;
};
};

#endif