#ifndef SEARCHENGINE_STRATEGY_H
#define SEARCHENGINE_STRATEGY_H

#include <cstddef>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <functional>

#include "state_equal_to.h"
#include "state.h"


namespace SearchEngine
{
class State;
/**
 * Abstract class representing a given search strategy. All the methods about the explored
 * states are provided and one only needs to override the methods conserning the frontier.
 */
class Strategy
{

  public:
    Strategy();
    virtual ~Strategy();
    
    /**
     * \return true if leaf has been already explored by the given strategy
     */
    bool isExplored( State *leaf ) const;


    /**
     * \return true if leaf is already part of the frontier
     */
    virtual bool inFrontier( State *leaf ) const;

    /**
     * Add leaf to the set of already explored states
     */
    void addToExplored(State *leaf );
    
    /**
     * \return number of leaves that have been already explored
     */ 
    std::size_t countExplored() const;
    
    /**
     * \return The next leaf to analyse according to the strategy
     */
    virtual State* getAndRemoveLeaf();

    virtual State* getNextLeafStrategy() = 0;
    /**
     * \return true if the frontier is empty
     */
    virtual bool frontierIsEmpty() const;
    
    /**
     * Add leaf to the frontier
     */
    virtual void addToFrontier( State *leaf );
    virtual void addToFrontierStrategy( State *leaf ) = 0;

    /**
     * \return The current size of the frontier
     */
    virtual std::size_t countFrontier() const;

    /**
     * \return String representation of the Strategy.
     */
    virtual std::string name() const = 0;

    void setAdditionalCheckPredicate(const std::function< bool(const State*) > &func) { additionalCheckPredicate_ = func; };
    const std::function< bool(const State*) > additionalCheckPredicate() const { return additionalCheckPredicate_; }

  private:
     std::unordered_map<State*, int, hashState, StateCompare > exploredMap_;
     std::unordered_map<State*, int, hashState, StateCompare > frontierMap_;
     std::function< bool(const State*) > additionalCheckPredicate_;
};

}

#endif
