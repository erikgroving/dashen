#include <iostream>
#include <vector>

#include "searchclient/state.h"
#include "searchclient/client.h"
#include "searchclient/searchclient.h"
#include "searchclient/typedefs.h"
#include "searchclient/command.h"
#include "strategies/strategybfs.h"

using namespace SearchEngine::Predicate;
void printMapMain(const SearchEngine::State *state) {
    for(int i = 0; inBound(state, 0, i); i++) {
        for(int j = 0; inBound(state, j, i); j++) {
            int index;
            if(wallAt(state, j, i))
                std::cerr << "+"; 
            else if(agentAt(state, j, i, &index))
                std::cerr << "0" /* state->getAgents()[index].num */ ;
            else if(boxAt(state, j, i, &index))
                std::cerr << state->getBoxes()[index].letter;
            else if(goalAt(state, j, i, &index))
                std::cerr << "a" /* SearchEngine::State::goals[index].letter */;
            else
                std::cerr << " ";
        }

        std::cerr << std::endl;
    }
}

int main() {

    SearchClient::Client searchClient;
    SearchEngine::State dummyState2 = searchClient.initState();
    auto agents = searchClient.extractAgents();
    std::cerr << "AGENT 0: (" << agents[0].getLocation().x << ", " << agents[0].getLocation().y << ")" << std::endl;
    SearchEngine::SearchClient client(&dummyState2, true);
    
    /*
    SearchEngine::State dummyState;
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
    dummyState.setAgents(std::vector<AgentDescription>{ AgentDescription{YELLOW, '0', Coord(6,1)} });
    dummyState.setBoxes({ Box(YELLOW, 'A', Coord(7,1))});
    SearchEngine::SearchClient client(&dummyState, true); 
    */

    Strategy::StrategyBFS strat;
    std::vector<SearchEngine::State*> plan = client.search(strat, 0);

    // std::cerr << "Computed plan (Size = " << plan.size() << ")" << std::endl;
    for(SearchEngine::State *state: plan) {
        // std::cerr << state->getAction().toActionString() << std::endl;
        std::cout << state->getAction().toActionString() << std::endl;
        //printMapMain(state);
    }

    std::cerr << "Complete!\n";
}
