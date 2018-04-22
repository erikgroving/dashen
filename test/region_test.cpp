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

TEST(RegionTests, RegionTest1) {
    std::cerr << "starting RegionTest1" << std::endl;
    Direction d1 = E;
    EXPECT_EQ(d1, E);
    Command command(MOVE, E);
    EXPECT_EQ(command.toString(), "Move(E)");

    SearchEngine::State initialState;
    SearchEngine::State::walls = {
        {true, true, true, true, true, true, true, true, true, true},
        {true, false, false, false, false, false, true, false, false, true},
        {true, true, true, true, true, true, true, true, false, true},
        {true, false, false, false, false, true, false, false, false, true},
        {true, true, true, true, true, true, true, true, true, true},
    };
    
    
    SearchEngine::Region *region = new SearchEngine::Region(&initialState);
    std::cerr << "region initialization done" << std::endl;
    auto result = region->getRegionMatrix();
    std::cerr << "getRegionMatrix done" << std::endl;

    printRegions(result);
    std::cerr << "printRegions done" << std::endl;

    delete region;
}