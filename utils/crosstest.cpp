#include <random>
#include <string>
#include <utility>
#include <vector>

#include "../src/gamestate.hpp"
#include "players/material.hpp"
#include "players/mauer.hpp"
#include "players/minimus.hpp"
#include "players/random.hpp"
#include "threadpool.hpp"

#define addPlayer(t) t t##_; players.push_back(&t##_); names.push_back(#t)

#define GAME_COUNT 100
#define PLAYERS() \
    addPlayer(Material); \
    addPlayer(Mauer); \
    addPlayer(Minimus); \
    addPlayer(Random)

struct InputData {
    unsigned long seed;
    Player* one;
    Player* two;
};

int main() {
    std::mt19937 mt19937{std::random_device{}()};

    ThreadPool<InputData, Team> pool{[](InputData data) -> Team {
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

        Team winner = NO_TEAM;

        while (!gameState.isOver()) {
            if (gameState.getPossibleMoves().size() == 0) {
                if (gameState.turn % 2 == ONE) winner = ONE;
                else winner = TWO;

                break;
            }

            if (gameState.turn % 2 == ONE) {
                gameState.makeMove(data.one->run(gameState, mt19937));
            } else {
                gameState.makeMove(data.two->run(gameState, mt19937));
            }
        }

        if (winner == NO_TEAM) winner = gameState.calcWinner();

        return winner;
    }};

    std::vector<Player*> players{};
    std::vector<std::string> names{};
    PLAYERS();

    int count = players.size();

    printf(",");
    for (int i = 0; i < count; ++i) {
        printf("%s,", names[i].c_str());
    }
    printf("\n");

    for (int i = 0; i < count; ++i) {
        printf("%s,", names[i].c_str());

        for (int j = 0; j < count; ++j) {
            if (i == j) {
                printf(",");
                continue;
            }

            int score[]{0, 0};

            for (int k = 0; k < GAME_COUNT; k++) {
                pool.push({mt19937(), players[i], players[j]});
            }

            for (int k = 0; k < GAME_COUNT; k++) {
                Team winner = pool.pop();
                if (winner != NO_TEAM) score[winner]++;
            }

            printf("%i:%i,", score[ONE], score[TWO]);
        }

        printf("\n");
    }

    return 0;
}
