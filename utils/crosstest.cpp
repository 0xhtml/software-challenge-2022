#include <fstream>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "threadpool.hpp"
#include "helper.hpp"

#define GAME_COUNT 100

struct InputData {
    unsigned long seed;
    int one;
    int two;
};

int main() {
    std::mt19937 mt19937{std::random_device{}()};

    ThreadPool<InputData, Team> pool{[](std::ofstream& log, InputData data) -> Team {
        std::mt19937 mt19937{data.seed};

        GameState gameState = Helper::createGameState(mt19937);

        NEW_PLAYER(one, data.one);
        NEW_PLAYER(two, data.one);

        Team winner = Helper::playGame(gameState, one, two, log);

        delete one;
        delete two;

        return winner;
    }};

    PLAYER_NAMES(names);

    printf(",");
    for (int i = 0; i < PLAYER_COUNT; ++i) {
        printf("%s,", names[i].c_str());
    }
    printf("\n");

    for (int i = 0; i < PLAYER_COUNT; ++i) {
        printf("%s,", names[i].c_str());

        for (int j = 0; j < PLAYER_COUNT; ++j) {
            if (i == j) {
                printf(",");
                continue;
            }

            int score[]{0, 0};

            for (int k = 0; k < GAME_COUNT; k++) {
                pool.push({mt19937(), i, j});
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
