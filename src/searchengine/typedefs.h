#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <cmath>

enum Color { NOCOLOR, BLUE, RED, GREEN, CYAN, MAGENTA, ORANGE, PINK, YELLOW};
enum Direction { N, E, S, W, NONE};
enum Action { MOVE, PUSH, PULL, NOOP };

typedef struct Coord {
    int x;
    int y;

    Coord(int xCoord = -1, int yCoord = -1) : x(xCoord), y(yCoord){}
    Coord(const Coord &c) {
        x = c.x;
        y = c.y;
    }
    Coord& operator=(const Coord &c) {
        x = c.x;
        y = c.y;
        return *this;
    }

    bool operator==(const Coord &c) const {
        return !operator!=(c);
    }

    bool operator!=(const Coord &c) const {
        return c.x != x || c.y != y;
    }

    static unsigned long distance(const Coord &c1, const Coord &c2) {
        return std::abs(c2.x - c1.x) + std::abs(c2.y - c1.y);
    }

} Coord;

typedef struct Box {
    Color color;
    Coord loc;
    short id;
    char letter;
    Box(): color(NOCOLOR), loc({-1,-1}), id(-1), letter(0) {
        
    }
    Box(Color color, char letter, Coord loc, short id) :  color(color),
                                                loc(loc),
                                                id(id),
                                                letter(letter)
                                                 {}

    Box(const Box &src): color(src.color), loc(src.loc), id(src.id), letter(src.letter) {}
    bool operator==(const Box &compared) const {
        if (compared.letter != letter || compared.color != color || 
            compared.id != id || compared.loc != loc) {
            return false;
        }
        return true;
    }
} Box;


typedef struct Goal {
    Coord loc;
    short assignedBoxID;
    char letter;
    unsigned int priority;

    Goal () :  loc(-1, -1), assignedBoxID(-1), letter('-') {}

    Goal (char letter, Coord loc) : loc(loc), assignedBoxID(-1), letter(letter) {}

    Goal(const Goal &src): loc(src.loc), assignedBoxID(src.assignedBoxID), letter(src.letter), priority(src.priority) {

    }

    Goal& operator=(const Goal &src) {
        loc = src.loc;
        assignedBoxID = src.assignedBoxID;
        letter = src.letter;
        priority = src.priority;
        return *this;
    }

} Goal;

typedef struct AgentDescription
{    
    bool operator<(const AgentDescription& a) const { return num < a.num;  }
    Color color;
    char num;
    Coord loc;
} AgentDescription;

#endif 
