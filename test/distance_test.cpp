#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "../src/searchengine/searchengine"
#include "../src/strategies/strategies"
#include "../src/searchclient/searchclient"
#include "../src/printMap.h"

using SearchEngine::Command;
using SearchEngine::Strategy;
using SearchClient::Client;

TEST(DistanceTests, DistanceTest1) {
    
    Direction d1 = E;
    EXPECT_EQ(d1, E);
    Command command(MOVE, E);
    EXPECT_EQ(command.toString(), "Move(E)");

    SearchEngine::Strategy *globalStrategy = new Strategy::StrategyBFS();

    SearchEngine::State initialState;
    SearchEngine::State::walls = {
        {true, true, true, true, true, true, true, true, true, true},
        {true, false, false, false, false, false, true, false, false, true},
        {true, false, true, true, true, true, true, true, false, true},
        {true, false, false, false, false, false, false, false, false, true},
        {true, true, true, true, true, true, true, true, true, true},
    };
    SearchEngine::State::goals = std::vector<Goal>{
        Goal('A', Coord(8,1))
    };
    initialState.setAgents(std::vector<AgentDescription>{ AgentDescription{YELLOW, '0', Coord(1,1)} });
    initialState.setBoxes({ Box(YELLOW, 'A', Coord(7,1))});

    SearchEngine::Distance *distance = new SearchEngine::Distance();
    auto result = distance->getDistanceFromPosition(initialState, 1, 1);

    printDistances(result);

    delete distance;

}