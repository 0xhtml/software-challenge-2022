#pragma once

#include <string>
#include <vector>

#include "types.hpp"

#define FIELD_COUNT 64

class GameState {
private:
    uint64_t zobristPiece[FIELD_COUNT][COLOR_COUNT][PIECE_TYPE_COUNT];
    uint64_t zobristStacked[FIELD_COUNT];
    uint64_t zobristColor;

public:
    Field board[FIELD_COUNT]{};
    int turn = 0;
    int score[COLOR_COUNT]{};
    uint64_t hash;

    GameState(const std::string fen);

    std::vector<Move> getPossibleMoves() const;

    bool isOver() const;

    SaveState makeMove(const Move &move);

    void unmakeMove(const Move &move, const SaveState &saveState);
};
