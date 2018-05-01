#include "level0agent.h"
#include "../searchengine/state.h"

using Agent::Level0Agent;


SearchEngine::State *Level0Agent::sharedState;
unsigned int Level0Agent::sharedTime = 0;

Level0Agent::Level0Agent(Color color, char num, Communication::Blackboard *blackboard):
    color_(color), num_(num), blackboard_(blackboard) {

}

Level0Agent::Level0Agent(const Level0Agent& src):
    color_(src.color_), num_(src.num_), blackboard_(src.blackboard_) {

}

Level0Agent &Level0Agent::operator=(const Agent::Level0Agent &src)
{
    if(this != &src) {

        color_ = src.color_;
        num_ = src.num_;
        blackboard_ = src.blackboard_;

    }

    return *this;
}

Level0Agent::~Level0Agent() {

}

bool Level0Agent::positionNoAgent(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep, std::string &errorDescription) const {

    size_t box_x = x;  // checks that both an agent and boxes new position is valid with timestep
    size_t box_y = y;
    if (cmd.action() == PUSH) {
        box_x = x + SearchEngine::Command::colToInt(cmd.d2());
        box_y = y + SearchEngine::Command::rowToInt(cmd.d2());
    }
    Coord newAgentPos = Coord(x, y);
    Coord newBoxPos = Coord(box_x, box_y);

    // check against agents
    for(const Communication::BlackboardEntry *entry: getBlackboard()->getPositionEntries()) {
        Coord entryPos = entry->getLocation();
        if(newAgentPos == entryPos || newBoxPos == entryPos) {
            // Okay... so an agent/box will go to this entry. But.. WHEN?
            int timeDiff = entry->getTimeStep() - timeStep;
            if (abs(timeDiff) <= 1) {
                errorDescription = "AGENT." + std::to_string(entry->getAuthorId());
                return false;
            }
        }
    }

    return true;
}

bool Level0Agent::positionNoBox(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep, int forgetBoxId, std::string &errorDescription) const {

    size_t box_x = x;  // checks that both an agent and boxes new position is valid with timestep
    size_t box_y = y;
    if (cmd.action() == PUSH) {
        box_x = x + SearchEngine::Command::colToInt(cmd.d2());
        box_y = y + SearchEngine::Command::rowToInt(cmd.d2());
    }
    Coord newAgentPos = Coord(x, y);
    Coord newBoxPos = Coord(box_x, box_y);


    // check against boxes except the ones that we are currently moving
    // int targetBoxIdx = getCurrentSearchGoal().assignedBoxID;
    for (size_t id = 0; id < sharedState->getBoxes().size(); id++) {
        auto boxPositionEntries = getBlackboard()->getBoxEntries(id);

        if( (size_t) forgetBoxId == id)
            continue;


        for (auto ite = boxPositionEntries.begin(); ite != boxPositionEntries.end(); ite++) {
            bool isLastEntry = ite == boxPositionEntries.end() - 1;
            Communication::BlackboardEntry* entry = *ite;
            Coord entryPos = entry->getLocation();
            if(newAgentPos == entryPos || newBoxPos == entryPos) {
                // Okay... so an agent will go to this entry. But.. WHEN?
                int timeDiff = entry->getTimeStep() - timeStep;
                if (abs(timeDiff) <= 1 || isLastEntry) {
                    errorDescription = "BOX." + std::to_string(id);
                    return false;
                }
            }
        }
    }

    return true;


}

bool Level0Agent::positionFree(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep, int forgetBoxId, std::string &errorDescription) const
{

    if(!positionNoAgent(x, y, cmd, timeStep, errorDescription))
        return false;

    if(!positionNoBox(x, y, cmd, timeStep, forgetBoxId, errorDescription))
        return false;

    errorDescription = "NONE";
    return true;
}
