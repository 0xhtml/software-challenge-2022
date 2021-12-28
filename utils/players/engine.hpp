#pragma once

#include "../../src/alphabeta.hpp"
#include "base.hpp"

class Engine : public Player {
    using Player::Player;

    Move run() override {
        AlphaBeta alphaBeta{gameState};

        Time start = std::chrono::system_clock::now();
        alphaBeta.start = start - std::chrono::milliseconds(1950 - 24);
        alphaBeta.timeOut = false;

        Move move;

        for (int depth = 1; depth <= 20; ++depth) {
            MoveValuePair moveValuePair = alphaBeta.alphaBetaRoot(depth, -INT_MAX, INT_MAX);

            if (alphaBeta.timeOut && depth > 1) break;

            move = moveValuePair.move;

            if (alphaBeta.timeOut || moveValuePair.value >= INT_MAX) break;
        }

        return move;
    }
};
