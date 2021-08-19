#include "alphabeta.hpp"

#include <climits>

#include "evaluation.hpp"
#include "gamestate.hpp"
#include "types.hpp"

int AlphaBeta::alphaBeta(GameState &gameState, const int depth, int alpha, int beta) {
    if (depth <= 0) return Evaluation::evaluate(gameState);

    for (Move move : gameState.getPossibleMoves()) {
        SaveState saveState = gameState.makeMove(move);
        int score = -alphaBeta(gameState, depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

Move AlphaBeta::alphaBetaRoot(GameState &gameState, const int depth, int alpha, int beta) {
    Move bestMove{};

    for (Move move : gameState.getPossibleMoves()) {
        SaveState saveState = gameState.makeMove(move);
        int score = -alphaBeta(gameState, depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (score >= beta) return move;

        if (score > alpha) {
            alpha = score;
            bestMove = move;
        }
    }

    printf("%i (%i|%i)->(%i|%i)\n", depth, bestMove.from.coords.x, bestMove.from.coords.y, bestMove.to.coords.x, bestMove.to.coords.y);

    return bestMove;
}

Move AlphaBeta::iterativeDeepening(GameState &gameState) {
    Move move{};

    for (int depth = 1; depth < 5; ++depth) {
        move = alphaBetaRoot(gameState, depth, INT_MIN, INT_MAX);
    }

    return move;
}
