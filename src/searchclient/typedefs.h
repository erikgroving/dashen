enum class Color { BLUE, RED, GREEN, CYAN, MAGENTA, ORANGE, PINK, YELLOW};

typedef struct Coord {
    char x;
    char y;
} Coord;

typedef struct Box {
    Color color;
    char letter;
    Coord loc;
} Box;

typedef struct Agent {
    Color color;
    char num;
    Coord loc;
} Agent;

typedef struct Goal {
    char letter;
    Coord loc;
}