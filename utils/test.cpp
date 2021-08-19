#include <algorithm>
#include <future>
#include <random>
#include <vector>
#include <cassert>

#include "../src/alphabeta.hpp"
#include "../src/gamestate.hpp"

#define GAME_COUNT 50000
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
    }

    for (int i = 21; i < 29; ++i) {
        int j = pieceDist(mt19937) + 21;
        int k = pieceDist(mt19937) + 21;
        std::swap(fen[i], fen[j]);
    }

    GameState gameState{fen};

    bool start = startDist(mt19937);

    AlphaBeta player{};

    while (!gameState.isOver()) {
        if (start && gameState.turn % 2 == 0) {
            gameState.makeMove(player.iterativeDeepening(gameState));
        } else {
            std::vector<Move> moves = gameState.getPossibleMoves();
            if (moves.size() == 0) return 1;
            std::uniform_int_distribution<int> moveDist(0, moves.size() - 1);
            gameState.makeMove(moves[moveDist(mt19937)]);
        }
    }

    double eval = evalGame(gameState);
    if (!start) eval = 1 - eval;

    return eval;
}

std::vector<double> worker() {
    std::vector<double> results;

    for (int i = 0; i < GAME_COUNT / THREAD_COUNT; ++i) {
        results.push_back(playGame());
    }

    return results;
}

int main() {
    std::future<std::vector<double>> threads[THREAD_COUNT];

    for (std::future<std::vector<double>> &thread : threads) {
        thread = std::async(&worker);
    }

    std::vector<double> results;

    for (std::future<std::vector<double>> &thread : threads) {
        std::vector<double> threadResults = thread.get();
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
