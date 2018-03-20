#include "command.h"

using SearchEngine::Command;

std::vector<Command> Command::possibleActions = std::vector<Command>{
                /* No opeartion */
                Command(), // Command::NoOperation,

                /* Move operation */
                Command(MOVE, N), // Command::Move_N,
                Command(MOVE, S), // Command::Move_S,
                Command(MOVE, E), // Command::Move_E,
                Command(MOVE, W), // Command::Move_W,

                /* Push Operation */ 
                Command(PUSH,N,N), // Command::Push_NN,
                Command(PUSH,N,W), // Command::Push_NW,
                Command(PUSH,W,N), // Command::Push_WN,
                Command(PUSH,W,W), // Command::Push_WW,
                Command(PUSH,W,S), // Command::Push_WS,
                Command(PUSH,S,W), // Command::Push_SW,
                Command(PUSH,E,N), // Command::Push_EN,
                Command(PUSH,N,E), // Command::Push_NE,
                Command(PUSH,E,E), // Command::Push_EE,
                Command(PUSH,E,S), // Command::Push_ES,
                Command(PUSH,S,E), // Command::Push_SE,
                Command(PUSH,S,S), // Command::Push_SS,

                
                /* Pull Operation */ 
                Command(PULL,N,S), // Command::Pull_NS,
                Command(PULL,N,W), // Command::Pull_NW,
                Command(PULL,W,N), // Command::Pull_WN,
                Command(PULL,W,E),  // Command::Pull_WE,
                Command(PULL,W,S),  // Command::Pull_WS,
                Command(PULL,S,W),  // Command::Pull_SW,
                Command(PULL,S,N),  // Command::Pull_SN,
                Command(PULL,N,E),  // Command::Pull_NE,
                Command(PULL,E,N),  // Command::Pull_EN,
                Command(PULL,E,W),  // Command::Pull_EW,
                Command(PULL,E,S),  // Command::Pull_ES,
                Command(PULL,S,E)  // Command::Pull_SE
};

Command::Command(const Command &src): d1(src.d1), d2(src.d2), action(src.action) {}

Command Command::toCommand(Command::Operation action) {
    switch(action) {
        default:
            return Command();
    }
}

Command::Command(Action type): action(type), d1(NONE), d2(NONE) {}
Command::Command(Action type, Direction dir1): action(type), d1(dir1), d2(NONE) {}
Command::Command(Action type, Direction dir1, Direction dir2) : 
    action(type), d1(dir1), d2(dir2) {}

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

int Command::directionToInt(Direction dir){
    switch(dir){
        case N:
        case W:
            return -1;
        case S:
        case E:
            return 1;
        case NONE:
            return 0;
    }
}
