#include "client.h"
#include "state.h"
#include "typedefs.h"

#include <iostream>
#include <string>
#include <regex>

using SearchClient::Client;
using SearchEngine::State;
using SearchEngine::Command;

Client::Client() {
    jointAction = std::vector<Command>();
    actionsRecv = 0;
    jointAction.resize(State::numAgents, Command());
}

void Client::setAction(int agentId, Command command) {
    jointAction[agentId] = command;
    actionsRecv++;
    if (actionsRecv == State::numAgents) {
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

        // if line contains ":"
        size_t colonPosition = s.find(':');
        if(colonPosition != std::string::npos) {
            std::string sColor = s.substring(s, 0, colonPosition);
            //std::string sElements = s.substring(s, colonPosition);

            std::regex nonlevelRegex("^[a-z]+:\\s*[0-9A-Z](\\s*,\\s*[0-9A-Z])*\\s*$");
            std::smatch stringMatch;
            std::regex_match(s, stringMatch, nonlevelRegex);

            //std::string lineColor = stringMatch[0];
            for (unsigned i=0; i<stringMatch.size(); ++i) {
                std::cerr << stringMatch[i] << std::endl;
            }
        }

        else {

            // else line is part of level
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
                    state.getAgents().push_back(agent);
                }

            }
            State::walls.push_back(lineWall);

            // For later ?
            // State::goals.push_back(lineGoal);
            for(const Goal &goal: lineGoal)
                State::goals.push_back(goal);
            row++;
        }
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
