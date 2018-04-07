#include "client.h"

using SearchClient::Client;
using SearchEngine::State;
using SearchEngine::Command;


Client::Client(): jointAction(), actionsRecv(0), agents() { }

void Client::setAction(int agentId, Command command) {
    jointAction[agentId] = command;
    actionsRecv++;
    if (actionsRecv == State::numAgents) {
        send();
        actionsRecv = 0;
    }
}


State Client::initState() {
    State state;
    std::string s;
    
    int row = 0, numAgents = 0;

    std::vector<AgentDescription> agentsDescription = std::vector<AgentDescription>();
    std::vector<Box> boxes = std::vector<Box>();
    std::vector<Goal> goals = std::vector<Goal>();

    std::unordered_map<char, Color> mapping;

    while(std::getline(std::cin, s)) {
        // We have finished reading the state
        if (s == "") break;

        // If line contains ":", we are describing colors for the boxes and agents
        size_t colonPosition = s.find(':');
        if (colonPosition != std::string::npos) {
            std::string::iterator end_pos = std::remove(s.begin(), s.end(), ' ');
            s.erase(end_pos, s.end());
            colonPosition = s.find(':');
            std::string sColor(s, 0, colonPosition);
            std::string sElements(s, colonPosition + 1);
            std::string token;

            Color currColor = stringToColor(sColor);
            std::istringstream iss(sElements);

            while (std::getline(iss, token, ',')) {
                char id = token[0];
                mapping.insert(std::make_pair(id, currColor));
            }
        }

        else {

            // else line is part of level
            std::vector<bool> lineWall = std::vector<bool>();

            for(size_t i = 0; i < s.length(); i++) {
                Coord currCoord = Coord(i, row);

                // set wall
                lineWall.push_back(s[i] == '+');

                // set Goal
                if (s[i]>= 'a' && s[i]<='z') {
                    std::cerr << "Added goal" << std::endl;
                    goals.push_back( Goal( toupper(s[i]), currCoord) );
                }

                // set agent(s)' position
                if (s[i]>= '0' && s[i]<='9') {
                    Color color = BLUE;
                    if (mapping.find(s[i]) != mapping.end()) {
                        color = mapping[s[i]];
                    }
                    int num = s[i] - '0';
                    agentsDescription.push_back( {color, num, currCoord} );

                    agents.push_back( SearchEngine::Agent(color, num, currCoord) );
                    numAgents++;
                }

                // set box
                if (s[i] >= 'A' && s[i] <= 'Z') {
                    Color color = BLUE;
                    if (mapping.find(s[i]) != mapping.end()) {
                        color = mapping[s[i]];
                    }
                    Box box = Box(color, s[i], currCoord);
                    boxes.push_back(box);
                }

            }
            State::walls.push_back(lineWall);
            row++;
        }
    }
    State::numAgents = numAgents;
    State::goals = goals;
    state.setAgents(agentsDescription);
    state.setBoxes(boxes);
    return state;
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

Color stringToColor(std::string c) {
    std::transform(c.begin(), c.end(), c.begin(), toupper);

    Color ret = (c == "BLUE")       ? BLUE      :
                (c == "RED")        ? RED       :
                (c == "GREEN")      ? GREEN     :
                (c == "CYAN")       ? CYAN      :
                (c == "MAGENTA")    ? MAGENTA   :
                (c == "ORANGE")     ? ORANGE    :
                (c == "PINK")       ? PINK      :   YELLOW;

    return ret;
}

const std::vector<SearchEngine::Agent>& Client::extractAgents() const {
    return agents;
}

std::vector<SearchEngine::Agent>& Client::extractAgents() {
    return agents;
}