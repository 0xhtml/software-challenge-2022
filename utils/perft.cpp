#include <iostream>
#include <string>

#include "../src/gamestate.hpp"
#include "../src/types.hpp"

int perft(GameState &gameState, const int depth) {
    if (depth <= 1) return gameState.getPossibleMoves().size();

    int num = 0;

    for (const Move move : gameState.getPossibleMoves()) {
        SaveState saveState = gameState.makeMove(move);
        num += perft(gameState, depth - 1);
        gameState.unmakeMove(move, saveState);
    }

    return num;
}

int main(int argc, char **argv) {
    while (true) {
        std::string inp;
        getline(std::cin, inp);

        int a = std::stoi(inp.substr(inp.find_last_of(" ")));

        GameState state{inp.substr(0, inp.find_last_of(" "))};

        int g = perft(state, 3);

        if (g != a) {
            printf("%s -> %i\n", inp.data(), g);
        }
    }

    return 0;
}
