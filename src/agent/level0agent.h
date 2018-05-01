#ifndef LEVEL0AGENT_H
#define LEVEL0AGENT_H

#include "../searchengine/typedefs.h"
#include "../communication/blackboard.h"
#include "../communication/blackboardentry.h"
#include "../searchengine/command.h"

namespace SearchEngine {
    class State;
}

namespace Agent {

class Level0Agent
{

public: // Constructors
    Level0Agent(Color color, char num,
                Communication::Blackboard *blackboard = nullptr);
    Level0Agent(const Level0Agent& src);
    Level0Agent& operator=(const Level0Agent& src);
    virtual ~Level0Agent();

public: // Operators
    bool operator<(const Level0Agent& a) const { return num_ < a.num_;  }

public: // Static attributes
    static SearchEngine::State* sharedState;
    static unsigned int sharedTime;

public: // Getters

    Color getColor() const { return color_; }
    int getIndex() const { return num_; }

    const Communication::Blackboard* getBlackboard() const { return blackboard_; }
    Communication::Blackboard* getBlackboard() { return blackboard_; }

    bool positionNoAgent(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep, std::string &errorDescription) const;
    bool positionFree(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep, int forgetBoxId, std::string &errorDescription) const;
    bool positionNoBox(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep, int forgetBoxId, std::string &errorDescription) const;

public: // Setters
    void setColor(Color newColor) { color_ = newColor; }
    void setIndex(char idx) { num_ = idx; }
    void setBlackboard(Communication::Blackboard* blck) { blackboard_ = blck; }

private:
    Color color_;
    char num_;
    Communication::Blackboard* blackboard_;


};

}

#endif // LEVEL0AGENT_H
