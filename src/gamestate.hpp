#pragma once

#include <string>
#include <vector>

#include "types.hpp"

#define FIELD_COUNT 64

class GameState {
private:
    int turn = 0;
    int score = 0;
    Field board[FIELD_COUNT]{};

public:
    GameState(const std::string fen);

    std::vector<Move> getPossibleMoves() const;

    SaveState makeMove(const Move &move);

    void unmakeMove(const Move &move, const SaveState &saveState);
};
