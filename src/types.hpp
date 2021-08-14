#pragma once

#include <cstdint>

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
    PieceType stackedPieceType:2;
};

enum Direction {
    UP = 8,
    DOWN = -UP,
    LEFT = 1,
    RIGHT = -LEFT
};

union Position {
    uint8_t square:6;
    struct {
        uint8_t y:3, x:3;
    } coords;

    Position() { square = 0; };
    Position(int s) { square = s; };
    Position operator+(Direction d) const { return {uint8_t(square + d)}; };
};

struct Move {
    Position from, to;
};

struct SaveState {
    Field from, to;
};