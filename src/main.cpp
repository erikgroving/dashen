
#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
#include <fstream>

#include "searchclient/searchclient"
#include "searchengine/searchengine"
#include "strategies/strategies"
#include "heuristics/astarheuristic.h"
#include "searchengine/master.h"

#define DEBUG_MODE true
// #define SERVERLESS_MODE

int main(int argc,  char **argv) {

    std::cerr << "DASHEN Artificial Intelligence Tool for 02285- Artificial Intelligence and Multi-Agent System " << std::endl;


    SearchClient::Client searchClient;
#ifndef SERVERLESS_MODE
    SearchEngine::State initialState = searchClient.initState();
#else
    std::ifstream input_stream("./environment/levels/MAsimple3.lvl");
    SearchEngine::State initialState = searchClient.initState(input_stream);
#endif

    SearchClient::Agent::setSharedState(&initialState);
    auto agents = searchClient.extractAgents();
    
    std::cerr << "Starting the search..." << std::endl;
    SearchEngine::Master master(initialState, agents);
    master.conductSearch();

    return 0;
}
