#pragma once

#include <climits>
#include <cassert>

#include "base.hpp"

class Minimus : public Player {
    int rate(GameState &gameState) const override {
        int value = 0;

        int turn = gameState.turn;

        gameState.turn = ONE;
        value += gameState.getPossibleMoves().size();

        gameState.turn = TWO;
        value -= gameState.getPossibleMoves().size();

        gameState.turn = turn;

        return value;
    }
};
