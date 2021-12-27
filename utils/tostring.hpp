#pragma once

#include <string>

#include "../src/gamestate.hpp"

class ToString {
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
};
