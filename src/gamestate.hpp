#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "types.hpp"

#define FIELD_COUNT 64
#define MAX_SCORE 2
#define TURN_LIMIT 60

class GameState {
private:
    struct {
        uint64_t piece[FIELD_COUNT][TEAM_COUNT][PIECE_TYPE_COUNT];
        uint64_t stacked[FIELD_COUNT];
        uint64_t score[TEAM_COUNT][MAX_SCORE];
        uint64_t team;
    } zobrist;

public:
    Field board[FIELD_COUNT]{};
    int turn = 0;
    int score[TEAM_COUNT]{};
    uint64_t hash;

    GameState();

    GameState(const std::string &fen);

    std::vector<Move> getPossibleMoves() const;

    bool isOver() const;

    Team calcWinner() const;

    SaveState makeMove(const Move &move);

    void unmakeMove(const Move &move, const SaveState &saveState);
};
