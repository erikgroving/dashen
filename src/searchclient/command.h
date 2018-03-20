#include <string>
#include "typedefs.h"

namespace SearchClient {
    class Command {
        private:
            const Direction d1;
            const Direction d2;
            const Action action;

        public:
            Command (Direction d1);
            Command (Direction d1, Direction d2, Action action);
            std::string actionToString(Action a);
            std::string dirToString(Direction a);
            std::string toString();
            std::string toActionString();         
    };
}