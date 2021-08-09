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
            position.coords.x += c - 48;
            continue;
        }

        if (c == '/') {
            position.coords.x = 0;
            position.coords.y++;
            continue;
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

        position.coords.x++;
    }
}

void GameState::pushDestination(std::vector<Move> &moves, Position from, Position to) const {
    if (!board[to.square].occupied || board[to.square].color != turn % 2) {
        moves.push_back({from, to});
    }
}

std::vector<Move> GameState::getPossibleMoves() const {
    std::vector<Move> moves{};

    int color = turn % 2;

    Direction forward = color ? DOWN : UP;
    uint8_t oppBaseline = color ? 0 : 7;

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field field = board[square];

        if (!field.occupied) continue;
        if (field.color != color) continue;

        const Position pos{square};

        switch (field.pieceType) {
            case HERZMUSCHEL:
                if (pos.coords.x == oppBaseline) break;

                if (pos.coords.y != 7) {
                    pushDestination(moves, pos, pos + forward + LEFT);
                }
                if (pos.coords.y != 0) {
                    pushDestination(moves, pos, pos + forward + RIGHT);
                }

                break;
            case MOEWE:
                if (pos.coords.x != 8) {
                    pushDestination(moves, pos, pos + UP);
                }
                if (pos.coords.x != 0) {
                    pushDestination(moves, pos, pos + DOWN);
                }
                if (pos.coords.y != 7) {
                    pushDestination(moves, pos, pos + LEFT);
                }
                if (pos.coords.y != 0) {
                    pushDestination(moves, pos, pos + RIGHT);
                }

                break;
            case SEESTERN:
                if (pos.coords.x != oppBaseline) {
                    pushDestination(moves, pos, pos + forward);
                }

                if (pos.coords.x != 7 && pos.coords.y != 7) {
                    pushDestination(moves, pos, pos + UP + LEFT);
                }
                if (pos.coords.x != 7 && pos.coords.y != 0) {
                    pushDestination(moves, pos, pos + UP + RIGHT);
                }
                if (pos.coords.x != 0 && pos.coords.y != 7) {
                    pushDestination(moves, pos, pos + DOWN + LEFT);
                }
                if (pos.coords.x != 0 && pos.coords.y != 0) {
                    pushDestination(moves, pos, pos + DOWN + RIGHT);
                }

                break;
            case ROBBE:
                if (pos.coords.x < 6 && pos.coords.y != 7) {
                    pushDestination(moves, pos, pos + UP + UP + LEFT);
                }
                if (pos.coords.x < 6 && pos.coords.y != 0) {
                    pushDestination(moves, pos, pos + UP + UP + RIGHT);
                }

                if (pos.coords.x > 1 && pos.coords.y != 7) {
                    pushDestination(moves, pos, pos + DOWN + DOWN + LEFT);
                }
                if (pos.coords.x > 1 && pos.coords.y != 0) {
                    pushDestination(moves, pos, pos + DOWN + DOWN + RIGHT);
                }

                if (pos.coords.x != 7 && pos.coords.y < 6) {
                    pushDestination(moves, pos, pos + UP + LEFT + LEFT);
                }
                if (pos.coords.x != 7 && pos.coords.y > 1) {
                    pushDestination(moves, pos, pos + UP + RIGHT + RIGHT);
                }

                if (pos.coords.x != 0 && pos.coords.y < 6) {
                    pushDestination(moves, pos, pos + DOWN + LEFT + LEFT);
                }
                if (pos.coords.x != 0 && pos.coords.y > 1) {
                    pushDestination(moves, pos, pos + DOWN + RIGHT + RIGHT);
                }

                break;
        }
    }

    return moves;
}

SaveState GameState::makeMove(const Move &move) {
    Field &from = board[move.from.square];
    Field &to = board[move.to.square];

    SaveState saveState{from, to};

    to.occupied = !(to.occupied && to.stacked);
    to.color = from.color;
    to.stackedPieceType = to.pieceType;
    to.pieceType = from.pieceType;

    from.pieceType = from.stackedPieceType;
    from.color = !from.color;
    from.occupied = from.stacked;
    from.stacked = false;

    ++turn;

    return saveState;
}

void GameState::unmakeMove(const Move &move, const SaveState &saveState) {
    board[move.from.square] = saveState.from;
    board[move.to.square] = saveState.to;

    --turn;
}
