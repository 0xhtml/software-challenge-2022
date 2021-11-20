#pragma once

#include "base.hpp"

class Random : public Player {
public:
    Move run(GameState &gameState, std::mt19937 &mt19937) const override {
        std::vector<Move> moves = gameState.getPossibleMoves();

        std::uniform_int_distribution<> dist(0, moves.size() - 1);

        Move move = moves[dist(mt19937)];

        return move;
    }
};
