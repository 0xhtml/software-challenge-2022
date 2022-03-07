#include "evaluation.hpp"

#include "types.hpp"

const int pieceSquareTable[PIECE_TYPE_COUNT][FIELD_COUNT]{
    // HERZMUSCHEL
    { 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2,
      3, 3, 4, 4, 3, 3, 4, 4,
      4, 4, 5, 4, 4, 4, 5, 4,
      6, 6, 5, 5, 6, 6, 5, 5,
      8, 7, 6, 6, 8, 7, 6, 6,
      0, 0, 0, 0, 0, 0, 0, 0},
    // MOEWE
    { 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2,
      3, 3, 4, 4, 3, 3, 4, 4,
      4, 4, 4, 5, 4, 4, 5, 4,
      6, 6, 5, 5, 6, 6, 5, 5,
      8, 7, 6, 6, 8, 7, 6, 6,
      0, 0, 0, 0, 0, 0, 0, 0},
    // SEESTERN
    { 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2,
      3, 3, 4, 3, 3, 3, 4, 4,
      4, 4, 5, 5, 4, 4, 5, 4,
      6, 6, 5, 5, 6, 6, 5, 5,
      8, 7, 6, 6, 8, 7, 6, 6,
      0, 0, 0, 0, 0, 0, 0, 0},
    // ROBBE
    { 0, 1, 2, 2, 0, 0, 0, 0,
      1, 2, 4, 4, 1, 1, 1, 1,
      2, 4, 6, 6, 2, 2, 2, 2,
      2, 4, 6, 6, 3, 3, 4, 4,
      2, 4, 6, 6, 4, 4, 5, 4,
      2, 4, 6, 6, 6, 6, 5, 5,
      1, 2, 4, 4, 8, 7, 6, 6,
      0, 1, 2, 2, 0, 0, 0, 0}
};

int scoreEvaluation(const GameState &gameState) {
    return (gameState.score[ONE] - gameState.score[TWO]) * 2000;
}

int pieceSquareEvaluation(int square, const Field &field) {
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
