#pragma once

#include <cstdint>

#define COLOR_COUNT 2
enum Color : uint8_t {
    RED,
    BLUE
};
inline Color operator!(Color c) {
    if (c == RED) return BLUE;
    return RED;
};

#define PIECE_TYPE_COUNT 4
enum PieceType : uint8_t {
    HERZMUSCHEL,
    MOEWE,
    SEESTERN,
    ROBBE
};

struct Field {
    bool occupied:1;
    Color color:2;
    PieceType pieceType:2;
    bool stacked:1;
};

enum Direction {
    UP = 1,
    DOWN = -UP,
    LEFT = 8,
    RIGHT = -LEFT
};

union Position {
    uint8_t square:6;
    struct {
        uint8_t x:3, y:3;
    } coords;

    Position() { square = 0; };
    Position(int s) { square = s; };
};

struct Move {
    Position from, to;
};

struct SaveState {
    Field from, to;
    int score;
};
