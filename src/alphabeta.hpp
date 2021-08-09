#pragma once

#include "gamestate.hpp"
#include "types.hpp"

class AlphaBeta {
private:
    int alphaBeta(GameState &gameState, const int depth, int alpha, int beta);

    Move alphaBetaRoot(GameState &gameState, const int depth, int alpha, int beta);

public:
    Move iterativeDeepening(GameState &gameState);
};
