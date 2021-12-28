#pragma once

#include <fstream>

#include "players/material.hpp"
#include "players/mauer.hpp"
#include "players/minimus.hpp"
#include "players/queren.hpp"
#include "players/random.hpp"

#define PLAYER_COUNT 5
#define NEW_PLAYER(v, i) \
    Player* v; \
    if (i == 0) v = new Material{gameState, mt19937}; \
    else if (i == 1) v = new Mauer{gameState, mt19937}; \
    else if (i == 2) v = new Minimus{gameState, mt19937}; \
    else if (i == 3) v = new Queren{gameState, mt19937}; \
    else v = new Random{gameState, mt19937}
#define PLAYER_NAMES(v) \
    std::vector<std::string> v{"Material", "Mauer", "Minimus", "Queren", "Random"};

class Helper {
public:
    static char fieldToUpperChar(const Field &field) {
        if (!field.occupied) return '0';

        PieceType t = field.pieceType;

        if (t == HERZMUSCHEL) return 'H';
        if (t == MOEWE) return 'M';
        if (t == SEESTERN) return 'S';
        if (t == ROBBE) return 'R';

        return 'E';
    }

    static std::string fieldToString(const Field &field) {
        char c = fieldToUpperChar(field);
        if (field.team == TWO) c = std::tolower(c);

        std::string str = "";
        str.push_back(c);
        if (field.stacked) str.push_back('.');
        return str;
    }

    static std::string boardToString(const GameState &gameState) {
        std::string str = "";

        for (int x = 0; x < 8; ++x) {
            for (int y = 0; y < 8; ++y) {
                Position pos{};
                pos.coords.x = x;
                pos.coords.y = y;

                str.append(fieldToString(gameState.board[pos.square]));

            }
            if (x != 7) str.push_back('/');
        }

        return str;
    }

    static GameState createGameState(std::mt19937 &mt19937) {
        std::uniform_int_distribution<> pieceDist{0, 7};

        std::string fen = "HHMMSSRR/8/8/8/8/8/8/rrssmmhh 0";

        for (int n = 0; n < 20; ++n) {
            int a = pieceDist(mt19937);
            int b = pieceDist(mt19937);
            std::swap(fen[a], fen[b]);
            std::swap(fen[28 - a], fen[28 - b]);
        }

        return GameState{fen};
    }

    static Team playGame(GameState &gameState, Player* one, Player* two, std::ofstream& log) {
        Team winner = NO_TEAM;

        std::vector<std::string> strs{};

        while (!gameState.isOver()) {
            if (gameState.getPossibleMoves().size() == 0) {
                if (gameState.turn % 2 == ONE) winner = ONE;
                else winner = TWO;

                break;
            }

            std::string str = boardToString(gameState);

            Move move;

            if (gameState.turn % 2 == ONE) {
                move = one->run();
            } else {
                move = two->run();
            }

            str.push_back(',');
            str.append(fieldToString(gameState.board[move.from.square]));

            gameState.makeMove(move);

            str.push_back(',');
            str.append(fieldToString(gameState.board[move.to.square]));

            strs.push_back(str);
        }

        if (winner == NO_TEAM) winner = gameState.calcWinner();

        for (std::string str : strs) {
            log << str;
            log << ",";
            switch (winner) {
                case ONE:
                    log << 1;
                    break;
                case TWO:
                    log << 0;
                    break;
                case NO_TEAM:
                    log << 0.5;
            }
            log << std::endl;
        }

        return winner;
    }
};
