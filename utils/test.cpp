#include <algorithm>
#include <future>
#include <random>
#include <vector>
#include <cassert>

#include "../src/alphabeta.hpp"
#include "../src/gamestate.hpp"

#define GAME_COUNT 1600
#define THREAD_COUNT 16

double evalGame(const GameState &gameState) {
    if (gameState.score[RED] > gameState.score[BLUE]) return 1;
    if (gameState.score[RED] < gameState.score[BLUE]) return 0;

    std::vector<int> dist[COLOR_COUNT]{};

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            const Field &field = gameState.board[(y << 3) | x];

            if (!field.occupied) continue;
            if (field.pieceType == ROBBE) continue;

            dist[field.color].push_back(field.color ? 7 - x : x);
        }
    }

    std::sort(dist[RED].begin(), dist[RED].end(), std::greater<>());
    std::sort(dist[BLUE].begin(), dist[BLUE].end(), std::greater<>());

    for (int i = 0; i < std::min(dist[RED].size(), dist[BLUE].size()); ++i) {
        if (dist[RED][i] > dist[BLUE][i]) return 1;
        if (dist[RED][i] < dist[BLUE][i]) return 0;
    }

    return .5;
}

double playGame() {
    std::random_device random_device;
    std::mt19937 mt19937(random_device());
    std::uniform_int_distribution<int> pieceDist(0, 7);
    std::uniform_int_distribution<int> startDist(0, 1);

    std::string fen = "HHMMSSRR/8/8/8/8/8/8/hhmmssrr 0";

    for (int i = 0; i < 8; ++i) {
        int j = pieceDist(mt19937);
        int k = pieceDist(mt19937);
        std::swap(fen[j], fen[k]);
        j = pieceDist(mt19937) + 21;
        k = pieceDist(mt19937) + 21;
        std::swap(fen[j], fen[k]);
    }

    GameState gameState{fen};

    int start = startDist(mt19937);

    AlphaBeta player{gameState};

    while (!gameState.isOver()) {
        if (gameState.turn % 2 == start) {
            gameState.makeMove(player.iterativeDeepening(std::chrono::system_clock::now()));
        } else {
            std::vector<Move> moves = gameState.getPossibleMoves();
            if (moves.size() == 0) return 1;
            std::uniform_int_distribution<int> moveDist(0, moves.size() - 1);
            gameState.makeMove(moves[moveDist(mt19937)]);
        }
    }

    double eval = evalGame(gameState);
    if (start == 1) eval = 1 - eval;

    return eval;
}

typedef std::vector<double> ThreadResults;

ThreadResults worker() {
    ThreadResults results;
    for (int i = 0; i < GAME_COUNT / THREAD_COUNT; ++i) {
        results.push_back(playGame());
    }
    return results;
}

int main() {
    std::future<ThreadResults> threads[THREAD_COUNT];

    for (std::future<ThreadResults> &thread : threads) {
        thread = std::async(&worker);
    }

    std::vector<double> results;

    for (std::future<ThreadResults> &thread : threads) {
        ThreadResults threadResults = thread.get();
        results.insert(results.end(), threadResults.begin(), threadResults.end());
    }

    double avg, lost, draw, won;

    for (double result : results) {
        avg += result;
        if (result == 0) ++lost;
        if (result == .5) ++draw;
        if (result == 1) ++won;
    }

    avg /= results.size();
    lost /= results.size();
    draw /= results.size();
    won /= results.size();

    lost *= 100;
    draw *= 100;
    won *= 100;

    printf("avg %f\nlost %f%%\ndraw %f%%\nwon %f%%\n", avg, lost, draw, won);

    return 0;
}
