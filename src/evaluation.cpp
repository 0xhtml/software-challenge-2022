#include "evaluation.hpp"

#include "types.hpp"

const int minorPiecePostionEvaluation[COLOR_COUNT][FIELD_COUNT]{
    {0, 1, 2, 3, 4, 5, 6, 0,
     0, 2, 3, 4, 5, 6, 7, 0,
     0, 2, 3, 4, 5, 6, 7, 0,
     0, 2, 3, 4, 5, 6, 7, 0,
     0, 2, 3, 4, 5, 6, 7, 0,
     0, 2, 3, 4, 5, 6, 7, 0,
     0, 2, 3, 4, 5, 6, 7, 0,
     0, 1, 2, 3, 4, 5, 6, 0},
    {0, 6, 5, 4, 3, 2, 1, 0,
     0, 7, 6, 5, 4, 3, 2, 0,
     0, 7, 6, 5, 4, 3, 2, 0,
     0, 7, 6, 5, 4, 3, 2, 0,
     0, 7, 6, 5, 4, 3, 2, 0,
     0, 7, 6, 5, 4, 3, 2, 0,
     0, 7, 6, 5, 4, 3, 2, 0,
     0, 6, 5, 4, 3, 2, 1, 0}
};

const int majorPiecePostionEvaluation[FIELD_COUNT]{
    -5, -4, -3, -3, -3, -3, -4, -5,
    -4, -2,  0,  0,  0,  0, -2, -4,
    -3,  0,  2,  4,  4,  2,  0, -3,
    -3,  0,  4,  5,  5,  4,  0, -3,
    -3,  0,  4,  5,  5,  4,  0, -3,
    -3,  0,  2,  4,  4,  2,  0, -3,
    -4, -2,  0,  0,  0,  0, -2, -4,
    -5, -4, -3, -3, -3, -3, -4, -5
};

int evaluatePiecePosition(const GameState &gameState) {
    int value = 0;

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field &field = gameState.board[square];

        if (!field.occupied) continue;

        int fieldValue;

        if (field.pieceType == ROBBE) {
            fieldValue = majorPiecePostionEvaluation[square];
        } else {
            fieldValue = minorPiecePostionEvaluation[field.color][square];
        }

        if (field.color == RED) {
            value += fieldValue;
        } else {
            value -= fieldValue;
        }
    }

    return value;
}

int Evaluation::evaluate(const GameState &gameState) {
    int value = 0;

    value += (gameState.score[RED] - gameState.score[BLUE]) * 55;
    value += evaluatePiecePosition(gameState);

    if (gameState.turn % 2) value = -value;
    return value;
}
