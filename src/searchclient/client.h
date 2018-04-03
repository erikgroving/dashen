#include <string>
#include <vector>
#include <algorithm>
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
