#pragma once

#include <cstdint>
#include <chrono>

typedef std::chrono::time_point<std::chrono::system_clock> Time;
typedef std::chrono::milliseconds MS;

#define COLOR_COUNT 2
enum Color : uint8_t {
    RED,
    BLUE
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
    bool operator==(const Position &p) const { return p.square == square; }
};

struct Move {
    Position from, to;

    bool operator==(const Move &m) const { return m.from == from && m.to == to; }
    bool operator!=(const Move &m) const { return !operator==(m); }
};

struct SaveState {
    Field from, to;
    int score;
    uint64_t hash;
};
