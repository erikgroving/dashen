#include "client.h"
#include "state.h"
#include "typedefs.h"

using SearchClient::Client;
using SearchEngine::State;

Client::Client() {
    jointAction = std::vector<Command>();
    jointAction.resize(State().getNumAgents(), Command(E));
}

void Client::send() {
    std::string message = "[";
    for (int i = 0; i < jointAction.size()-1; i++) {
        message += jointAction[i].toString() + ",";
    }
    message += jointAction[jointAction.size()-1].toString() + "]";
}