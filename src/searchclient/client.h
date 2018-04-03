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


Color stringToColor(std::string);

namespace SearchClient {
    class Client {
        private:
            std::vector<SearchEngine::Command> jointAction;
            int actionsRecv;  
        public:
            Client();

            SearchEngine::State initState();

            void setAction(int, SearchEngine::Command);

            void send();                 
    };
}

#endif
