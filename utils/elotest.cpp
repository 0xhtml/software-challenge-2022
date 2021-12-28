#include <fstream>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "players/engine.hpp"
#include "helper.hpp"
#include "threadpool.hpp"

#define GAME_COUNT 160

struct InputData {
    unsigned long seed;
    int player;
    Team ownTeam;
};

int main() {
    std::mt19937 mt19937{std::random_device{}()};

    ThreadPool<InputData, Team> pool{[](std::ofstream& log, InputData data) -> Team {
        std::mt19937 mt19937{data.seed};

        GameState gameState = Helper::createGameState(mt19937);

        Engine engine{gameState, mt19937};

        NEW_PLAYER(player, data.player);

        Team winner;

        if (data.ownTeam == ONE) {
            winner = Helper::playGame(gameState, &engine, player, log);
        } else {
            winner = Helper::playGame(gameState, player, &engine, log);

            if (winner == ONE) winner = TWO;
            else if (winner == TWO) winner = ONE;
        }

        delete player;

        return winner;
    }};

    int wins = 0;
    int draws = 0;
    int losses = 0;

    for (int j = 0; j < PLAYER_COUNT; ++j) {
        for (int k = 0; k < GAME_COUNT; k++) {
            pool.push({mt19937(), j, ONE});
            pool.push({mt19937(), j, TWO});
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
