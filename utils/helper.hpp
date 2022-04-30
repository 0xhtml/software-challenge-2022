#pragma once

#include <fstream>
#include <string>
#include <random>
#include <assert.h>

#include "../src/gamestate.hpp"
#include "../src/types.hpp"

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
        if (field.occupied && field.stacked) str.push_back('.');
        return str;
    }

    static std::string boardToString(const GameState &gameState) {
        std::string str = "";

        for (int x = 0; x < 8; ++x) {
            int n = 0;

            for (int y = 0; y < 8; ++y) {
                Position pos{};
                pos.coords.x = x;
                pos.coords.y = y;
                const Field &field = gameState.board[pos.square];

                if (!field.occupied) {
                    n++;
                    continue;
                }

                if (n != 0) {
                    str.append(std::to_string(n));
                    n = 0;
                }

                str.append(fieldToString(field));
            }

            if (n != 0) str.append(std::to_string(n));
            if (x != 7) str.push_back('/');
        }

        return str;
    }

    static GameState createGameState(const std::string fen) {
        GameState gameState{};

        Position position{};

        for (char c : fen) {
            if (c == ' ') break;

            if (isdigit(c)) {
                position.coords.y += c - '0';
                continue;
            }

            if (c == '/') {
                position.coords.y = 0;
                position.coords.x++;
                continue;
            }

            if (c == '.') {
                gameState.board[position.square - 1].stacked = true;
                continue;
            }

            gameState.board[position.square].occupied = true;

            if (isupper(c)) {
                gameState.board[position.square].team = ONE;
                c = tolower(c);
            } else {
                gameState.board[position.square].team = TWO;
            }

            switch (c) {
                case 'h':
                    gameState.board[position.square].pieceType = HERZMUSCHEL;
                    break;
                case 'm':
                    gameState.board[position.square].pieceType = MOEWE;
                    break;
                case 's':
                    gameState.board[position.square].pieceType = SEESTERN;
                    break;
                case 'r':
                    gameState.board[position.square].pieceType = ROBBE;
                    break;
                default:
                    assert(false);
            }

            position.coords.y++;
        }

        gameState.score[ONE] = std::stoi(fen.substr(fen.find(' '), 2));
        gameState.score[TWO] = std::stoi(fen.substr(fen.find(' ') + 3, 2));

        gameState.turn = std::stoi(fen.substr(fen.find(' ') + 5));

        return gameState;
    }

    static GameState createGameState(std::mt19937 &mt19937) {
        std::uniform_int_distribution<> pieceDist{0, 7};

        std::string fen = "HHMMSSRR/8/8/8/8/8/8/rrssmmhh 0-0 0";

        for (int n = 0; n < 20; ++n) {
            int a = pieceDist(mt19937);
            int b = pieceDist(mt19937);
            std::swap(fen[a], fen[b]);
            std::swap(fen[28 - a], fen[28 - b]);
        }

        return createGameState(fen);
    }
};
