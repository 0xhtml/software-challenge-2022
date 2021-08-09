#include "../src/alphabeta.hpp"
#include "../src/gamestate.hpp"

int main() {
    GameState gameState{"HHMMSSRR/8/8/8/8/8/8/hhmmssrr"};
    AlphaBeta alpaBeta{};

    /* for (int i = 0; i < 30; ++i) { */
        alpaBeta.iterativeDeepening(gameState);
    /* } */

    return 0;
}
