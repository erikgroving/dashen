#include "agent.h"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "../searchengine/strategy.h"
#include "../strategies/strategies"
#include "../heuristics/greedyheuristic.h"
#include "../heuristics/astarheuristic.h"
#include "../heuristics/distanceoracle.h"

#include "../communication/boxpositionentry.h"
#include "../communication/globalgoalentry.h"
#include "../communication/positionentry.h"
#include "../communication/helpentry.h"


#ifndef __LONG_MAX__
#define __LONG_MAX__    2147483647
#endif

using SearchClient::Agent;
using SearchClient::Client;
using namespace SearchEngine::Predicate;

SearchEngine::State *Agent::sharedState;
unsigned int Agent::sharedTime = 0;

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

Agent::Agent(Color color, char num, Coord loc, Communication::Blackboard *blackboard) :
            color(color), num(num), loc(loc), searchStrategy_(),
            private_initialState(), currentSearchGoal_(),
            currentHelpGoal_(),
            currentHelpEntry_(), takenGoals_(), plan_(), blackboard_(blackboard),
            correctGoals_(0), firstMoveInPlan_(false), isWaitingForHelp(false), helpEntriesToMonitor_() {

}

Agent::Agent(const SearchClient::Agent &src): color(src.color), num(src.num), loc(src.loc),
    private_initialState(src.private_initialState), currentSearchGoal_(src.currentSearchGoal_),
    currentHelpGoal_(src.currentHelpGoal_), blackboard_(src.blackboard_), correctGoals_(src.correctGoals_),
    firstMoveInPlan_(src.firstMoveInPlan_), isWaitingForHelp(src.isWaitingForHelp),
    helpEntriesToMonitor_(src.helpEntriesToMonitor_)
     {

}

Agent::~Agent() {
    if(private_initialState != nullptr)
        delete private_initialState;
}

void Agent::identifyBlockingObjects(const std::vector<SearchEngine::State* > &path) {
    std::vector<Coord> forbiddenCoords;
    for(const SearchEngine::State* state: path) {
        forbiddenCoords.push_back(state->getAgents()[num].loc);
    }
    
    for(const SearchEngine::State* state: path) {
        Coord agentLoc = state->getAgents()[num].loc;
        SearchEngine::Command cmd = state->getAction();
        int idx;
        if (boxAt(sharedState, agentLoc.x, agentLoc.y, &idx)) {
            
            std::cerr << "There is an box in the way... at " << 
                agentLoc.x << " , " << agentLoc.y << "\n";

            // Only create an entry if the color is not the same as the agent.
            // Otherwise agent can move it.
            if (sharedState->getBoxes()[idx].color != this->color) {
                auto entry = Communication::HelpEntry::create(agentLoc, Communication::HelpEntry::Box, forbiddenCoords, sharedTime, *this, blackboard_);
                entry->setBlockingBoxId(idx);
                helpEntriesToMonitor_.push_back(entry);
            }

        }
        else if (agentAt(sharedState, agentLoc.x, agentLoc.y, &idx)) {
            
            std::cerr << "There is an agent in the way... at " << 
                agentLoc.x << " , " << agentLoc.y << "\n";

            auto entry = Communication::HelpEntry::create(agentLoc, Communication::HelpEntry::Agent, forbiddenCoords, sharedTime, *this, blackboard_);
            entry->setBlockingAgentId(idx);
            helpEntriesToMonitor_.push_back(entry);
        }
        isWaitingForHelp = true;
    }
}

void Agent::askForHelp(const std::vector<SearchEngine::State *> &path) {

}

std::vector<SearchEngine::State*> Agent::searchGoal(const Goal &goal, SearchEngine::Strategy& strategy, bool ignoreOthers) {
    if(!ignoreOthers)
        configurePrivateInitialState();
    else
        initialStateRemovedAllBut(num, goal.assignedBoxID);

    if (private_initialState->getBoxes()[goal.assignedBoxID].loc.x == -1) {
        return std::vector<SearchEngine::State*>();
    }
    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([&goal](const SearchEngine::State *currentState) {
        return goalHasCorrectBox(currentState, goal);
    }); 

    return searcher.search(strategy, (int)num);
}
    
std::vector<SearchEngine::State*> Agent::searchBox(const Box& box, SearchEngine::Strategy& strategy, bool ignoreOthers) {
    if(!ignoreOthers)
        configurePrivateInitialState();
    else
        initialStateRemovedAllBut(num, box.id);

    if (private_initialState->getBoxes()[box.id].loc.x == -1) {
        return std::vector<SearchEngine::State*>();
    }

    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([&box, this](const SearchEngine::State *currentState) {
        return agentNextToBox(currentState, box, this);
    }); 

    return searcher.search(strategy, (int)num);

}

