#include "agent.h"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "../searchengine/strategy.h"
#include "../searchengine/goalpriority.h"
#include "../strategies/strategies"
#include "../heuristics/greedyheuristic.h"
#include "../heuristics/astarheuristic.h"
#include "../heuristics/distanceoracle.h"

#include "../communication/boxpositionentry.h"
#include "../communication/globalgoalentry.h"
#include "../communication/positionentry.h"
#include "../communication/helpentry.h"

#include "../searchengine/master.h"
#include "../searchengine/region.h"

#ifndef __LONG_MAX__
#define __LONG_MAX__    2147483647
#endif

using SearchClient::Agent;
using SearchEngine::Region;
using SearchClient::Client;
using namespace SearchEngine::Predicate;

SearchEngine::State *Agent::sharedState;
unsigned int Agent::sharedTime = 0;
std::vector<Goal> Agent::takenGoals = std::vector<Goal>();

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
            color(color), num(num), searchStrategy_(),
            private_initialState(), ctIdx_(-1), noFoundGoals_(0), takenTasks_(),
            plan_(), blackboard_(blackboard),
            correctGoals_(0) {

}

Agent::Agent(const SearchClient::Agent &src): color(src.color), num(src.num),
    private_initialState(src.private_initialState), ctIdx_(src.ctIdx_),
    noFoundGoals_(src.noFoundGoals_), takenTasks_(src.takenTasks_), plan_(src.plan_),
    blackboard_(src.blackboard_), correctGoals_(src.correctGoals_)
     {

}

Agent::~Agent() {
    if(private_initialState != nullptr)
        delete private_initialState;
}

void Agent::identifyBlockingObjects(const std::vector<SearchEngine::State* > &path) {
    std::vector<Coord> forbiddenCoords;

    auto revPath = path;
    std::reverse(revPath.begin(), revPath.end());

    if (takenTasks_[ctIdx_].task.type == GOAL) {
        forbiddenCoords.push_back(takenTasks_[ctIdx_].task.goal.loc);
    }

    for(const SearchEngine::State* state: revPath) {
        Coord agentLoc = state->getAgents()[num].loc;
        forbiddenCoords.push_back(agentLoc);
    }
    if (path.size() > 0) {
    auto cmdStart = path[0]->getAction();
    Coord agtSecond = path[0]->getAgents()[num].loc;
    Coord agentStart = Coord(agtSecond.x - SearchEngine::Command::colToInt(cmdStart.d1()),
                            agtSecond.y - SearchEngine::Command::rowToInt(cmdStart.d1()));
    forbiddenCoords.push_back(agentStart);
    }
    bool taskWillWaitForHelp = false;
    for (auto c : forbiddenCoords) {
        int idx;
        if (boxAt(sharedState, c.x, c.y, &idx)) {
            if (takenTasks_[ctIdx_].task.type == GOAL && idx == takenTasks_[ctIdx_].task.goal.assignedBoxID) {
                continue;
            }
            else if (takenTasks_[ctIdx_].task.type == CLEAR_BOX && idx == takenTasks_[ctIdx_].task.clearBox.boxToMoveID) {
                continue;
            }
            Box b = sharedState->getBoxes()[idx];
            // Only ask for help if the color is not the same as the agent.
            // Otherwise agent can move it.
            if (b.color != this->color) {
                askForHelp(c, 'b', forbiddenCoords, idx);
                taskWillWaitForHelp = true;
            }
            // We can only help ourselves if we can reach the same colored box in question
            else {
               /* short bId = b.id;
                // check if a clear goal is set yet for this box!
                bool add = true;
                for (const auto&t : takenTasks_) {
                    if (t.task.type == CLEAR_BOX && t.task.clearBox.boxToMoveID == bId) {
                        add = false;
                    }
                }
                if (add) {*/
                    ClearBox clearBoxTask = ClearBox(b.id, forbiddenCoords);
                    takenTasks_.push_back(TaskInfo(TaskStackElement(clearBoxTask), nullptr));
                    ctIdx_ = takenTasks_.size() - 1;
                //}
            }
        }
        else if (agentAt(sharedState, c.x, c.y, &idx)) {
            if(idx != num) { 
                askForHelp(c, 'a', forbiddenCoords, idx);
                taskWillWaitForHelp = true;;
            }
        }
    }
    if (taskWillWaitForHelp) {
        takenTasks_[ctIdx_].waitingForHelp = true;
    }
}

