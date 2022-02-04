#include "evaluation.hpp"

#include "types.hpp"

const int pieceSquareTable[PIECE_TYPE_COUNT][FIELD_COUNT / 2]{
    // HERZMUSCHEL
    {0,-1,-1, 0, 0, 0, 0, 0,
     0, 0,-1, 1, 0, 0, 0, 0,
     0, 3, 3, 2, 3, 0, 0, 0,
     0, 4, 4, 4, 2, 0, 0, 0},
    // MOEWE
    {0,-1, 0, 4, 4, 1, 0, 0,
     0,-1, 0, 3, 3, 0, 0, 0,
     0, 0,-1, 1, 1, 0, 0, 0,
     0, 0,-1, 0, 0, 0, 0, 0},
    // SEESTERN
    {0,-1,-1, 0, 0, 1, 0, 0,
     0,-1,-1, 2, 3, 0, 0, 0,
     0, 2, 2, 4, 3, 0, 0, 0,
     0, 4, 4, 3, 2, 0, 0, 0},
    // ROBBE
    {0,-1, 0, 1, 0, 0, 0, 0,
     0, 0, 2, 0, 0, 0, 0, 0,
     0,-1, 4, 1, 3, 0, 0, 0,
     0,-1, 4, 2, 1, 0, 0, 0}
};

int scoreEvaluation(const GameState &gameState) {
    return (gameState.score[ONE] - gameState.score[TWO]) * 100;
}

int pieceSquareEvaluation(int square, const Field &field) {
    Position pos{square};
    if (pos.coords.y >= 4) pos.coords.y = 7 - pos.coords.y;

    if (field.team == ONE) {
        return pieceSquareTable[field.pieceType][pos.square];
    } else {
        pos.coords.x = 7 - pos.coords.x;
        return -pieceSquareTable[field.pieceType][pos.square];
    }
}

int Evaluation::evaluate(const GameState &gameState) {
    int value = 0;

    value += (gameState.score[ONE] - gameState.score[TWO]) * 100;

    for (int square = 0; square < FIELD_COUNT; ++square) {
         const Field &field = gameState.board[square];

         if (!field.occupied) continue;

        value += pieceSquareEvaluation(square, field);
    }

    if (gameState.turn % 2 == TWO) value = -value;
    return value;
}
