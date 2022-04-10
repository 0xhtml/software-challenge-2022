#include "evaluation.hpp"

#include "types.hpp"

const int pieceSquareTable[PIECE_TYPE_COUNT][FIELD_COUNT]{
    // HERZMUSCHEL
    {  0,  1,  3,  1,  1,  3,  1,  0,
      -2,  1,  1,  1,  1,  1,  1, -2,
       1,  2,  2,  2,  2,  2,  2,  1,
       4,  5,  7,  5,  5,  7,  5,  4,
       9,  5,  9,  6,  6,  9,  5,  9,
      16, 12,  8, 10, 10,  8, 12, 16,
      14, 21, 27, 33, 33, 27, 21, 14,
       0,  0,  0,  0,  0,  0,  0,  0},
    // MOEWE
    { -1,  1,  0,  0,  0,  0,  1, -1,
       0,  3,  3,  2,  2,  3,  3,  0,
       2,  3,  4,  3,  3,  4,  3,  2,
       4,  5,  8,  5,  5,  8,  5,  4,
       8,  8,  5,  9,  9,  5,  8,  8,
      11,  9, 12,  4,  4, 12,  9, 11,
      12, 27, 26, 13, 13, 26, 27, 12,
       0,  0,  0,  0,  0,  0,  0,  0},
    // SEESTERN
    {  0,  0,  1,  0,  0,  1,  0,  0,
       0,  3,  1,  2,  2,  1,  3,  0,
       4,  5,  5,  5,  5,  5,  5,  4,
       5,  8,  8,  8,  8,  8,  8,  5,
       9, 11, 11, 11, 11, 11, 11,  9,
      14, 14, 13, 13, 13, 13, 14, 14,
      24, 23, 23, 21, 21, 23, 23, 24,
       0,  0,  0,  0,  0,  0,  0,  0},
    // ROBBE
    {  2,  4,  4,  4,  4,  4,  4,  2,
       2,  3,  3,  3,  3,  3,  3,  2,
       3,  4,  8,  6,  6,  8,  4,  3,
       5,  5,  6,  7,  7,  6,  5,  5,
       5,  7,  7,  7,  7,  7,  7,  5,
       5, 10,  8, 10, 10,  8, 10,  5,
       7,  6,  9,  8,  8,  9,  6,  7,
       4,  5,  5,  7,  7,  5,  5,  4}
};

int scoreEvaluation(const GameState &gameState) {
    return (gameState.score[ONE] - gameState.score[TWO]) * 2000;
}

int pieceSquareEvaluation(const int square, const Field &field) {
    Position pos{square};

    if (field.team == ONE) {
        return pieceSquareTable[field.pieceType][pos.square];
    } else {
        pos.coords.x = 7 - pos.coords.x;
        return -pieceSquareTable[field.pieceType][pos.square];
    }
}

int Evaluation::evaluate(const GameState &gameState) {
    int value = 0;

    value += scoreEvaluation(gameState);

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field &field = gameState.board[square];

        if (!field.occupied) continue;

        value += pieceSquareEvaluation(square, field);
    }

    if (gameState.turn % 2 == TWO) value = -value;
    return value;
}
