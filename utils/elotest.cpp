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

#define GAME_COUNT 100
#define PLAYERS() \
    addPlayer(Material); \
    addPlayer(Mauer); \
    addPlayer(Minimus); \
    addPlayer(Random)

struct InputData {
    unsigned long seed;
    Player* player;
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

            if (gameState.turn % 2 == ONE) {
                alphaBeta.start = std::chrono::system_clock::now();
                alphaBeta.timeOut = false;

                MoveValuePair result;
                for (int d = 1; d <= 5; ++d) {
                    result = alphaBeta.alphaBetaRoot(d, -INT_MAX, INT_MAX);
                    if (result.value >= INT_MAX) break;
                }

                move = result.move;
            } else {
                move = data.player->run(gameState, mt19937);
            }

            gameState.makeMove(move);
        }

        if (winner == NO_TEAM) winner = gameState.calcWinner();

        return winner;
    }};

    std::vector<Player*> players{};
    PLAYERS();

    int score[]{0, 0};

    for (int j = 0; j < players.size(); ++j) {
        for (int k = 0; k < GAME_COUNT; k++) {
            pool.push({mt19937(), players[j]});
        }

        for (int k = 0; k < GAME_COUNT; k++) {
            Team winner = pool.pop();
            if (winner != NO_TEAM) score[winner]++;
        }
    }

    printf("%i;%i\n", score[ONE], score[TWO]);

    return 0;
}
