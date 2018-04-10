#ifndef TYPEDEFS_H
#define TYPEDEFS_H

enum Color { NOCOLOR, BLUE, RED, GREEN, CYAN, MAGENTA, ORANGE, PINK, YELLOW};
enum Direction { N, E, S, W, NONE};
enum Action { MOVE, PUSH, PULL, NOOP };

typedef struct Coord {
    int x;
    int y;

    Coord(int xCoord = -1, int yCoord = -1) : x(xCoord), y(yCoord){}
    Coord(const Coord &c): x(c.x), y(c.y) {}
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
} Coord;

typedef struct Box {
    Color color;
    char letter;
    Coord loc;

    Box(): color(NOCOLOR), letter(0), loc({-1,-1}) {
        
    }
    Box(Color color, char letter, Coord loc) :  color(color),
                                                letter(letter),
                                                loc(loc) {}

    Box(const Box &src): color(src.color), letter(src.letter), loc(src.loc) {}
    bool operator==(const Box &compared) const {
        if(compared.letter != letter)
            return false;

        if(compared.color != color)
            return false;

        return compared.loc != loc;
    }
} Box;


typedef struct Goal {
    char letter;
    Coord loc;

    Goal (char letter, Coord loc) : letter(letter), loc(loc) {}

} Goal;

typedef struct AgentDescription
{
    Color color;
    char num;
    Coord loc;
} AgentDescription;

#endif 
