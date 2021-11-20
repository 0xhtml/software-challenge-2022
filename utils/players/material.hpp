#pragma once

#include <climits>
#include <cassert>

#include "base.hpp"

class Material : public Player {
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

            if (field.team == ONE) {
                ++value;
            } else {
                --value;
            }
        }

        return value;
    }
};
