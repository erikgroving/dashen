#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>
#include <thread>

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
        globalStrategy = new Strat::StrategyGreedy();
    }
    else if(searchStrategy == "--astar") {
    //    globalStrategy = new Strat::StrategyHeuristic<Heuristic::BasicAStar>();
    }
    else {
        std::cerr << "Unrecognized search strategy" << searchStrategy << std::endl;
        std::cerr << "The execution is aborted." << std::endl;
        return 2;
    }

#ifndef SERVERLESS_MODE

    SearchClient::Client searchClient(globalStrategy);
    SearchEngine::State initialState = searchClient.initState();
    SearchClient::Agent::setSharedState(&initialState);
    auto agents = searchClient.extractAgents();

#else

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
    
#endif    
    
// SingleAgent mode
#ifndef SERVERLESS_MODE   

    if(searchClient.getProblemType() == SearchClient::Client::SingleAgent) {
        
        std::cerr << "Single agent mode has been identified" << std::endl;
        std::cerr << "Starting the search..." << std::endl;
        //agents[0].makeSearch();
        SearchEngine::Master master(initialState, agents);
        master.conductSearch();

        std::cerr << "Search over. Sending the computed to the server... (Size = " << searchClient.getCurrentActionPlan().size() << ")";
        //searchClient.send( searchClient.getCurrentActionPlan() );
        std::cerr << " Complete !" << std::endl;
    }

#else

    if(initialState.getAgents().size() == 1) {
        SearchEngine::SearchClient client(&initialState, DEBUG_MODE);
        std::vector<SearchEngine::State*> plan = client.search(*globalStrategy, 0);
        int i = 0;
        for(SearchEngine::State *step: plan) 
            std::cout << "Step " << ++i << ": " << step->getAction().toActionString() << std::endl;        
    }
#endif
    /*
    else if(searchClient.getProblemType() == SearchClient::Client::MultiAgent) {
        std::cerr << "Error: Multi agent problem solving in not available yet" << std::endl;
    }
    else {
        std::cerr << "The level that was transmitted does not contain any agent." << std::endl;
    }
    */

    /* Solely to allow printing to finish */
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cerr.flush();
    delete globalStrategy;
    return 0;
}
