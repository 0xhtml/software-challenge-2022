#pragma once

#include <climits>
#include <random>
#include <stdexcept>

#include "../../src/gamestate.hpp"
#include "../../src/types.hpp"

class Player {
protected:
    GameState &gameState;
    std::mt19937 &mt19937;

    virtual int initialDepth() const {
        return 1;
    }

    virtual bool filterMove(const GameState &gameState, const Move &move) const {
        return false;
    }

    virtual int rate(GameState &gameState) const {
        throw std::runtime_error("rate not implemented");
    }

    int rateWrapper(GameState &gameState) const {
        int value = 0;

        value += gameState.score[ONE] * 10;
        value -= gameState.score[TWO] * 10;

        value += rate(gameState);

        if (gameState.turn % 2 == TWO) value = -value;

        return value;
    }

    int alphaBeta(GameState &gameState, const int depth, int alpha, int beta) const {
	if (depth <= 0 || gameState.isOver()) return rateWrapper(gameState);

        int terminalNode = true;

	for (const Move &move : gameState.getPossibleMoves()) {
            if (filterMove(gameState, move)) continue;

            terminalNode = false;

            SaveState saveState = gameState.makeMove(move);
            int score = -alphaBeta(gameState, depth - 1, -beta, -alpha);
            gameState.unmakeMove(move, saveState);

            if (score >= beta) {
                return beta;
            }

            if (score > alpha) {
                alpha = score;
            }
        }

        if (terminalNode) return rateWrapper(gameState);

	return alpha;
    }

public:
    Player(GameState &gameState, std::mt19937 &mt19937) : gameState(gameState), mt19937(mt19937) {}
    virtual ~Player() = default;

    virtual Move run() {
	int alpha = -INT_MAX;
        Move bestMove;

	for (const Move &move : gameState.getPossibleMoves()) {
            SaveState saveState = gameState.makeMove(move);
            int score = -alphaBeta(gameState, initialDepth(), -INT_MAX, -alpha);
            gameState.unmakeMove(move, saveState);

            if (score > alpha) {
                alpha = score;
                bestMove = move;
            }
        }

        return bestMove;
    }
};
