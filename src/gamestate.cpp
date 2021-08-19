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

#define pushMove(to) \
    if (!board[square + to].occupied || board[square + to].color != color) { \
        moves.push_back({pos, square + to}); \
    }

std::vector<Move> GameState::getPossibleMoves() const {
    std::vector<Move> moves{};

    const int color = turn % 2;

    const Direction forward = color ? DOWN : UP;
    const uint8_t oppBaseline = color ? 0 : 7;

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field &field = board[square];

        if (!field.occupied) continue;
        if (field.color != color) continue;

        const Position pos{square};

        switch (field.pieceType) {
            case HERZMUSCHEL:
                if (pos.coords.x != oppBaseline) {
                    if (pos.coords.y < 7) pushMove(forward + LEFT);
                    if (pos.coords.y > 0) pushMove(forward + RIGHT);
                }
                break;
            case MOEWE:
                if (pos.coords.x < 7) pushMove(UP);
                if (pos.coords.x > 0) pushMove(DOWN);
                if (pos.coords.y < 7) pushMove(LEFT);
                if (pos.coords.y > 0) pushMove(RIGHT);
                break;
            case SEESTERN:
                if (pos.coords.x != oppBaseline) pushMove(forward);
                if (pos.coords.x < 7) {
                    if (pos.coords.y < 7) pushMove(UP + LEFT);
                    if (pos.coords.y > 0) pushMove(UP + RIGHT);
                }
                if (pos.coords.x > 0) {
                    if (pos.coords.y < 7) pushMove(DOWN + LEFT);
                    if (pos.coords.y > 0) pushMove(DOWN + RIGHT);
                }
                break;
            case ROBBE:
                if (pos.coords.x < 6) {
                    if (pos.coords.y < 7) pushMove(UP + UP + LEFT);
                    if (pos.coords.y > 0) pushMove(UP + UP + RIGHT);
                }
                if (pos.coords.x > 1) {
                    if (pos.coords.y < 7) pushMove(DOWN + DOWN + LEFT);
                    if (pos.coords.y > 0) pushMove(DOWN + DOWN + RIGHT);
                }
                if (pos.coords.x < 7) {
                    if (pos.coords.y < 6) pushMove(UP + LEFT + LEFT);
                    if (pos.coords.y > 1) pushMove(UP + RIGHT + RIGHT);
                }
                if (pos.coords.x > 0) {
                    if (pos.coords.y < 6) pushMove(DOWN + LEFT + LEFT);
                    if (pos.coords.y > 1) pushMove(DOWN + RIGHT + RIGHT);
                }
                break;
        }
    }

    return moves;
}

bool GameState::isOver() const {
    if (turn > 60) return true;
    if (turn % 2) return false;
    for (int color = 0; color < COLOR_COUNT; ++color) {
        if (score[color] >= 2) return true;
    }
    return false;
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
