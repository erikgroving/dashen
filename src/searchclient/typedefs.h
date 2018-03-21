#ifndef TYPEDEFS_H
#define TYPEDEFS_H

enum Color { BLUE, RED, GREEN, CYAN, MAGENTA, ORANGE, PINK, YELLOW};
enum Direction { N, E, S, W, NONE};
enum Action { MOVE, PUSH, PULL, NOOP };

typedef struct Coord {
    int x;
    int y;

    Coord(int xCoord, int yCoord) : x(xCoord), y(yCoord){}
    bool operator!=(const Coord &c) const {
        return c.x != x || c.y != y;
    }
} Coord;

typedef struct Box {
    Color color;
    char letter;
    Coord loc;

    Box(Color color, char letter, Coord loc) :  color(color),
                                                letter(letter),
                                                loc(loc) {}
    bool operator==(const Box &compared) const {
        if(compared.letter != letter)
            return false;

        if(compared.color != color)
            return false;

        return compared.loc != loc;
    }
} Box;

typedef struct Agent {
    Color color;
    char num;
    Coord loc;

    Agent (Color color, char num, Coord loc) :  color(color),
                                                num(num),
                                                loc(loc) {}
} Agent;

typedef struct Goal {
    char letter;
    Coord loc;

    Goal (char letter, Coord loc) : letter(letter), loc(loc) {}

} Goal;

#endif 
