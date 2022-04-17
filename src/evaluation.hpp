#pragma once

#include "gamestate.hpp"

#define WINNING_SCORE 10000

class Evaluation {
public:
    static int evaluate(const GameState &gameState, const bool isOver);
};
