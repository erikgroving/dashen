#include "client.h"

using SearchClient::Client;
using SearchClient::Agent;
using SearchClient::JointAction;

using SearchEngine::State;
using SearchEngine::Command;

Client::Client(SearchEngine::Strategy *strategy): 
    type_(None), onGoingJointAction(), actionPlan_(), 
    agents(), actionsRecv(0), searchStrategy_(strategy)  {

    if(strategy == nullptr) {
        std::cerr << "Error: no strategy has been provided to the client." << std::endl;
        std::cerr << "No solving will be executed" << std::endl;
    }
}

void Client::setAction(size_t agentId, const Command &command) {
    
    // Check bound to prevent any out-of-bounds exception
    if( agentId >= agents.size() ) {
        std::cerr << "Wrong agentId " << agentId << "was passed when setting up action" << std::endl;
        return;
    }

    if( getProblemType() == SingleAgent ) {
        onGoingJointAction[agentId] = command;
        saveJointAction();
        resetJointAction();
    }
    else if( getProblemType() == MultiAgent ) {
        for (int i = 0; i < State::numAgents; i++) {

        }
    }
    else {
        std::cerr << "Cannot set a move in a invalid problem" << std::endl;
    }
    actionsRecv++;
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
                    char num = s[i] - '0';
                    agentsDescription.push_back( {color, num, currCoord} );

                    agents.push_back( Agent(color, num, currCoord, searchStrategy_, this) );
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

    resetJointAction();
    switch(State::numAgents) {
        case 0: break;
        case 1: 
            type_ = SingleAgent; 
            break;
        default: 
            type_ = MultiAgent;
            break;
    }
    
    return state;
}

bool Client::send(const std::vector<JointAction> &plan, size_t *failingStep) {

    size_t step = 0;
    for(const JointAction& action: plan) {
        bool isStepOkay = sendStep(action);
        if(!isStepOkay) {
            if(failingStep != nullptr) *failingStep = step;
            return false;
        }
        step++;
    }
    return true;
}

bool Client::sendStep(const JointAction &jointAction) {
    
    // If no action has been scheduled, cancel the sending and return that the action has failed.
    if(jointAction.getData().size() == 0) {
        return false;
    }
    
    // Construct the joint action string
    std::string message = "[" + jointAction.getData()[0].toString();
    for(auto ite =  jointAction.getData().begin() + 1; ite != jointAction.getData().end(); ite++)
        message += "," + (*ite).toString();
    message += "]";

    // Send the joint action on the standard output
    // std::cerr << message;
    std::cout << message << std::endl; 

    // Listen for the response of the server
    std::string returnMsg;
    std::cin >> returnMsg;

    // Return the response code of the action
    if(returnMsg == "[true]")
        return true;
    else if(returnMsg == "[false]") return false;
    else {
        std::cerr << "The server answered with a unintended response: " << returnMsg << std::endl;
        return false;
    }
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


void Client::saveJointAction() {
    actionPlan_.emplace_back(onGoingJointAction);
    std::cerr << actionPlan_[0].getData().size();
}

void Client::resetJointAction() {
    
    onGoingJointAction = JointAction();
    onGoingJointAction.initialize(State::numAgents);

}
