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
    State state = State();
    std::string s;
    bool inLevel = false;
    int plusCount = 0;
    int row = 0;

    while(std::getline(std::cin, s)) {
        std::vector<bool> lineWall = std::vector<bool>();
        std::vector<Goal> lineGoal = std::vector<Goal>();

        for(int i=0; i<s.length(); i++) {
            Coord currCoord = Coord(row, i);
            
            // set wall
            if(s[i] == '+') {
                lineWall.push_back(true);
            } else {
                lineWall.push_back(false);
            }

            // set Goal
            if (s[i]>= 'a' && s[i]<='z') {
                Goal goal = Goal(s[i], currCoord);
                lineGoal.push_back(goal);
            } else {
                Goal goal = Goal(0, currCoord);
                lineGoal.push_back(goal);
            }
            
            // set agent(s)' position
            if (s[i]>= 'A' && s[i]<='Z') {
                Color color = RED;
                int num = 0; // TODO convert s[i] to integer
                Agent agent = Agent(color, num, currCoord);
                state.agents.push_back(agent);
            }


        }
        state.walls.push_back(lineWall);
        state.goals.push_back(lineGoal);
        row++;
    }    
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