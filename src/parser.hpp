#pragma once

#include <pugixml.hpp>

#include "gamestate.hpp"
#include "types.hpp"

class Parser {
public:
    static int parseCoord(const pugi::xml_attribute &xml);

    static Position parsePosition(const pugi::xml_node &xml);

    static Move parseMove(const pugi::xml_node &xml);

    static Team parseTeam(const pugi::xml_attribute &xml);

    static PieceType parsePieceType(const pugi::xml_attribute &xml);

    static bool parsePieceStacked(const pugi::xml_attribute &xml);

    static void parseGameState(const pugi::xml_node &xml, GameState &gameState);

    static std::string encodeCoord(const int coord);

    static std::string encodePosition(const Position &position);

    static std::string encodeMove(const Move &move);
};
