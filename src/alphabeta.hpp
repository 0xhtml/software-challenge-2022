#pragma once

#include "gamestate.hpp"
#include "transpositiontable.hpp"
#include "types.hpp"

class AlphaBeta {
private:
    TranspositionTable transpositionTable{};

    int alphaBeta(GameState &gameState, const int depth, int alpha, int beta);

    Move alphaBetaRoot(GameState &gameState, const int depth, int alpha, int beta);

public:
    Move iterativeDeepening(GameState &gameState);
};
