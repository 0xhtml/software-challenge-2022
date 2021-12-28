#pragma once

#include <climits>
#include <cassert>

#include "base.hpp"

class Queren : public Player {
    using Player::Player;

    int initialDepth() const override {
        return 3;
    }

    bool filterMove(const GameState &gameState, const Move &move) const override {
        return !gameState.board[move.to.square].occupied;
    }

    int rate(GameState &gameState) const override {
        int value = 0;

        for (int square = 0; square < FIELD_COUNT; ++square) {
            const Field &field = gameState.board[square];

            if (!field.occupied) continue;
            if (field.pieceType == ROBBE) continue;

            if (field.team == ONE) value += Position{square}.coords.x;
            else value -= 7 - Position{square}.coords.x;
        }

        return value;
    }
};
