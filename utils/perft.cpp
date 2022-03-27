#include <iostream>
#include <string>

#include "../src/gamestate.hpp"
#include "../src/types.hpp"
#include "helper.hpp"

int main(int argc, char **argv) {
    while (true) {
        std::string inp;
        getline(std::cin, inp);

        int a = std::stoi(inp.substr(inp.find_last_of(" ")));

        GameState state = Helper::createGameState(inp.substr(0, inp.find_last_of(" ")));

        int g = state.getPossibleMoves().size();

        if (g != a) {
            printf("!!! %s -> %i\n", inp.data(), g);
        } else {
            printf("%s -> ok\n", inp.data());
        }
    }

    return 0;
}
