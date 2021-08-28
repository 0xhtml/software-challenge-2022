#include "alphabeta.hpp"

#include <cassert>
#include <climits>

#include "evaluation.hpp"
#include "gamestate.hpp"
#include "transpositiontable.hpp"
#include "types.hpp"

int AlphaBeta::alphaBeta(GameState &gameState, const int depth, int alpha, int beta) {
    Transposition transposition = transpositionTable.get(gameState.hash);

    if (transposition.type != EMPTY) {
        if (transposition.depth >= depth) {
            if (transposition.type == EXACT) return transposition.score;
            if (transposition.type == ALPHA && transposition.score <= alpha) return alpha;
            if (transposition.type == BETA && transposition.score >= beta) return beta;
        }
    }

    if (depth <= 0 || gameState.isOver()) return Evaluation::evaluate(gameState);

    TranspositionType type = ALPHA;

    std::vector<Move> moves = gameState.getPossibleMoves();

    if (moves.size() == 0) return -INT_MAX;

    for (Move move : moves) {
        SaveState saveState = gameState.makeMove(move);
        int score = -alphaBeta(gameState, depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (score >= beta) {
            transpositionTable.put({gameState.hash, BETA, depth, beta});
            return beta;
        }
        if (score > alpha) {
            type = EXACT;
            alpha = score;
        }
    }

    transpositionTable.put({gameState.hash, type, depth, alpha});

    return alpha;
}

Move AlphaBeta::alphaBetaRoot(GameState &gameState, const int depth, int alpha, int beta) {
    Move bestMove{};

    std::vector<Move> moves = gameState.getPossibleMoves();

    assert(moves.size() != 0);

    for (Move move : moves) {
        SaveState saveState = gameState.makeMove(move);
        int score = -alphaBeta(gameState, depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (score >= beta) return move;

        if (score > alpha) {
            alpha = score;
            bestMove = move;
        }
    }


    return bestMove;
}

Move AlphaBeta::iterativeDeepening(GameState &gameState) {
    Move move{};

    for (int depth = 1; depth < 5; ++depth) {
        move = alphaBetaRoot(gameState, depth, -INT_MAX, INT_MAX);
    }

    return move;
}
