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

int evaluateWinner(const GameState &gameState) {
    Team winner = gameState.calcWinner();
    if (winner == NO_TEAM) return 0;
    if (winner == gameState.turn % TEAM_COUNT) return INT_MAX;
    return -INT_MAX;
}

int AlphaBeta::alphaBeta(const int depth, int alpha, int beta) {
    assert(depth >= 0);
    assert(alpha < beta);

    if (checkTimeOut()) return 0;

    Transposition transposition = transpositionTable.get(gameState.hash);

    if (transposition.type != EMPTY && transposition.depth >= depth && gameState.turn + transposition.depth < TURN_LIMIT) {
        if (transposition.type == EXACT) return transposition.score;

        if (transposition.type == ALPHA) {
            if (transposition.score <= alpha) return alpha;

            if (transposition.depth == depth && transposition.score < beta)
                beta = transposition.score;
        } else if (transposition.type == BETA) {
            if (transposition.score >= beta) return beta;

            if (transposition.depth == depth && transposition.score > alpha)
                alpha = transposition.score;
        }
    }

    assert(alpha < beta);

    if (gameState.isOver()) return evaluateWinner(gameState);
    if (depth <= 0) return Evaluation::evaluate(gameState);

    TranspositionType type = ALPHA;

    std::vector<Move> moves = gameState.getPossibleMoves();

    if (moves.size() == 0) return -INT_MAX;

    for (Move move : moves) {
        SaveState saveState = gameState.makeMove(move);
        int score = -alphaBeta(depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (timeOut) return 0;

        if (score >= beta) {
            transpositionTable.put({BETA, gameState.hash, depth, beta});
            return beta;
        }
        if (score > alpha) {
            type = EXACT;
            alpha = score;
        }
    }

    transpositionTable.put({type, gameState.hash, depth, alpha});

    return alpha;
}

MoveValuePair AlphaBeta::alphaBetaRoot(const int depth, int alpha, int beta) {
    assert(depth > 0);
    assert(alpha < beta);

    if (checkTimeOut()) return {};

    std::vector<Move> moves = gameState.getPossibleMoves();

    assert(moves.size() != 0);

    Move bestMove = moves[0];

    for (Move move : moves) {
        SaveState saveState = gameState.makeMove(move);
        int score = -alphaBeta(depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (timeOut) return {bestMove, alpha};

        if (score >= beta) return {move, beta};

        if (score > alpha) {
            alpha = score;
            bestMove = move;
        }
    }

    return {bestMove, alpha};
}

Move AlphaBeta::iterativeDeepening(const Time start) {
    this->start = start;
    timeOut = false;

    Move bestMove;

    for (int depth = 1; depth <= 20; ++depth) {
        MoveValuePair moveValuePair = alphaBetaRoot(depth, -INT_MAX, INT_MAX);

        if (timeOut && depth > 1) break;

        bestMove = moveValuePair.move;

        printf("DEBUG: d=%i s=%i\n", depth, moveValuePair.value);

        if (timeOut || moveValuePair.value >= INT_MAX) {
            break;
        }
    }

    assert(bestMove != Move{});

    return bestMove;
}