std::vector<SearchEngine::State*> Agent::searchAllGoals(SearchEngine::Strategy &strategy, bool ignoreOthers) {
    configurePrivateInitialState();
    SearchEngine::SearchCli searcher(private_initialState);
    searcher.setGoalStatePredicate([](const SearchEngine::State *currentState) {
        return isGoalState(currentState);
    });    

    return searcher.search(strategy, (int)num );
}

std::vector<SearchEngine::State *> Agent::searchHelpMoveAgent(SearchEngine::Strategy &strategy, bool ignoreOthers)
{
    configurePrivateInitialState();

    SearchEngine::SearchCli searcher(private_initialState);

    searcher.setGoalStatePredicate([this](const SearchEngine::State *currentState){
        return isAgentNotOnForbiddenPath(currentState, num, currentHelpGoal_.forbiddenPath);
    });

    return searcher.search(strategy, (int) num);
}

std::vector<SearchEngine::State *> Agent::searchHelpMoveBox(SearchEngine::Strategy &strategy, bool ignoreOthers)
{
    configurePrivateInitialState();
    SearchEngine::SearchCli searcher(private_initialState);

    searcher.setGoalStatePredicate([this](const SearchEngine::State *currentState){
        return isAgentNotOnForbiddenPath(currentState, num, currentHelpGoal_.forbiddenPath) &&
               isBoxNotOnForbiddenPath(currentState, currentHelpGoal_.id, currentHelpGoal_.forbiddenPath);
    });

    return searcher.search(strategy, (int) num);
}

void Agent::setSharedState(SearchEngine::State *sharedState) {
    Agent::sharedState = sharedState;
}

SearchEngine::Command Agent::nextMove() {
    /* If plan is empty, need to construct a new plan */
    if(isWaitingForHelp) {
        isWaitingForHelp = false;
        for(auto ite = helpEntriesToMonitor_.begin(); ite != helpEntriesToMonitor_.end();){
            Communication::HelpEntry* entry = (Communication::HelpEntry*) *ite;
            if(entry->getProblemType() != Communication::HelpEntry::Done) {
                ite++;
                isWaitingForHelp = true;
            }
            else {
                Communication::BlackboardEntry::revoke(entry, *this);
                ite = helpEntriesToMonitor_.erase(ite);
            }
        }
        if(isWaitingForHelp)
            return SearchEngine::Command();
    }

    if (plan_.empty()) {
        firstMoveInPlan_ = true;

        // Find the next goal to complete
        // If the search goal is '-', there are no goals available
        if (!determineNextGoal(&currGoalIsHelpGoal_)) {
            return SearchEngine::Command(); // no goals so send a NoOp back
        }
        
        std::vector<SearchEngine::State*> ans;
        if(!currGoalIsHelpGoal_) {
            // Gets the current amount of goals satisfied in the current state
            correctGoals_ = SearchEngine::Predicate::getCorrectGoals(sharedState);

            // Search to find the answer for the goal
            bool searchFailed = false;
            ans = conductSubgoalSearch(&searchFailed);
            if(searchFailed) {
                identifyBlockingObjects(ans);
                return SearchEngine::Command();
            }
        }
        else {
            ans = conductHelpSubgoalSearch();
        }

        // Extract the plan from the search
        extractPlan(ans);

        // Post entries to the blackboard
        postAllPositionEntries(ans);
    }
    else {
        firstMoveInPlan_ = false;
    }

    if (plan_.empty()) {
        return SearchEngine::Command();
    }

    SearchEngine::Command ret = plan_[0];
    plan_.erase(plan_.begin());


    return ret;
}

bool Agent::isEntryDoable(const Communication::BlackboardEntry *entry, const SearchEngine::State* state, int *boxIndex) {

    Box closestBox = Box();
    int goalIndex = -1;
        goalAt(state, entry->getLocation().x, entry->getLocation().y, &goalIndex);
    Goal entryGoal = SearchEngine::State::goals[goalIndex];

    if (Heur::DistanceOracle::fetchDistFromCoord(entryGoal.loc, state->getAgents()[num].loc) == (unsigned long)-1) {
        return false;
    }

    unsigned long minDist = ULONG_MAX;
    for (const Box &b : state->getBoxes()) {

        if (b.color == color && b.letter == entryGoal.letter && !State::takenBoxes[b.id]) {
            //unsigned long dist = Coord::distance(b.loc, entryGoal.loc);
            unsigned long dist = Heur::DistanceOracle::fetchDistFromCoord(entryGoal.loc, b.loc);
            if (dist < minDist) {
                minDist = dist;
                closestBox = b;
            }
        }

    }

    if (closestBox.id == -1) {
        return false;
    }

    if(boxIndex != nullptr) {
        *boxIndex = closestBox.id;
    }

    return true;
}
    

