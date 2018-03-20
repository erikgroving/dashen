#include "command.h"
#include "typedefs.h"

using SearchClient::Command;

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
