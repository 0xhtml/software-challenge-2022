#include <fstream>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "../src/gamestate.hpp"
#include "../src/alphabeta.hpp"
#include "players/material.hpp"
#include "players/mauer.hpp"
#include "players/minimus.hpp"
#include "players/random.hpp"
#include "threadpool.hpp"
#include "tostring.hpp"

#define addPlayer(t) t t##_; players.push_back(&t##_)

#define GAME_COUNT 160
#define PLAYERS() \
    addPlayer(Material); \
    addPlayer(Mauer); \
    addPlayer(Minimus); \
    addPlayer(Random)

struct InputData {
    unsigned long seed;
    Player* player;
    Team ownTeam;
};

int main() {
    std::mt19937 mt19937{std::random_device{}()};

    ThreadPool<InputData, Team> pool{[](std::ofstream& log, InputData data) -> Team {
        std::mt19937 mt19937{data.seed};
        std::uniform_int_distribution<> pieceDist{0, 7};
        std::string fen = "HHMMSSRR/8/8/8/8/8/8/rrssmmhh 0";

        for (int n = 0; n < 20; ++n) {
            int a = pieceDist(mt19937);
            int b = pieceDist(mt19937);
            std::swap(fen[a], fen[b]);
            std::swap(fen[28 - a], fen[28 - b]);
        }

        GameState gameState{fen};

        AlphaBeta alphaBeta{gameState};

        Team winner = NO_TEAM;

        while (!gameState.isOver()) {
            if (gameState.getPossibleMoves().size() == 0) {
                if (gameState.turn % 2 == ONE) winner = ONE;
                else winner = TWO;

                break;
            }

            Move move;

            if (gameState.turn % 2 == data.ownTeam) {
                Time start = std::chrono::system_clock::now();
                alphaBeta.start = start - std::chrono::milliseconds(1950 - 15);
                alphaBeta.timeOut = false;

                for (int depth = 1; depth <= 20; ++depth) {
                    MoveValuePair moveValuePair = alphaBeta.alphaBetaRoot(depth, -INT_MAX, INT_MAX);

                    if (alphaBeta.timeOut && depth > 1) break;

                    move = moveValuePair.move;

                    if (alphaBeta.timeOut || moveValuePair.value >= INT_MAX) break;
                }
            } else {
                move = data.player->run(gameState, mt19937);
            }

            gameState.makeMove(move);
        }

        if (winner == NO_TEAM) winner = gameState.calcWinner();

        if (data.ownTeam == TWO) {
            if (winner == ONE) winner = TWO;
            else if (winner == TWO) winner = ONE;
        }

        return winner;
    }};

    std::vector<Player*> players{};
    PLAYERS();

    int wins = 0;
    int draws = 0;
    int losses = 0;

    for (int j = 0; j < players.size(); ++j) {
        for (int k = 0; k < GAME_COUNT; k++) {
            pool.push({mt19937(), players[j], ONE});
            pool.push({mt19937(), players[j], TWO});
        }

        for (int k = 0; k < GAME_COUNT * 2; k++) {
            Team winner = pool.pop();

            if (winner == ONE) wins++;
            else if (winner == TWO) losses++;
            else draws++;
        }
    }

    int games = wins + draws + losses;
    double score = wins + .5 * draws;
    double winRatio = score / games;

    printf("%f\n", winRatio);

    return 0;
}