bool Agent::positionFree(size_t x, size_t y, SearchEngine::Command cmd, unsigned int timeStep, std::string& errorDescription) const {
    size_t box_x = x;  // checks that both an agent and boxes new position is valid with timestep
    size_t box_y = y;
    if (cmd.action() == PUSH) {
        box_x = x + SearchEngine::Command::colToInt(cmd.d2());
        box_y = y + SearchEngine::Command::rowToInt(cmd.d2());
    }
    Coord newAgentPos = Coord(x, y);
    Coord newBoxPos = Coord(box_x, box_y);

    auto positionEntries = blackboard_->getPositionEntries();


    // check against agents
    for (auto posForAgent : positionEntries) {
        for(const Communication::BlackboardEntry *entry: posForAgent) {
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
    }

    // check against boxes except the ones that we are currently moving
    int targetBoxIdx = -1;
    if (!isHelpGoal()) {
        targetBoxIdx = currentSearchGoal_.assignedBoxID;
    }
    else if (currentHelpGoal_.type == HelpGoal::HelpType::Box){
        targetBoxIdx = currentHelpGoal_.id;
    }
    for (size_t id = 0; id < private_initialState->getBoxes().size(); id++) {
        auto boxPositionEntries = blackboard_->getBoxEntriesByID(id);
        if((size_t)targetBoxIdx == id || boxPositionEntries.size() == 1)
            continue;
        for (auto ite = boxPositionEntries.begin(); ite != boxPositionEntries.end(); ite++) {
            Communication::BlackboardEntry* entry = *ite;
            Coord entryPos = entry->getLocation();
            if(newAgentPos == entryPos || newBoxPos == entryPos) {
                // Okay... so an agent will go to this entry. But.. WHEN?
                int timeDiff = entry->getTimeStep() - timeStep;
                if (abs(timeDiff) <= 1) {
                    errorDescription = "BOX." + std::to_string(id);
                    return false;
                }
            }
        }
    }

    errorDescription = "NONE";
    return true;
}

Goal Agent::getGoalFromBlackboard() {

    // If the blackboarg is empty, no goal can be retrieved
    if(blackboard_->getGoalEntries().size() <= 0) return Goal();

    Communication::GlobalGoalEntry *selectedEntry = nullptr;
    unsigned int priority = 0;

    int closestBoxIndex = -1;
    // Select the doable entry with the highest priority and find its associated box
    for (Communication::BlackboardEntry *entry : blackboard_->getGoalEntries()) {

        Communication::GlobalGoalEntry *entry_casted = static_cast<Communication::GlobalGoalEntry*>(entry);
        if( priority < entry_casted ->getPriority()                      &&
            isEntryDoable(entry_casted , sharedState, &closestBoxIndex) ) {
            selectedEntry = entry_casted ;
            priority = entry_casted ->getPriority();
        }

    }

    // If no entry is doable, return an empty goal
    if(selectedEntry == nullptr)
        return Goal();


    /* Find the goal for that position */
    int goalIndex;
    goalAt(sharedState, selectedEntry->getLocation().x, selectedEntry->getLocation().y, &goalIndex);

    Goal &result = SearchEngine::State::goals[goalIndex];

    State::takenBoxes[closestBoxIndex] = true;
    result.assignedBoxID = closestBoxIndex;
    takenGoals_.push_back(result);

    Communication::BlackboardEntry::accept(selectedEntry, *this);
    return result;
}

void Agent::clearPlan(SearchEngine::Command cmd) {
    if (cmd.action() == PUSH || cmd.action() == PULL) {
        int id = cmd.targBoxId();
        blackboard_->clear(Communication::Blackboard::BoxPositionEntry, id);
    }
    for (SearchEngine::Command cmd : plan_) {
        if (cmd.action() == PUSH || cmd.action() == PULL) {
            int id = cmd.targBoxId();
            blackboard_->clear(Communication::Blackboard::BoxPositionEntry, id);
        }
    }
    plan_.erase(plan_.begin(), plan_.end());
    blackboard_->clear(Communication::Blackboard::PositionEntry, this->num);
}

void Agent::configurePrivateInitialState() {
    if(private_initialState != nullptr)
        delete private_initialState;

    private_initialState = new SearchEngine::State(*Agent::sharedState);
    private_initialState->setInitialTimeStep(Agent::sharedTime);

    std::vector<char> agentsInMotion = std::vector<char>();
    Communication::Blackboard* bbPtr = this->getBlackboard();


    // Remove agents in motion. Will use blackboard instead 
    auto posEntries = bbPtr->getPositionEntries();
    for (size_t i = 0; i < bbPtr->getPositionEntries().size(); i++) {
        if (posEntries[i].size() > 1) {
            agentsInMotion.push_back(i);
        }
    }

    // Blackboard used for boxes that are in motion.
    auto boxes = private_initialState->getBoxes();
    for (Box& b : boxes) {
        if (bbPtr->getBoxEntriesByID(b.id).size() > 1) {
            b.loc.x = -1;
        }
    }
    private_initialState->setBoxes(boxes);

    std::vector<AgentDescription> agents = private_initialState->getAgents();
    for (char& agentID : agentsInMotion) {
        agents[agentID].loc.x = -1;
    }
    private_initialState->setAgents(agents);
}

void Agent::initialStateRemovedAllBut(char agentIndex, char boxIndex) {
    if(private_initialState != nullptr)
        delete private_initialState;

    private_initialState = new SearchEngine::State(*Agent::sharedState);
    private_initialState->setInitialTimeStep(Agent::sharedTime);

    for(auto &agent: private_initialState->getAgents()) {
        if(agent.num != agentIndex)
            agent.loc.x = -1;
    }

    for(auto &box: private_initialState->getBoxes()) {
        if(box.id != boxIndex)
            box.loc.x = -1;
    }

}

bool Agent::determineNextGoal(bool *isHelpGoal) {


    // Does anyone need help ? We firstly solve any HelpEntry before doing any goal
    if(!currentHelpGoal_.over) {
        *isHelpGoal = true;
        return true;
    }

    if(blackboard_->getHelpEntries().size() > 0) {

        for(auto* entry: blackboard_->getHelpEntries()) {
            Communication::HelpEntry* entry_casted = (Communication::HelpEntry*) entry;

            if(entry_casted->getProblemType() == Communication::HelpEntry::Agent &&
               entry_casted->getBlockingAgentId() == getIndex() ) {

                entry_casted->setProblemType(Communication::HelpEntry::TakenCareOf);
                currentHelpEntry_ = entry;
                currentHelpGoal_ = SearchClient::HelpGoal(SearchClient::HelpGoal::Agent, entry_casted->getBlockingAgentId(), entry_casted->getLocation(), entry_casted->getForbiddenPath(), false);


                *isHelpGoal = true;
                return true;
            }

            else if(entry_casted->getProblemType() == Communication::HelpEntry::Box) {
                Box &box = sharedState->getBoxes()[entry_casted->getBlockingBoxId()];

                if(box.color == color) {

                    entry_casted->setProblemType(Communication::HelpEntry::TakenCareOf);
                    currentHelpEntry_ = entry;
                    currentHelpGoal_ = SearchClient::HelpGoal(SearchClient::HelpGoal::Box, entry_casted->getBlockingBoxId(), entry_casted->getLocation(), entry_casted->getForbiddenPath(), false);

                    *isHelpGoal = true;
                    return true;
                }
            }
        }
    }

    *isHelpGoal = false;

    // See if we have any unsatisfied goals
    bool satisfied = true;
    Goal unsatGoal = Goal();

    for (const Goal& g : takenGoals_) {
        if (!goalHasCorrectBox(sharedState, g)) {
            satisfied = false;
            unsatGoal = g;
            break;
        }
    }

    if (satisfied) {
        currentSearchGoal_ = getGoalFromBlackboard();
        return !(currentSearchGoal_ == Goal());
    }
    else {
        currentSearchGoal_ = unsatGoal;
        return true;
    }
}
    
    
std::vector<SearchEngine::State*> Agent::conductSubgoalSearch(bool *searchFailed) {
    std::vector<SearchEngine::State*> ans = std::vector<SearchEngine::State*>();
    Box &targBox = sharedState->getBoxes()[currentSearchGoal_.assignedBoxID];

    if (!agentNextToBox(sharedState, targBox, this)) {

        std::cerr << "Getting to box " << targBox.id << " (" << targBox.loc.x << ", " << targBox.loc.y << ")" << 
            " from " << sharedState->getAgents()[num].loc.x << "," << sharedState->getAgents()[num].loc.y << std::endl;

        Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic> strat(this);
        strat.linkBlackboard(blackboard_);
        strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
            std::string errorDescription;
            return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                state->getAction() ,state->getTimeStep(), errorDescription);
        });

        ans = searchBox(targBox, strat); //TODO reflect proper strategy
        std::cerr << "Solution of length: " << ans.size() << " found!\n";
    }
    else {
        std::cerr << "Agent " << (int)this->num << " will satisfy goal (" << currentSearchGoal_.loc.x << ", " << currentSearchGoal_.loc.y << ") with box " << targBox.id << std::endl;
        Strat::StrategyHeuristic<Heur::BoxToGoalAStarHeuristic> strat(this);
        strat.linkBlackboard(blackboard_);
        strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
            std::string errorDescription;
            return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                state->getAction(), state->getTimeStep(), errorDescription);
        });
        ans = searchGoal(currentSearchGoal_, strat);
        std::cerr << "Solution of length: " << ans.size() << " found!\n";
    }

    // Something went wrong, could not compute the path
    // Let's compute a simplier path and ask for help
    if(ans.size() == 0) {
        *searchFailed = true;

        if(!agentNextToBox(sharedState, targBox, this)) {
            Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic> strat(this);
            strat.linkBlackboard(nullptr);
            ans = searchBox(targBox, strat, true);
        }
        else {
            Strat::StrategyHeuristic<Heur::BoxToGoalAStarHeuristic> strat(this);
            strat.linkBlackboard(nullptr);
            ans = searchGoal(currentSearchGoal_, strat, true);
        }
    }
    else {
        *searchFailed = false;
    }

    return ans;
}

