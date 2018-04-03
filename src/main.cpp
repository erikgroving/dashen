#include <iostream>
#include <vector>

#include "searchclient/state.h"
#include "searchclient/client.h"
#include "searchclient/searchclient.h"
#include "searchclient/typedefs.h"
#include "searchclient/command.h"
#include "strategies/strategybfs.h"


int main() {

    SearchClient::Client searchClient;
    SearchEngine::State dummyState2 = searchClient.initState();
    Strategy::StrategyBFS strat;

    SearchEngine::SearchClient sc(&dummyState2);
    auto plan = sc.search(strat, 0);

    std::cerr << "Computed plan (Size = " << plan.size() << ")" << std::endl;
    for(auto *state: plan) {
        std::cout << state->getAction().toString() << std::endl;
    }

    return 0;
/*
    SearchEngine::State dummyState;

    SearchEngine::State::walls = {
        {true, true, true, true, true},
        {true, false, false, false, true},
        {true, false, false, false, true},
        {true, true, true, true, true},
    };

    SearchEngine::State::goals = std::vector<Goal>{
        Goal('A', Coord(1,1))
    };

    dummyState.setAgents({ Agent(YELLOW, '0', Coord(3,2)) });

    dummyState.setBoxes({ Box(YELLOW, 'A', Coord(2,1))});

    std::cerr << "Pre-copy\n";
    //SearchEngine::SearchClient client(dummyState);
    SearchEngine::SearchClient client(dummyState2);
    std::cerr << "Post-copy\n";
    Strategy::StrategyBFS strat;
    std::vector<SearchEngine::State*> plan = client.search(strat, 0);

    std::cerr << "Computed plan (Size = " << plan.size() << ")" << std::endl;
    for(SearchEngine::State *state: plan) {
        std::cout << state->getAction().toString() << std::endl;
    }

    */
}
