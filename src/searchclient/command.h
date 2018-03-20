#include <string>
#include "typedefs.h"

#include <vector>

namespace SearchClient {
    class Command {
        private:
            const Direction d1;
            const Direction d2;
            const Action action;

        public:
            Command (Direction, Direction, Action);

            enum Operation {
                /* No opeartion */
                NoOperation,

                /* Move operation */
                Move_N,
                Move_S,
                Move_E,
                Move_W,

                /* Push Operation */ 
                Push_NN,
                Push_NW,
                Push_WN,
                Push_WW,
                Push_WS,
                Push_SW,
                Push_EN,
                Push_NE,
                Push_EE,
                Push_ES,
                Push_SE,
                Push_SS,

                
                /* Pull Operation */ 
                Pull_NS,
                Pull_NW,
                Pull_WN,
                Pull_WE,
                Pull_WS,
                Pull_SW,
                Pull_SN,
                Pull_NE,
                Pull_EN,
                Pull_EW,
                Pull_ES,
                Pull_SE
            };

            static Command toCommand(Operation action);

            static std::vector<Operation> possibleActions;
            Command (Direction d1);
            Command (Direction d1, Direction d2, Action action);
            std::string actionToString(Action a);
            std::string dirToString(Direction a);
            std::string toString();
            std::string toActionString();         
    };
}