std::vector<SearchEngine::State*> Agent::conductHelpSubgoalSearch()
{   std::vector<SearchEngine::State*> ans;

    if(currentHelpGoal_.type == SearchClient::HelpGoal::Agent) {
        Strat::StrategyBFSMovePriority strat;
        strat.linkBlackboard(blackboard_);
        ans = searchHelpMoveAgent(strat);

        currentHelpGoal_.over = true;
        if(ans.size() == 0 || isAgentNotOnForbiddenPath(ans.back(), num, currentHelpGoal_.forbiddenPath)) {
            // Cannot help the other agent
            static_cast<Communication::HelpEntry*>(currentHelpEntry_)->setProblemType(Communication::HelpEntry::Done);
        }
        else {
            static_cast<Communication::HelpEntry*>(currentHelpEntry_)->setProblemType(Communication::HelpEntry::Agent);
        }
    }
    else if(currentHelpGoal_.type == SearchClient::HelpGoal::Box) {

        Box &box = sharedState->getBoxes()[currentHelpGoal_.id];
        if(!agentNextToBox(sharedState, box, this)) {

            Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic> strat(this);
            strat.linkBlackboard(blackboard_);
            strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
                std::string errorDescription;
                return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y,
                                    state->getAction() ,state->getTimeStep(), errorDescription);
            });
            ans = searchBox(box, strat);
        }
        else {
            Strat::StrategyBFS strat;
            strat.linkBlackboard(blackboard_);
            ans = searchHelpMoveBox(strat);
            currentHelpGoal_.over = true; // TODO: check that it has indeed worked
            static_cast<Communication::HelpEntry*>(currentHelpEntry_)->setProblemType(Communication::HelpEntry::Done);
        }
    }

    return ans;
}

