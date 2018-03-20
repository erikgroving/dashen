#include <string>
#include <vector>
#include "command.h"
#include "typedefs.h"

namespace SearchClient {
    class Client {
        private:
            std::vector<Command> jointAction;
            
        public:
            Client();


            void send();                 
    };
}