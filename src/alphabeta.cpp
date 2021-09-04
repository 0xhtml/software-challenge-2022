#include "alphabeta.hpp"

#include <cassert>
#include <climits>

#include "evaluation.hpp"
#include "gamestate.hpp"
#include "transpositiontable.hpp"
#include "types.hpp"

AlphaBeta::AlphaBeta(GameState &gameState) : gameState(gameState) {}

bool AlphaBeta::checkTimeOut() {
    assert(!timeOut);

    Time now = std::chrono::system_clock::now();

    if (std::chrono::duration_cast<MS>(now - start).count() >= 1950) {
        timeOut = true;
        return true;
    }

    return false;
}

int AlphaBeta::alphaBeta(const int depth, int alpha, const int beta) {
    if (checkTimeOut()) return 0;

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
        int score = -alphaBeta(depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (timeOut) return alpha;

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

Move AlphaBeta::alphaBetaRoot(const int depth, int alpha, const int beta) {
    if (checkTimeOut()) return {};

    Move bestMove;

    std::vector<Move> moves = gameState.getPossibleMoves();

    assert(moves.size() != 0);

    for (Move move : moves) {
        SaveState saveState = gameState.makeMove(move);
        int score = -alphaBeta(depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (timeOut) return bestMove;

        if (score >= beta) return move;

        if (score > alpha) {
            alpha = score;
            bestMove = move;
        }
    }

    return bestMove;
}

Move AlphaBeta::iterativeDeepening(const Time start) {
    this->start = start;
    timeOut = false;

    Move bestMove;

    for (int depth = 1; depth <= 12; ++depth) {
        Move move = alphaBetaRoot(depth, -INT_MAX, INT_MAX);

        if (timeOut) break;

        bestMove = move;
    }

    assert(bestMove != Move{});

    return bestMove;
}