void Agent::extractPlan(const std::vector<SearchEngine::State*>& ans) {
    // Construct a  plan for the answer
    for (auto *a : ans) {
        plan_.push_back(a->getAction());
    }
}

void Agent::postAllPositionEntries(const std::vector<SearchEngine::State*>& ans) {
    // Construct a  plan for the answer
    for (auto *a : ans) {
        Communication::PositionEntry::create(a->getAgents()[num].loc, a->getTimeStep(), *this, blackboard_ );

        SearchEngine::Command cmd = a->getAction();

        // Add the box position entries
        if (cmd.action() == PUSH || cmd.action() == PULL) { 
            Box targBox;
            targBox = a->getBoxes()[cmd.targBoxId()];
            int newBoxCol, newBoxRow;
            if (cmd.action() == PUSH) {
                newBoxCol = a->getAgents()[num].loc.x + SearchEngine::Command::colToInt(cmd.d2());
                newBoxRow = a->getAgents()[num].loc.y + SearchEngine::Command::rowToInt(cmd.d2());
            }
            else if (cmd.action() == PULL) {
                targBox = a->getBoxes()[cmd.targBoxId()];
                newBoxCol = a->getAgents()[num].loc.x;
                newBoxRow = a->getAgents()[num].loc.y;
            }

            Communication::BoxPositionEntry::create(
                    Coord(newBoxCol, newBoxRow), a->getTimeStep(), targBox.id, *this, blackboard_ );
        }
    }
}
