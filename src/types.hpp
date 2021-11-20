#pragma once

#include <chrono>
#include <cstdint>

typedef std::chrono::time_point<std::chrono::system_clock> Time;
typedef std::chrono::milliseconds MS;

#define TEAM_COUNT 2
enum Team : uint8_t {
    ONE,
    TWO,
    NO_TEAM
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
    Team team:2;
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

    Position() { square = 0; }
    Position(int s) { square = s; }

    bool operator==(const Position &p) const { return p.square == square; }
    bool operator!=(const Position &p) const { return !operator==(p); }
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
