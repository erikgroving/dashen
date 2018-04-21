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
    if(argc <= 1) {
        std::cerr << "No search strategy was specified. The execution is aborted." << std::endl;
        return 1;
    }

    SearchEngine::Strategy *globalStrategy = nullptr;
    std::string searchStrategy(argv[1], strlen(argv[1]));

    if(searchStrategy == "--dfs") {
        globalStrategy = new Strat::StrategyDFS();
    }
    else if(searchStrategy == "--bfs") {
        globalStrategy = new Strat::StrategyBFS();
    }
    else if(searchStrategy == "--greedy") {               
        globalStrategy = new Strat::StrategyBFS(); 
        //globalStrategy = new Strat::StrategyHeurstic<Heuristic::GreedyHeuristic>();
    }
    else if(searchStrategy == "--astar") {
    //    globalStrategy = new Strat::StrategyHeuristic<Heuristic::BasicAStar>();
    }
    else {
        std::cerr << "Unrecognized search strategy" << searchStrategy << std::endl;
        std::cerr << "The execution is aborted." << std::endl;
        return 2;
    }



    SearchClient::Client searchClient(globalStrategy);
#ifndef SERVERLESS_MODE
    SearchEngine::State initialState = searchClient.initState();
#else
    std::ifstream input_stream("./environment/levels/SAsokobanLevel96.lvl");
    SearchEngine::State initialState = searchClient.initState(input_stream);
    SearchEngine::Distance distance(&initialState);
#endif

    SearchClient::Agent::setSharedState(&initialState);
    auto agents = searchClient.extractAgents();
    

    std::cerr << "Starting the search..." << std::endl;
    SearchEngine::Master master(initialState, agents);
    master.conductSearch();

    delete globalStrategy;
    return 0;
}
