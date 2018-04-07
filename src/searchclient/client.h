#ifndef SEARCHENGINE_CLIENT_H
#define SEARCHENGINE_CLIENT_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <regex>
#include <unordered_map>
#include <utility>

#include "command.h"
#include "state.h"
#include "typedefs.h"
#include "agent.h"

Color stringToColor(std::string);

namespace SearchClient {

class Client {
    public:
        Client();

        SearchEngine::State initState();

        std::vector<SearchEngine::Agent>& extractAgents();
        const std::vector<SearchEngine::Agent>& extractAgents() const;

        void setAction(int, SearchEngine::Command);

        void send();                 
    
    private:
        std::vector<SearchEngine::Command> jointAction;
        std::vector<SearchEngine::Agent> agents;
        int actionsRecv;  
};

}

#endif
