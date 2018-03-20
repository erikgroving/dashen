#include "command.h"

using SearchClient::Command;

std::vector<Command::Operation> Command::possibleActions = std::vector<Command::Operation>{
                /* No opeartion */
                Command::NoOperation,

                /* Move operation */
                Command::Move_N,
                Command::Move_S,
                Command::Move_E,
                Command::Move_W,

                /* Push Operation */ 
                Command::Push_NN,
                Command::Push_NW,
                Command::Push_WN,
                Command::Push_WW,
                Command::Push_WS,
                Command::Push_SW,
                Command::Push_EN,
                Command::Push_NE,
                Command::Push_EE,
                Command::Push_ES,
                Command::Push_SE,
                Command::Push_SS,

                
                /* Pull Operation */ 
                Command::Pull_NS,
                Command::Pull_NW,
                Command::Pull_WN,
                Command::Pull_WE,
                Command::Pull_WS,
                Command::Pull_SW,
                Command::Pull_SN,
                Command::Pull_NE,
                Command::Pull_EN,
                Command::Pull_EW,
                Command::Pull_ES,
                Command::Pull_SE
};


Command Command::toCommand(Command::Operation action) {
    switch(action) {
        default:
            return Command(N);
    }
}

Command::Command(Direction d1) : d1(d1), d2((const Direction)NULL), action(MOVE) {} 

Command::Command(Direction d1, Direction d2, Action action) : d1(d1), 
                                                                d2(d2), 
                                                                action(action){}

std::string Command::actionToString(Action a) {
    return  a == MOVE ? "Move" : 
            a == PUSH ? "Push" : "Pull";
}

std::string Command::dirToString(Direction d) {
    return  d == N ? "N" :
            d == E ? "E" :
            d == S ? "S" : "W";
}

std::string Command::toString() {
    if (action == MOVE) {
        return actionToString(action) + "(" + dirToString(d1) + ")";
    }
        return actionToString(action) + "(" + dirToString(d1) + "," + dirToString(d2) + ")";
}

std::string Command::toActionString() {
    return "[" + toString() + "]";
}
