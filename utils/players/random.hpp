#pragma once

#include "base.hpp"

class Random : public Player {
    using Player::Player;

public:
    Move run() override {
        std::vector<Move> moves = gameState.getPossibleMoves();

        std::uniform_int_distribution<> dist(0, moves.size() - 1);

        Move move = moves[dist(mt19937)];

        return move;
    }
};
