#include <gtest/gtest.h>
#include <vector>
#include "../src/searchclient/searchclient"
#include "../src/strategies/strategies"
#include "../src/searchengine/searchengine"

using SearchEngine::Strategy;

TEST(Strategy_Test1, StateCompareTest1) {
    SearchEngine::State::walls = {
        {true, true, true, true, true, true, true, true, true, true},
        {true, false, false, false, false, false, false, false, false, true},
        {true, false, true, true, true, true, true, true, false, true},
        {true, false, false, false, false, false, false, false, false, true},
        {true, true, true, true, true, true, true, true, true, true},
    };
    SearchEngine::State::goals = std::vector<Goal>{
        Goal('A', Coord(8,1))
    };
    
    std::vector<SearchEngine::State*> states;

    for(size_t i = 2; i < 8; i++) {
        SearchEngine::State *state = new SearchEngine::State();
        state->setAgents(std::vector<AgentDescription>{ AgentDescription{YELLOW, '0', Coord(1,1)} });
        state->setBoxes({ Box(YELLOW, 'A', Coord(i,1))});

        states.push_back(state);
    }

    for(SearchEngine::State* state: states) {
        for(SearchEngine::State* st: states)
            if(state != st) {
                ASSERT_EQ(!StateCompare()(state, st), true) << "Two different states have been declared identical by the state comparator";
            }
    }

    for(SearchEngine::State* state: states)
        delete state;
}

TEST(Strategy_Test1, StatesInHashMap) {
    SearchEngine::State::walls = {
        {true, true, true, true, true, true, true, true, true, true},
        {true, false, false, false, false, false, false, false, false, true},
        {true, false, true, true, true, true, true, true, false, true},
        {true, false, false, false, false, false, false, false, false, true},
        {true, true, true, true, true, true, true, true, true, true},
    };
    SearchEngine::State::goals = std::vector<Goal>{
        Goal('A', Coord(8,1))
    };
    SearchEngine::Strategy *myStrategy = new Strategy::StrategyBFS();

    std::vector<SearchEngine::State*> sourceStates;
    SearchEngine::State *state1 = new SearchEngine::State();
    state1->setAgents( std::vector<AgentDescription>{ AgentDescription{YELLOW, '0', Coord(2,1)} } );
    state1->setBoxes ( { Box(YELLOW, 'A', Coord(1,1))} );

    SearchEngine::State *state1copy = new SearchEngine::State();
    state1copy->setAgents( std::vector<AgentDescription>{ AgentDescription{YELLOW, '0', Coord(2,1)} } );
    state1copy->setBoxes ( { Box(YELLOW, 'A', Coord(1,1))} );

    SearchEngine::State *state2 = new SearchEngine::State();
    state2->setAgents( std::vector<AgentDescription>{ AgentDescription{YELLOW, '0', Coord(2,1)} } );
    state2->setBoxes ( { Box(YELLOW, 'A', Coord(3,1))} );

    SearchEngine::State *state2copy = new SearchEngine::State();
    state2copy->setAgents( std::vector<AgentDescription>{ AgentDescription{YELLOW, '0', Coord(2,1)} } );
    state2copy->setBoxes ( { Box(YELLOW, 'A', Coord(3,1))} );


    myStrategy->addToFrontier(state1);
    myStrategy->addToExplored(state2);

    ASSERT_EQ(myStrategy->inFrontier(state1copy), true);
    ASSERT_EQ(myStrategy->isExplored(state2copy), true);
    
    delete state1;
    delete state2;
    delete state1copy;
    delete state2copy;
    delete myStrategy;    
}