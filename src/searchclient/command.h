#include "typedefs.h"

namespace SearchClient {
    class Command {
        private:
            const Direction d1;
            const Direction d2;
            const Action action;

        public:
            Command (Direction, Direction, Action);

    };
}