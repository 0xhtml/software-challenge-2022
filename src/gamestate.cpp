#include "gamestate.hpp"

#include <algorithm>
#include <pugixml.hpp>
#include <string>
#include <vector>

#include "types.hpp"

GameState::GameState(const std::string fen) {
    Position position{};

    for (char c : fen) {
        if (isdigit(c)) {
            position.coords.y += c - 48;
            continue;
        }

        if (c == '/') {
            position.coords.y = 0;
            position.coords.x++;
            continue;
        }

        if (c == ' ') {
            break;
        }

        board[position.square].occupied = true;

        if (isupper(c)) {
            board[position.square].color = RED;
            c = tolower(c);
        } else {
            board[position.square].color = BLUE;
        }

        switch (c) {
            case 'h':
                board[position.square].pieceType = HERZMUSCHEL;
                break;
            case 'm':
                board[position.square].pieceType = MOEWE;
                break;
            case 's':
                board[position.square].pieceType = SEESTERN;
                break;
            case 'r':
                board[position.square].pieceType = ROBBE;
                break;
        }

        position.coords.y++;
    }

    turn = std::stoi(fen.substr(fen.find(' ')));
}

#define pushDestination(from, to) \
    if (!board[(to).square].occupied || board[(to).square].color != color) { \
        moves.push_back({from, to}); \
    }

std::vector<Move> GameState::getPossibleMoves() const {
    std::vector<Move> moves{};

    const int color = turn % 2;

    const Direction forward = color ? DOWN : UP;
    const uint8_t oppBaseline = color ? 0 : 7;

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field field = board[square];

        if (!field.occupied) continue;
        if (field.color != color) continue;

        const Position pos{square};

        switch (field.pieceType) {
            case HERZMUSCHEL:
                if (pos.coords.x == oppBaseline) break;

                if (pos.coords.y < 7) {
                    pushDestination(pos, pos + forward + LEFT);
                }
                if (pos.coords.y > 0) {
                    pushDestination(pos, pos + forward + RIGHT);
                }

                break;
            case MOEWE:
                if (pos.coords.x < 7) {
                    pushDestination(pos, pos + UP);
                }
                if (pos.coords.x > 0) {
                    pushDestination(pos, pos + DOWN);
                }
                if (pos.coords.y < 7) {
                    pushDestination(pos, pos + LEFT);
                }
                if (pos.coords.y > 0) {
                    pushDestination(pos, pos + RIGHT);
                }

                break;
            case SEESTERN:
                if (pos.coords.x != oppBaseline) {
                    pushDestination(pos, pos + forward);
                }

                if (pos.coords.x < 7 && pos.coords.y < 7) {
                    pushDestination(pos, pos + UP + LEFT);
                }
                if (pos.coords.x < 7 && pos.coords.y > 0) {
                    pushDestination(pos, pos + UP + RIGHT);
                }
                if (pos.coords.x > 0 && pos.coords.y < 7) {
                    pushDestination(pos, pos + DOWN + LEFT);
                }
                if (pos.coords.x > 0 && pos.coords.y > 0) {
                    pushDestination(pos, pos + DOWN + RIGHT);
                }

                break;
            case ROBBE:
                if (pos.coords.x < 6 && pos.coords.y < 7) {
                    pushDestination(pos, pos + UP + UP + LEFT);
                }
                if (pos.coords.x < 6 && pos.coords.y > 0) {
                    pushDestination(pos, pos + UP + UP + RIGHT);
                }

                if (pos.coords.x > 1 && pos.coords.y < 7) {
                    pushDestination(pos, pos + DOWN + DOWN + LEFT);
                }
                if (pos.coords.x > 1 && pos.coords.y > 0) {
                    pushDestination(pos, pos + DOWN + DOWN + RIGHT);
                }

                if (pos.coords.x < 7 && pos.coords.y < 6) {
                    pushDestination(pos, pos + UP + LEFT + LEFT);
                }
                if (pos.coords.x < 7 && pos.coords.y > 1) {
                    pushDestination(pos, pos + UP + RIGHT + RIGHT);
                }

                if (pos.coords.x > 0 && pos.coords.y < 6) {
                    pushDestination(pos, pos + DOWN + LEFT + LEFT);
                }
                if (pos.coords.x > 0 && pos.coords.y > 1) {
                    pushDestination(pos, pos + DOWN + RIGHT + RIGHT);
                }

                break;
        }
    }

    return moves;
}

SaveState GameState::makeMove(const Move &move) {
    Field &from = board[move.from.square];
    Field &to = board[move.to.square];

    int color = turn % 2;
    uint8_t oppBaseline = color ? 0 : 7;

    SaveState saveState{from, to, score[color]};

    int points = (
        (to.occupied && (from.stacked || to.stacked)) +
        (move.to.coords.x == oppBaseline && from.pieceType != ROBBE)
    );

    if (points > 0) {
        to.occupied = false;
        score[color] += points;
    } else {
        to.stacked = to.occupied || from.stacked;
        to.occupied = true;
        to.color = from.color;
        to.pieceType = from.pieceType;
    }

    from.occupied = false;

    ++turn;

    return saveState;
}

void GameState::unmakeMove(const Move &move, const SaveState &saveState) {
    --turn;

    board[move.from.square] = saveState.from;
    board[move.to.square] = saveState.to;
    score[turn % 2] = saveState.score;
}
