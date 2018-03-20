#include "client.h"
#include "state.h"
#include "typedefs.h"

using SearchClient::Client;
using SearchEngine::State;

Client::Client() {
    jointAction = std::vector<Command>();
    actionsRecv = 0;
    jointAction.resize(State().getNumAgents(), Command(E));
}

void Client::setAction(int agentId, Command command) {
    jointAction[agentId] = command;
    actionsRecv++;
    if (actionsRecv == State().getNumAgents()) {
        send();
        actionsRecv = 0;
    }
}

State Client::initState() {
}

void Client::send() {
    std::string message = "[";
    std::string returnMsg;
    for (int i = 0; i < jointAction.size()-1; i++) {
        message += jointAction[i].toString() + ",";
    }
    message += jointAction[jointAction.size()-1].toString() + "]";
    std::cout << message;
    std::cin >> returnMsg;
    actionsRecv = 0;
}