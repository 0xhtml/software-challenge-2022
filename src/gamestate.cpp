#include "gamestate.hpp"

#include <algorithm>
#include <cassert>
#include <pugixml.hpp>
#include <string>
#include <vector>

#include "types.hpp"

#define RANDOM_SEED_A 1103515495
#define RANDOM_SEED_B 12345

GameState::GameState() {
    uint64_t rand = 1;
    for (int square = 0; square < FIELD_COUNT; ++square) {
        for (int team = 0; team < TEAM_COUNT; ++team) {
            for (int piece = 0; piece < PIECE_TYPE_COUNT; ++piece) {
                rand = rand * RANDOM_SEED_A + RANDOM_SEED_B;
                zobristPiece[square][team][piece] = rand;
            }
        }
    }
    for (int square = 0; square < FIELD_COUNT; ++square) {
        rand = rand * RANDOM_SEED_A + RANDOM_SEED_B;
        zobristStacked[square] = rand;
    }
    for (int team = 0; team < TEAM_COUNT; ++team) {
        for (int score = 0; score < MAX_SCORE; ++score) {
            rand = rand * RANDOM_SEED_A + RANDOM_SEED_B;
            zobristScore[team][score] = rand;
        }
    }
    zobristTeam = rand * RANDOM_SEED_A + RANDOM_SEED_B;
}

GameState::GameState(const std::string &fen) : GameState() {
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

        if (c == '.') {
            board[position.square - 1].stacked = true;
            continue;
        }

        board[position.square].occupied = true;

        if (isupper(c)) {
            board[position.square].team = ONE;
            c = tolower(c);
        } else {
            board[position.square].team = TWO;
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
    if (!board[square + to].occupied || board[square + to].team != team) { \
        moves.push_back({pos, square + to}); \
    }

std::vector<Move> GameState::getPossibleMoves() const {
    std::vector<Move> moves{};

    const int team = turn % 2;

    const Direction forward = team ? DOWN : UP;
    const uint8_t oppBaseline = team ? 0 : 7;

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field &field = board[square];

        if (!field.occupied) continue;
        if (field.team != team) continue;

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
    if (turn > TURN_LIMIT) return true;
    if (turn % 2) return false;
    for (int team = 0; team < TEAM_COUNT; ++team) {
        if (score[team] >= MAX_SCORE) return true;
    }
    return false;
}

Team GameState::calcWinner() const {
    assert(isOver());

    if (score[ONE] > score[TWO]) return ONE;
    if (score[ONE] < score[TWO]) return TWO;

    std::vector<int> dist[TEAM_COUNT]{};

    for (int square = 0; square < FIELD_COUNT; ++square) {
        const Field &field = board[square];

        if (!field.occupied) continue;
        if (field.pieceType == ROBBE) continue;

        const Position pos{square};

        dist[field.team].push_back(field.team ? 7 - pos.coords.x : pos.coords.x);
    }

    std::sort(dist[ONE].begin(), dist[ONE].end(), std::greater<>());
    std::sort(dist[TWO].begin(), dist[TWO].end(), std::greater<>());

    for (int i = 0; i < std::min(dist[ONE].size(), dist[TWO].size()); ++i) {
        if (dist[ONE][i] > dist[TWO][i]) return ONE;
        if (dist[ONE][i] < dist[TWO][i]) return TWO;
    }

    return NO_TEAM;
}

SaveState GameState::makeMove(const Move &move) {
    assert(move.from.square != move.to.square);

    Field &from = board[move.from.square];
    Field &to = board[move.to.square];

    int team = turn % 2;
    uint8_t oppBaseline = team ? 0 : 7;

    assert(from.occupied);
    assert(from.team == team);
    assert(!to.occupied || to.team != team);

    SaveState saveState{from, to, score[team], hash};

    int points = (
        (to.occupied && (from.stacked || to.stacked)) +
        (move.to.coords.x == oppBaseline && from.pieceType != ROBBE)
    );

    if (points > 0) {
        if (to.occupied) {
            to.occupied = false;
            hash ^= zobristPiece[move.to.square][to.team][to.pieceType];
            if (to.stacked) hash ^= zobristStacked[move.to.square];
        }
        if (score[team] > 0) hash ^= zobristScore[team][score[team] - 1];
        score[team] += points;
        hash ^= zobristScore[team][score[team] - 1];
    } else {
        if (to.occupied) {
            hash ^= zobristPiece[move.to.square][to.team][to.pieceType];
        }

        to.stacked = to.occupied || from.stacked;
        if (to.stacked) hash ^= zobristStacked[move.to.square];

        to.occupied = true;
        to.team = from.team;
        to.pieceType = from.pieceType;

        hash ^= zobristPiece[move.to.square][to.team][to.pieceType];
    }

    from.occupied = false;
    hash ^= zobristPiece[move.from.square][from.team][from.pieceType];
    if (from.stacked) hash ^= zobristStacked[move.from.square];

    ++turn;
    hash ^= zobristTeam;

    assert(hash != saveState.hash);

    return saveState;
}

void GameState::unmakeMove(const Move &move, const SaveState &saveState) {
    assert(move.from.square != move.to.square);
    assert(saveState.hash != hash);

    --turn;

    assert(saveState.from.team == turn % 2);

    board[move.from.square] = saveState.from;
    board[move.to.square] = saveState.to;
    score[turn % 2] = saveState.score;
    hash = saveState.hash;
}
