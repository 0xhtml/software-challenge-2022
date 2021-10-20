#include "parser.hpp"

#include <assert.h>
#include <cstring>

#include "types.hpp"

int Parser::parseCoord(const pugi::xml_attribute &xml) {
    int coord = xml.as_int(-1);

    assert(coord >= 0);
    assert(coord < 8);

    return coord;
}

Position Parser::parsePosition(const pugi::xml_node &xml) {
    Position position;

    position.coords.x = parseCoord(xml.attribute("x"));
    position.coords.y = parseCoord(xml.attribute("y"));

    return position;
}

Move Parser::parseMove(const pugi::xml_node &xml) {
    Move move;

    move.from = parsePosition(xml.child("from"));
    move.to = parsePosition(xml.child("to"));

    assert(move.from != move.to);

    return move;
}

Team Parser::parseTeam(const pugi::xml_attribute &xml) {
    Team team{};

    if (std::strcmp(xml.value(), "TWO") == 0) {
        team = TWO;
    } else {
        assert(std::strcmp(xml.value(), "ONE") == 0);
    }

    return team;
}

PieceType Parser::parsePieceType(const pugi::xml_attribute &xml) {
    PieceType pieceType{};

    if (std::strcmp(xml.value(), "Moewe") == 0) {
        pieceType = MOEWE;
    } else if (std::strcmp(xml.value(), "Seestern") == 0) {
        pieceType = SEESTERN;
    } else if (std::strcmp(xml.value(), "Robbe") == 0) {
        pieceType = ROBBE;
    } else {
        assert(std::strcmp(xml.value(), "Herzmuschel") == 0);
    }

    return pieceType;
}

bool Parser::parsePieceStacked(const pugi::xml_attribute &xml) {
    int count = xml.as_int(-1);

    assert(count == 0 || count == 1);

    return count == 1;
}

GameState Parser::parseGameState(const pugi::xml_node &xml) {
    GameState gameState{};

    // turn = xml.attribute("turn").as_int();
    // TODO
    // start team, turn?

    pugi::xml_node pieces = xml.child("board").child("pieces");
    for (const pugi::xml_node &piece : pieces.children()) {
        Position position = parsePosition(piece.child("coordinates"));
        Field &field = gameState.board[position.square];

        assert(!field.occupied);

        field.occupied = true;

        const pugi::xml_node &child = piece.child("piece");
        field.team = parseTeam(child.attribute("team"));
        field.pieceType = parsePieceType(child.attribute("type"));
        field.stacked = parsePieceStacked(child.attribute("count"));

        field.stacked = 0;
    }

    return gameState;
}

std::string Parser::encodeCoord(const int coord) {
    assert(coord >= 0);
    assert(coord < 8);

    return std::to_string(coord);
}

std::string Parser::encodePosition(const Position &position) {
    std::string xml;

    xml.append("x=\"");
    xml.append(encodeCoord(position.coords.x));
    xml.append("\" y=\"");
    xml.append(encodeCoord(position.coords.y));
    xml.append("\"");

    return xml;
}

std::string Parser::encodeMove(const Move &move) {
    std::string xml;

    xml.append("<data class=\"move\"><from ");
    xml.append(encodePosition(move.from));
    xml.append("/><to ");
    xml.append(encodePosition(move.to));
    xml.append("/></data>");

    return xml;
}
