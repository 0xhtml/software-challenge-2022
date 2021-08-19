#pragma once

#include "gamestate.hpp"
#include "types.hpp"

class Evaluation {
public:
    static int evaluate(const GameState &gameState);
};