void Agent::askForHelp(Coord agentLoc, char hEntryToPerform, std::vector<Coord> forbiddenCoords, int idx) {
    Communication::HelpEntry* entry;
    if (!takenTasks_[ctIdx_].waitingForHelp) {
        if (hEntryToPerform == 'b') {
            if (blackboard_->getHelpEntries().size() > 0) {
                for(auto* entry: blackboard_->getHelpEntries()) {
                    Communication::HelpEntry* entry_casted = (Communication::HelpEntry*) entry;
                    if (entry_casted->getBlockingBoxId() == idx) {
                        return;
                    }
                }
            }
            entry = Communication::HelpEntry::create(agentLoc, Communication::HelpEntry::Box, forbiddenCoords, sharedTime, *this, blackboard_);
            entry->setBlockingBoxId(idx);
            takenTasks_[ctIdx_].hEntryToMonitor.push_back(entry);
        }
        else {
            if (blackboard_->getHelpEntries().size() > 0) {
                for(auto* entry: blackboard_->getHelpEntries()) {
                    Communication::HelpEntry* entry_casted = (Communication::HelpEntry*) entry;
                    if (entry_casted->getBlockingAgentId() == idx) {
                        return;
                    }
                }
            }
            entry = Communication::HelpEntry::create(agentLoc, Communication::HelpEntry::Agent, forbiddenCoords, sharedTime, *this, blackboard_);
            entry->setBlockingAgentId(idx);
            entry->setBlockingBoxId(-1);
            takenTasks_[ctIdx_].hEntryToMonitor.push_back(entry);
        } 
    }
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

std::vector<SearchEngine::State *> Agent::searchClearSelf(SearchEngine::Strategy &strategy, bool ignoreOthers)
{
    if(!ignoreOthers)
        configurePrivateInitialState();
    else
        initialStateRemovedAllBut(num, -1);
    SearchEngine::SearchCli searcher(private_initialState);

    if (takenTasks_[ctIdx_].hEntryToPerform != nullptr) {
        int boxId = takenTasks_[ctIdx_].hEntryToPerform->getBlockingBoxId(); 
        searcher.setGoalStatePredicate([this, boxId](const SearchEngine::State *currentState){
            return isAgentNotOnForbiddenPath(currentState, num, takenTasks_[ctIdx_].task.clearSelf.forbiddenPath); //&&
                isBoxNotOnForbiddenPath(currentState, boxId, takenTasks_[ctIdx_].task.clearSelf.forbiddenPath);
        });
    
    }
    else {
        searcher.setGoalStatePredicate([this](const SearchEngine::State *currentState){
            return isAgentNotOnForbiddenPath(currentState, num, takenTasks_[ctIdx_].task.clearSelf.forbiddenPath);
        });
    }

    return searcher.search(strategy, (int) num);
}

std::vector<SearchEngine::State *> Agent::searchClearBox(SearchEngine::Strategy &strategy, bool ignoreOthers)
{
    int boxID = takenTasks_[ctIdx_].task.clearBox.boxToMoveID;
    if(!ignoreOthers)
        configurePrivateInitialState();
    else
        initialStateRemovedAllBut(num, boxID);
    
    SearchEngine::SearchCli searcher(private_initialState);

    // This takes forever if we don't have a way of saying how far box is from a good spot. 
    // BFS for the target and do a move box to target heuristic
    searcher.setGoalStatePredicate([this, boxID](const SearchEngine::State *currentState){
        //return currentState->getBoxes()[boxID].loc == this->getCurrentTask().clearBox.target;
        return isBoxNotOnForbiddenPath(currentState, boxID, this->takenTasks_[ctIdx_].task.clearBox.forbiddenPath);
    });

    return searcher.search(strategy, (int) num);
}

std::vector<SearchEngine::State *> Agent::searchClearAgent(SearchEngine::Strategy &strategy, bool ignoreOthers)
{
    configurePrivateInitialState();
    SearchEngine::SearchCli searcher(private_initialState);

    searcher.setGoalStatePredicate([this](const SearchEngine::State *currentState){
        return isAgentNotOnForbiddenPath(currentState, this->num, this->getCurrentTask().clearSelf.forbiddenPath);
    });

    return searcher.search(strategy, (int) num);
}

void Agent::setSharedState(SearchEngine::State *sharedState) {
    Agent::sharedState = sharedState;
}

SearchEngine::Command Agent::nextMove() {
    //std::cerr << "AGENT "  <<  (int)num << std::endl;

    for (size_t i = 0; i < takenTasks_.size(); i++) {
        TaskInfo& t = takenTasks_[i];
        if (isTaskSatisfied(sharedState, t.task)) {
            for (size_t j = 0; j < t.hEntryToMonitor.size(); j++) {
                Communication::BlackboardEntry::revoke(t.hEntryToMonitor[j], *this);
            }
            t.hEntryToMonitor.clear();
        }
        else {
            for (size_t j = 0; j < t.hEntryToMonitor.size(); j++) {
                if (t.hEntryToMonitor[j]->getProblemType() == Communication::HelpEntry::ProblemType::Done) {
                    Communication::BlackboardEntry::revoke(t.hEntryToMonitor[j], *this);
                    t.hEntryToMonitor[j] = t.hEntryToMonitor.back();
                    t.hEntryToMonitor.pop_back();
                    j--;
                }
            }
        }
        if (t.hEntryToMonitor.size() == 0) {
            t.waitingForHelp = false;
        }
    }
    
    /* If plan is empty, need to construct a new plan */
    if (plan_.empty()) {
        for (size_t i = 0; i < takenTasks_.size(); i++) {
            if (isTaskSatisfied(sharedState, takenTasks_[i].task) && takenTasks_[i].task.type != GOAL) {
                takenTasks_[i] = takenTasks_.back();
                takenTasks_.pop_back();
                ctIdx_ = -1;
            }
        }
        // Find the next goal to complete
        if (ctIdx_ == -1 || isTaskSatisfied(sharedState, takenTasks_[ctIdx_].task) || 
            takenTasks_[ctIdx_].waitingForHelp) {
            if (!determineNextGoal()) {
                noFoundGoals_++;
                // no goal. if we are in a dead end, leave
                if (SearchEngine::Master::deadends.isDeadend(sharedState->getAgents()[num].loc) && noFoundGoals_ < 3) {
                    leaveDeadend();
                    return SearchEngine::Command();
                }
                else {
                    return SearchEngine::Command(); // no goals so send a NoOp back
                }
            }
            else {
                noFoundGoals_ = 0;
            }
        }
        std::vector<SearchEngine::State*> ans;
        // Gets the current amount of goals satisfied in the current state
        correctGoals_ = SearchEngine::Predicate::getCorrectGoals(sharedState);

        // Search to find the answer for the goal
        bool searchFailed = false;
        ans = conductSubgoalSearch(&searchFailed);
        if(searchFailed) {
            identifyBlockingObjects(ans);
            //std::rotate(takenTasks_.begin(), takenTasks_.begin() + 1, takenTasks_.end());
            if (SearchEngine::Master::deadends.isDeadend(sharedState->getAgents()[num].loc)) {
                leaveDeadend();
                return SearchEngine::Command();
            }
            else {
                return SearchEngine::Command(); // no goals so send a NoOp back
            }
        }

        // Extract the plan from the search
        extractPlan(ans);

        // Post entries to the blackboard
        postAllPositionEntries(ans);
    }

    if (plan_.empty()) {
        if (SearchEngine::Master::deadends.isDeadend(sharedState->getAgents()[num].loc)) {
            leaveDeadend();
            return SearchEngine::Command();
        }
        else {
            return SearchEngine::Command(); // no goals so send a NoOp back
        }
    }

    SearchEngine::Command ret = plan_[0];
    plan_.erase(plan_.begin());

    return ret;
}

bool Agent::isEntryDoable(const Communication::BlackboardEntry *entry, const SearchEngine::State* state, int *boxIndex) {
    int goalIndex = -1;
    SearchEngine::Predicate::goalAt(state, entry->getLocation().x, entry->getLocation().y, &goalIndex);
    Goal entryGoal = SearchEngine::State::goals[goalIndex];

    Box b = sharedState->getBoxes()[entryGoal.assignedBoxID];

    if (b.color != color || 
            Heur::DistanceOracle::fetchDistFromCoord(
            b.loc, state->getAgents()[num].loc) == (unsigned long)-1) {
        return false;
    }

    auto& sOrder = SearchEngine::StrictOrdering::strictOrderings;
    for (int gIdx : sOrder[goalIndex]) {
        if (!goalHasCorrectBox(sharedState, SearchEngine::State::goals[gIdx])) {
            return false;
        }
    }

    // Would satisfying this goal break map into separate regions
    for (Goal& g : takenGoals) {
        State::walls[g.loc.y][g.loc.x] = true;
    }

    Region reg(sharedState);
    size_t numRegBefore = reg.numRegions_;
    State::walls[entryGoal.loc.y][entryGoal.loc.x] = true;
    reg = Region(sharedState);
    size_t numRegAfter = reg.numRegions_;
    State::walls[entryGoal.loc.y][entryGoal.loc.x] = false;

    for (Goal& g : takenGoals) {
        State::walls[g.loc.y][g.loc.x] = false;
    }

    if (numRegBefore != numRegAfter) {
        for (Goal&g : State::goals) {
            if (g.loc != entryGoal.loc) {
                if (!reg.isInSameRegion(g.loc, sharedState->getBoxes()[g.assignedBoxID].loc)) {
                   // std::cerr << "Goal " << entryGoal.letter << " would split goal " << 
                   // g.letter << " from its goal\n";
                    return false;
                }
            }
        }
    }
  //  std::cerr << "Taking goal " << entryGoal.letter << std::endl;
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
    size_t targetBoxIdx = -1;
    if (takenTasks_[ctIdx_].task.type == GOAL) {
        targetBoxIdx =  takenTasks_[ctIdx_].task.goal.assignedBoxID;
    }
    else if (takenTasks_[ctIdx_].task.type == CLEAR_BOX) {
        targetBoxIdx = takenTasks_[ctIdx_].task.clearBox.boxToMoveID;
    }
    for (size_t id = 0; id < private_initialState->getBoxes().size(); id++) {
        auto boxPositionEntries = blackboard_->getBoxEntriesByID(id);
        if(targetBoxIdx == id || boxPositionEntries.size() == 1)
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

    // If the blackboard is empty, no goal can be retrieved
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
    SearchEngine::Predicate::goalAt(sharedState, selectedEntry->getLocation().x, selectedEntry->getLocation().y, &goalIndex);

    Goal &result = SearchEngine::State::goals[goalIndex];
    takenGoals.push_back(result);
    TaskStackElement task = TaskStackElement(result);
    TaskInfo tInfo = TaskInfo(task, nullptr);
    takenTasks_.push_back(tInfo);
    ctIdx_ = takenTasks_.size() - 1;
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

bool Agent::determineNextGoal() {

    // No unsatisfied goals. Check the blackboard
    if (blackboard_->getHelpEntries().size() > 0) {

        for(auto* entry: blackboard_->getHelpEntries()) {
            Communication::HelpEntry* entry_casted = (Communication::HelpEntry*) entry;

            if(entry_casted->getProblemType() == Communication::HelpEntry::Agent &&
                entry_casted->getBlockingAgentId() == getIndex() ) {

                entry_casted->setProblemType(Communication::HelpEntry::TakenCareOf);
                TaskStackElement newTask = ClearSelf(entry_casted->getForbiddenPath());
                takenTasks_.push_back(TaskInfo(newTask, entry_casted));
                ctIdx_ = takenTasks_.size() - 1;
                return true;
            }

            else if(entry_casted->getProblemType() == Communication::HelpEntry::Box) {
                Box &box = sharedState->getBoxes()[entry_casted->getBlockingBoxId()];
                if(box.color == color && Heur::DistanceOracle::fetchDistFromCoord(box.loc, sharedState->getAgents()[num].loc) != (unsigned long) -1) {
                    entry_casted->setProblemType(Communication::HelpEntry::TakenCareOf);
                    TaskStackElement newTask = ClearBoxSelf(entry_casted->getBlockingBoxId(), entry_casted->getForbiddenPath());
                    TaskStackElement clrBox = ClearBox(newTask.clearBoxSelf.boxToMoveID, newTask.clearBoxSelf.forbiddenPath);
                    TaskStackElement clrSelf = ClearSelf(newTask.clearBoxSelf.forbiddenPath);
                    TaskInfo clrBoxInfo = TaskInfo(clrBox, nullptr);
                    TaskInfo clrSelfInfo = TaskInfo(clrBox, entry_casted);
                    takenTasks_.push_back(clrBoxInfo);
                    ctIdx_ = takenTasks_.size() - 1;
                    takenTasks_.push_back(clrSelfInfo);
                    return true;
                }
            }
        }
    }

    if (takenTasks_.size() > 1) {
        std::rotate(takenTasks_.begin(), takenTasks_.begin() + 1, takenTasks_.end());
    }

    // Does anyone need help ? We firstly solve any HelpEntry before doing any goal
    int idx = 0;
    for (TaskInfo& t : takenTasks_) {
        if (!isTaskSatisfied(sharedState, t.task) && t.task.type != GOAL) {
            ctIdx_ = idx;
            return true;
        }
        idx++;
    }

    // See if we have any unsatisfied goals that aren't waiting for help
    bool satisfied = true;
    int unsatIdx;
    for (size_t i = 0; i < takenTasks_.size(); i++) {
        TaskStackElement& t = takenTasks_[i].task;
        if (!isTaskSatisfied(sharedState, t)) {
            satisfied = false;
            unsatIdx = i;
            break;
        }
    }

    if (satisfied) {
        Goal g = getGoalFromBlackboard();
        if (!(g == Goal())) {
            return true;
        }
        return false;
    }
    else {
        ctIdx_ = unsatIdx;
        return true;
    }
}
    
    
std::vector<SearchEngine::State*> Agent::conductSubgoalSearch(bool *searchFailed) {
    if (takenTasks_[ctIdx_].task.type == CLEAR_BOX) {
        if (!isTaskSatisfied(sharedState, takenTasks_[ctIdx_].task)) {
            return conductClearBoxSearch(searchFailed);
        }
    }
    else if (takenTasks_[ctIdx_].task.type == CLEAR_SELF) {
        if (!isTaskSatisfied(sharedState, takenTasks_[ctIdx_].task)) {
            return conductClearSelfSearch(searchFailed);
        }
    }
    else if (takenTasks_[ctIdx_].task.type == CLEAR_BOX_AND_SELF) {
        /* Split the task into a CLEAR_BOX then a CLEAR_SELF */
        //std::cerr << "CLEAR_BOX_AND_SELF should not be pushed into takenTasks or"
        //                " set to currentTaskPtr_. It should be split into 2 tasks\n";
        exit(1);
    }
    else if (takenTasks_[ctIdx_].task.type == GOAL) {
        if (!isTaskSatisfied(sharedState, takenTasks_[ctIdx_].task)) {
            return conductGoalSearch(searchFailed);
        }
    }
    else {
        *searchFailed = false;
    }

    return std::vector<SearchEngine::State*>();
}

std::vector<SearchEngine::State*> Agent::conductGoalSearch(bool* searchFailed) {
    std::vector<SearchEngine::State*> ans = std::vector<SearchEngine::State*>();
    Box &targBox = sharedState->getBoxes()[takenTasks_[ctIdx_].task.goal.assignedBoxID];

    if (!agentNextToBox(sharedState, targBox, this)) {

        //std::cerr << "Getting to box " << targBox.id << " (" << targBox.loc.x << ", " << targBox.loc.y << ")" << 
        //    " from " << sharedState->getAgents()[num].loc.x << "," << sharedState->getAgents()[num].loc.y << std::endl;

        Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic> strat(this);
        strat.linkBlackboard(blackboard_);
        strat.setMaxIterations(1000);
        strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
            std::string errorDescription;
            return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                state->getAction() ,state->getTimeStep(), errorDescription);
        });

        ans = searchBox(targBox, strat); //TODO reflect proper strategy
    }
    else {
        //std::cerr << "Agent " << (int)this->num << " will satisfy goal (" << takenTasks_[ctIdx_].task.goal.loc.x << ", " << takenTasks_[ctIdx_].task.goal.loc.y << ") with box " << targBox.id << std::endl;
        Strat::StrategyHeuristic<Heur::BoxToGoalAStarHeuristic> strat(this);
        strat.linkBlackboard(blackboard_);
        strat.setMaxIterations(1000);
        strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
            std::string errorDescription;
            return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                state->getAction(), state->getTimeStep(), errorDescription);
        });
        ans = searchGoal(takenTasks_[ctIdx_].task.goal, strat);
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
            ans = searchGoal(takenTasks_[ctIdx_].task.goal, strat, true);
        }
    }
    return ans;
}
std::vector<SearchEngine::State*> Agent::conductClearBoxSearch(bool* searchFailed) {
    std::vector<SearchEngine::State*> ans = std::vector<SearchEngine::State*>();
    Box &targBox = sharedState->getBoxes()[takenTasks_[ctIdx_].task.clearBox.boxToMoveID];

    if (!agentNextToBox(sharedState, targBox, this)) {
        Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic> strat(this);
        strat.linkBlackboard(blackboard_);
        strat.setMaxIterations(1000);
        strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
            std::string errorDescription;
            return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                state->getAction() ,state->getTimeStep(), errorDescription);
        });
        ans = searchBox(targBox, strat);
    }
   /* else {
        Coord target = findBoxTargetWithBFS(targBox.id);
        if (target.x != -1 ) {
            takenTasks_[ctIdx_].task.clearBox.target = target;
            Strat::StrategyHeuristic<Heur::BoxToTargetAStarHeuristic> strat(this);
            strat.linkBlackboard(blackboard_);
            strat.setMaxIterations(1000);
            strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
                std::string errorDescription;
                return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                    state->getAction() ,state->getTimeStep(), errorDescription);
            });
            ans = searchClearBox(strat);
        }
    }*/
    
    else {
        Strat::StrategyBFS strat;
        strat.linkBlackboard(blackboard_);
        strat.setMaxIterations(50000);
        strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
            std::string errorDescription;
            return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                state->getAction() ,state->getTimeStep(), errorDescription);
        });
        ans = searchClearBox(strat); 
    }
    

    if (ans.size() == 0) {
        *searchFailed = true;
        if(!agentNextToBox(sharedState, targBox, this)) {
            Strat::StrategyHeuristic<Heur::AgentToBoxAStarHeuristic> strat(this);
            strat.linkBlackboard(nullptr);
            strat.setMaxIterations(1000);
            ans = searchBox(targBox, strat, true);
        }
        else {
           /* Coord target = findBoxTargetWithBFS(targBox.id);
            if (target.x != -1 ) {
                takenTasks_[ctIdx_].task.clearBox.target = target;
                Strat::StrategyHeuristic<Heur::BoxToTargetAStarHeuristic> strat(this);
                strat.linkBlackboard(blackboard_);
                strat.setMaxIterations(1000);
                strat.setAdditionalCheckPredicate([this](const SearchEngine::State* state) {
                    std::string errorDescription;
                    return positionFree(state->getAgents()[num].loc.x, state->getAgents()[num].loc.y, 
                                        state->getAction() ,state->getTimeStep(), errorDescription);
                });
                ans = searchClearBox(strat);
            }*/
            Strat::StrategyBFS strat;
            strat.linkBlackboard(nullptr);
            strat.setMaxIterations(1000);
            ans = searchClearBox(strat);
        }
    }
    return ans;
}

