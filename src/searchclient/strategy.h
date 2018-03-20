#ifndef SEARCHENGINE_STRATEGY_H
#define SEARCHENGINE_STRATEGY_H

#include <cstddef>
#include <map>
#include <string>
#include <algorithm>
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

class StrategyBFS: public Strategy {

    std::string name() {
      return "Strategy BFS";
    }  

    std::size_t countFrontier() const {
      return queue.size();
    }

    void addToFrontier(const State &state){
      queue.push_back(state);
    }

    bool frontierIsEmpty() const {
      return queue.size() == 0;
    }

    bool inFrontier(const State &state) const {
      auto ite = std::find(queue.begin(), queue.end(), [this](State &s1, State &s2){
        return s1 == s2;
      });

      if(ite == queue.end())
        return false;
      return true;
    }

    State getAndRemoveLeaf() {
      State result = queue.front();
      queue.erase(queue.begin());
      return result;
    }

    private:
      std::vector<State> queue; // TODO: Hash map to speed up the proess
};
};

#endif