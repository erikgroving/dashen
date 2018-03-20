#include "command.h"

using SearchClient::Command;

std::vector<Command::Action> Command::possibleActions = std::vector<Command::Action>{/* No opeartion */
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

Command Command::toCommand(Command::Action action) {
    switch(action) {
        default:
            return Command();
    }
}

