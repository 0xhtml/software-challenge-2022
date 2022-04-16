#include "alphabeta.hpp"

#include <algorithm>
#include <assert.h>
#include <chrono>
#include <limits.h>
#include <stdio.h>
#include <vector>

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

bool isTacticalMove(const GameState &gameState, const Move &move) {
    const Field &to = gameState.board[move.to.square];
    if (to.occupied) return true;

    const Field &from = gameState.board[move.from.square];
    const int oppBaseline = (from.team == ONE) ? 7 : 0;
    if (from.pieceType == ROBBE && move.to.coords.x == oppBaseline) return true;

    const Direction forward = (from.team == ONE) ? RIGHT : LEFT;
    const Field &front = gameState.board[move.from.square + forward];
    if (from.pieceType == HERZMUSCHEL && front.pieceType == MOEWE
            && front.team != from.team) return true;

    return false;
}

int AlphaBeta::quiesce(int alpha, int beta) {
    assert(alpha < beta);
    assert(!gameState.isOver());

    if (checkTimeOut()) return 0;

    int static_evaluation = Evaluation::evaluate(gameState, false);

    if (static_evaluation >= beta) return beta;
    if (alpha < static_evaluation) alpha = static_evaluation;

    std::vector<Move> moves = gameState.getPossibleMoves();
    if (moves.size() == 0) return -WINNING_SCORE;

    for (const Move &move : moves) {
        if (!isTacticalMove(gameState, move)) continue;

        SaveState saveState = gameState.makeMove(move);

        int score;
        if (gameState.isOver()) score = -Evaluation::evaluate(gameState, true);
        else score = -quiesce(-beta, -alpha);

        gameState.unmakeMove(move, saveState);

        if (timeOut) return 0;

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

int AlphaBeta::alphaBeta(const int depth, int alpha, int beta) {
    assert(depth >= 0);
    assert(alpha < beta);

    if (checkTimeOut()) return 0;

    Transposition transposition = transpositionTable.get(gameState.hash);

    if (transposition.type != EMPTY && transposition.depth >= depth) {
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

    if (gameState.isOver()) return Evaluation::evaluate(gameState, true);
    if (depth <= 0) return quiesce(alpha, beta);

    TranspositionType type = ALPHA;
    Move bestMove;

    if (transposition.type == EXACT || transposition.type == BETA) {
        SaveState saveState = gameState.makeMove(transposition.move);
        int score = -alphaBeta(depth - 1, -beta, -alpha);
        gameState.unmakeMove(transposition.move, saveState);

        if (timeOut) return 0;

        if (score >= beta) {
            transpositionTable.put({BETA, gameState.hash, depth, beta, transposition.move, gameState.turn});
            return beta;
        }

        if (score > alpha) {
            type = EXACT;
            bestMove = transposition.move;
            alpha = score;
        }
    }

    std::vector<Move> moves = gameState.getPossibleMoves();

    if (moves.size() == 0) return -WINNING_SCORE;

    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b){
        return history[gameState.board[a.from.square].pieceType][a.to.square]
             > history[gameState.board[b.from.square].pieceType][b.to.square];
    });

    for (Move move : moves) {
        if (transposition.move == move) continue;

        SaveState saveState = gameState.makeMove(move);
        int score = -alphaBeta(depth - 1, -beta, -alpha);
        gameState.unmakeMove(move, saveState);

        if (timeOut) return 0;

        if (score >= beta) {
            transpositionTable.put({BETA, gameState.hash, depth, beta, move, gameState.turn});
            history[gameState.board[move.from.square].pieceType][move.to.square] += 1 << depth;
            return beta;
        }

        if (score > alpha) {
            type = EXACT;
            bestMove = move;
            alpha = score;
        }
    }

    transpositionTable.put({type, gameState.hash, depth, alpha, bestMove, gameState.turn});

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

        if (timeOut) break;
    }

    assert(bestMove != Move{});

    return bestMove;
}
