#ifndef TYPEDEFS_H
#define TYPEDEFS_H

enum Color { BLUE, RED, GREEN, CYAN, MAGENTA, ORANGE, PINK, YELLOW};
enum Direction { N, E, S, W};
enum Action { MOVE, PUSH, PULL };

typedef struct Coord {
    char x;
    char y;

    Coord(char x, char y) : x(x), y(y){}
} Coord;

typedef struct Box {
    Color color;
    char letter;
    Coord loc;

    Box(Color color, char letter, Coord loc) :  color(color),
                                                letter(letter),
                                                loc(loc) {}
    
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