std::vector<SearchEngine::State*> Agent::conductClearSelfSearch(bool* searchFailed) {
    std::vector<SearchEngine::State*> ans;
    Strat::StrategyBFSMovePriority strat;
    strat.linkBlackboard(blackboard_);
    ans = searchClearSelf(strat);
    if (ans.size() == 0) {
        *searchFailed = true;
        Strat::StrategyBFSMovePriority strat;
        strat.linkBlackboard(nullptr);
        ans = searchClearSelf(strat, true);
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

bool Agent::isTaskSatisfied(SearchEngine::State* state, TaskStackElement& t) {
    if (t.type == GOAL) {
        return goalHasCorrectBox(sharedState, t.goal);
    }
    else if (t.type == CLEAR_BOX) {
        return isBoxNotOnForbiddenPath(sharedState, t.clearBox.boxToMoveID, t.clearBox.forbiddenPath);
    }
    else if (t.type == CLEAR_SELF) {
        return isAgentNotOnForbiddenPath(sharedState, num, t.clearSelf.forbiddenPath);
    }
    else if (t.type == NIL) {
        return true;
    }
    else {
        return false; // Clear box and self tasks are always false because they split into
                        // two different tasks, clear box and then clear self. This should never be run
    }
}

Coord Agent::findBoxTargetWithBFS(int boxId) {
    std::queue<Coord> q;
    std::vector<std::vector<bool>> seen = std::vector<std::vector<bool>> (height(sharedState), std::vector<bool>());
    for (size_t i = 0; i < height(sharedState); i++) {
        seen[i] = std::vector<bool>(width(sharedState, i), false);
    }

    Coord currCoord = sharedState->getAgents()[boxId].loc;
    q.push(currCoord);

    bool foundATarget = false;
    while (!q.empty()) {
        currCoord = q.front();
        q.pop();
        std::vector< Coord > coords = {
            Coord(currCoord.x - 1, currCoord.y), Coord(currCoord.x, currCoord.y + 1),
            Coord(currCoord.x + 1, currCoord.y), Coord(currCoord.x, currCoord.y - 1)
        };
                
        if (isCoordNotOnForbiddenPath(sharedState, currCoord, takenTasks_[ctIdx_].task.clearBox.forbiddenPath)) {
            foundATarget = true;
            break;
        }

        for (const Coord &c : coords) {
            if (SearchEngine::Predicate::inBound(sharedState, c.x, c.y) &&
                !SearchEngine::Predicate::wallAt(sharedState, c.x, c.y) &&
                !seen[c.y][c.x] ){
                seen[c.y][c.x] = true;
                q.push(c);
            }
        }
    }
    
    if (!foundATarget) {
        return Coord();
    }

    // Go as deep as possible
  /*  if (SearchEngine::Master::deadends.isDeadend(currCoord)) {
        bool gotDeeper = true;
        while (gotDeeper) {
            gotDeeper = false;
            std::vector< Coord > coords = {
                Coord(currCoord.x - 1, currCoord.y), Coord(currCoord.x, currCoord.y + 1),
                Coord(currCoord.x + 1, currCoord.y), Coord(currCoord.x, currCoord.y - 1)
            };

            for (const auto& c : coords) {
                if (SearchEngine::Predicate::inBound(sharedState, c.x, c.y) &&
                    !SearchEngine::Predicate::wallAt(sharedState, c.x, c.y) &&
                    !seen[c.y][c.x]){
                    currCoord = c;
                    gotDeeper = true;
                    seen[c.y][c.x] = true;
                }
            }
        }        
    }*/
    return currCoord;   
}

void Agent::leaveDeadend() {

    std::queue<Coord> q;
    std::vector<Coord> deadEndCoords;
    std::vector<std::vector<bool>> seen = std::vector<std::vector<bool>> (height(sharedState), std::vector<bool>());

    for (size_t i = 0; i < height(sharedState); i++) {
        seen[i] = std::vector<bool>(width(sharedState, i), false);
    }

    Coord currCoord = sharedState->getAgents()[num].loc;
    q.push(currCoord);

    while (!q.empty()) {
        currCoord = q.front();
        q.pop();

        std::vector< Coord > coords = {
            Coord(currCoord.x - 1, currCoord.y), Coord(currCoord.x, currCoord.y + 1),
            Coord(currCoord.x + 1, currCoord.y), Coord(currCoord.x, currCoord.y - 1)
        };


        for (const Coord &c : coords) {
            if (SearchEngine::Predicate::inBound(sharedState, c.x, c.y) &&
                !SearchEngine::Predicate::wallAt(sharedState, c.x, c.y) &&
                SearchEngine::Master::deadends.isDeadend(c) &&
                !seen[c.y][c.x] ){
                seen[c.y][c.x] = true;
                deadEndCoords.push_back(c);
                q.push(c);
            }
        }
    }

    bool deadEndIsSolved = true;
    for (Coord& c : deadEndCoords) {
        int gIdx, bIdx;
        if (goalAt(sharedState, c.x, c.y, &gIdx)) {
            if (!goalHasCorrectBox(sharedState, SearchEngine::State::goals[gIdx])) {
                deadEndIsSolved = false;
                break;
            }
        }
        else if (boxAt(sharedState, c.x, c.y, &bIdx)) {
            if (sharedState->getBoxes()[bIdx].color != color) {
                deadEndIsSolved = false;
                break;
            }
        }
    }

    if (!deadEndIsSolved) {
        configurePrivateInitialState();
        std::vector<SearchEngine::State*> ans;
        Strat::StrategyBFSMovePriority strat;
        strat.linkBlackboard(blackboard_);
        SearchEngine::SearchCli searcher(private_initialState);
        searcher.setGoalStatePredicate([this, deadEndCoords](const SearchEngine::State *currentState){
            return isAgentNotOnForbiddenPath(currentState, num, deadEndCoords);
        });
        auto stateSol = searcher.search(strat, (int) num);
        extractPlan(stateSol);
    }
}