#include <iostream>
#include <vector>
#include "./searchclient/state.h"
#include "./searchclient/strategy.h"
#include "./searchclient/searchclient.h"
#include "./searchclient/typedefs.h"
#include "./searchclient/command.h"

int main() {

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

    SearchEngine::SearchClient client(dummyState);
    SearchEngine::StrategyBFS strat;
    std::vector<SearchEngine::State*> plan = client.search(strat, 0);

    std::cout << "Computed plan (Size = " << plan.size() << ")" << std::endl;
    for(SearchEngine::State *state: plan) {
        std::cout << state->getAction().toString() << std::endl;
    }
}
