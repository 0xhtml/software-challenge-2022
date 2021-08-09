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
    std::string fen;

    if (argc == 1) {
        fen = "HHMMRRSS/8/8/8/8/8/8/hhmmrrss";
    } else if (argc == 2) {
        fen.append(argv[1]);
    } else {
        printf("usage: %s <fen>", argv[0]);
        return 1;
    }

    GameState gameState{fen};

    for (int depth = 1; ; ++depth) {
        printf("%i | %i\n", depth, perft(gameState, depth));
    }

    return 1;
}
