#pragma once

#include "gamestate.hpp"
#include "transpositiontable.hpp"
#include "types.hpp"

class AlphaBeta {
private:
    GameState &gameState;
    TranspositionTable transpositionTable{};
    Time start;
    bool timeOut;

    bool checkTimeOut();

    int alphaBeta(const int depth, int alpha, const int beta);

    Move alphaBetaRoot(const int depth, int alpha, const int beta);

public:
    AlphaBeta(GameState &gameState);

    Move iterativeDeepening(const Time start);
};
