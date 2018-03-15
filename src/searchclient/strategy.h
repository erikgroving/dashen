#ifndef SEARCHENGINE_STRATEGY_H
#define SEARCHENGINE_STRATEGY_H

#include <cstddef>
#include <map>
#include <string>

namespace SearchEngine
{

class Strategy
{

  public:
    Strategy();

    bool isExplored(/* State leaf */) const;
    void addToExplored(/* State leaf */);
    std::size_t countExplored() const;
    
    virtual void /* State */ getAndRemoveLeaf() = 0;
    virtual bool inFrontier(/* State leaf */) const = 0;
    virtual bool frontierIsEmpty() const = 0;
    virtual void addToFrontier() = 0;
    virtual std::size_t countFrontier() const = 0;

    virtual std::string name() const = 0;

  private:
    // std::map<int, State> explored_;
};
};

#endif