#include "command.h"
#include <iostream>

using SearchEngine::Command;

std::vector<Command> Command::possibleActions = std::vector<Command>{
                /* No operation */
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

Command::Command(const Command &src): action_(src.action_), d1_(src.d1_), d2_(src.d2_) {
}

Command::Command(Action type): action_(type), d1_(NONE), d2_(NONE) {
}

Command::Command(Action type, Direction dir1): action_(type), d1_(dir1), d2_(NONE) {}
Command::Command(Action type, Direction dir1, Direction dir2) : 
    action_(type), d1_(dir1), d2_(dir2) {}

std::string Command::actionToString(Action a) const {
    return  a == MOVE ? "Move" : 
            a == PUSH ? "Push" : 
            a == PULL ? "Pull" : "NONE";
}

std::string Command::dirToString(Direction d) const {
    return  d == N ? "N" :
            d == E ? "E" :
            d == S ? "S" : 
            d == W  ? "W" : "-";
}

std::string Command::toString() const {
    if (action_ == MOVE) {
        return actionToString(action_) + "(" + dirToString(d1_) + ")";
    }
    else if (action_ == NOOP) {
        return "NoOp";
    }
    return actionToString(action_) + "(" + dirToString(d1_) + "," + dirToString(d2_) + ")";
}

std::string Command::toActionString() const {
    return "[" + toString() + "]";
}

int Command::colToInt(Direction dir) {
    switch(dir){
        case W:
            return -1;
        case E:
            return 1;
        default:
            return 0;
    }
}
int Command::rowToInt(Direction dir) {
    switch(dir){
        case N:
            return -1;
        case S:
            return 1;
        default:
            return 0;
    }
}
