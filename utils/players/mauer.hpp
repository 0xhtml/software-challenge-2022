#pragma once

#include <algorithm>

#include "base.hpp"

class Mauer : public Player {
public:
    Move run(GameState &gameState) const override {
        std::vector<Move> moves = gameState.getPossibleMoves();

        std::sort(moves.begin(), moves.end(), [gameState](const Move& a, const Move& b) {
            // Points
            const Field &afrom = gameState.board[a.from.square];
            const Field &ato = gameState.board[a.to.square];
            const bool ac = ato.occupied && (afrom.stacked || ato.stacked);

            const Field &bfrom = gameState.board[b.from.square];
            const Field &bto = gameState.board[b.to.square];
            const bool bc = bto.occupied && (bfrom.stacked || bto.stacked);

            if (ac != bc) {
                return ac;
            }

            // Only move foreward
            if (gameState.turn % 2 == ONE) {
                if (a.from.coords.x >= a.to.coords.x) return false;
                if (b.from.coords.x >= b.to.coords.x) return true;
            } else {
                if (a.from.coords.x <= a.to.coords.x) return false;
                if (b.from.coords.x <= b.to.coords.x) return true;
            }

            // Smallest distance first
            if (a.to.coords.x - a.from.coords.x != b.to.coords.x - b.from.coords.x) {
                return abs(a.to.coords.x - a.from.coords.x) < abs(b.to.coords.x - b.from.coords.x);
            }

            // Piece closes to start first
            if (a.from.coords.x != b.from.coords.x) {
                if (gameState.turn % 2 == ONE) {
                    return a.from.coords.x < b.from.coords.x;
                } else {
                    return a.from.coords.x > b.from.coords.x;
                }
            }

            // Smallest change in y-direction
            return abs(a.from.coords.y - a.to.coords.y) < abs(b.from.coords.y - b.to.coords.y);
        });

        return moves[0];
    }
};
