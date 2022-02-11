#include "evaluation.hpp"

#include "types.hpp"

const int pieceSquareTable[PIECE_TYPE_COUNT][FIELD_COUNT / 2]{
    // HERZMUSCHEL
    { 0, 1, 2, 3, 4, 6, 8, 0,
      0, 1, 2, 3, 4, 6, 7, 0,
      0, 1, 2, 4, 5, 5, 6, 0,
      0, 1, 2, 4, 4, 5, 6, 0},
    // MOEWE
    { 0, 1, 2, 3, 4, 6, 8, 0,
      0, 1, 2, 3, 4, 6, 7, 0,
      0, 1, 2, 4, 4, 5, 6, 0,
      0, 1, 2, 4, 5, 5, 6, 0},
    // SEESTERN
    { 0, 1, 2, 3, 4, 6, 8, 0,
      0, 1, 2, 3, 4, 6, 7, 0,
      0, 1, 2, 4, 5, 5, 6, 0,
      0, 1, 2, 3, 5, 5, 6, 0},
    // ROBBE
    { 0, 1, 2, 2, 2, 2, 1, 0,
      1, 2, 4, 4, 4, 4, 2, 1,
      2, 4, 6, 6, 6, 6, 4, 2,
      2, 4, 6, 6, 6, 6, 4, 2}
};

int scoreEvaluation(const GameState &gameState) {
    return (gameState.score[ONE] - gameState.score[TWO]) * 2000;
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

int stackEvaluation(const Field &field) {
    if (field.stacked) {
        return (field.team == ONE) ? 10 : -10;
    }
    return 0;
}

int Evaluation::evaluate(const GameState &gameState) {
    int value = 0;

    value += scoreEvaluation(gameState);

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field &field = gameState.board[square];

        if (!field.occupied) continue;

        value += pieceSquareEvaluation(square, field);
        value += stackEvaluation(field);
    }

    if (gameState.turn % 2 == TWO) value = -value;
    return value;
}
