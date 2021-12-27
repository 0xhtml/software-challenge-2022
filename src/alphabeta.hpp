#pragma once

#include "gamestate.hpp"
#include "transpositiontable.hpp"
#include "types.hpp"

struct MoveValuePair {
    Move move;
    int value;
};

class AlphaBeta {
private:
    GameState &gameState;
    TranspositionTable transpositionTable{};
    uint64_t history[PIECE_TYPE_COUNT][FIELD_COUNT]{};
    Time start;
    bool timeOut;

    bool checkTimeOut();

    int alphaBeta(const int depth, int alpha, int beta);

    MoveValuePair alphaBetaRoot(const int depth, int alpha, int beta);

public:
    AlphaBeta(GameState &gameState);

    Move iterativeDeepening(const Time start);
};
