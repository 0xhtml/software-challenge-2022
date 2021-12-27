#pragma once

#include "gamestate.hpp"
#include "types.hpp"

enum TranspositionType {
    EMPTY,
    EXACT,
    ALPHA,
    BETA
};

struct Transposition {
    TranspositionType type;
    uint64_t hash;
    int depth;
    int score;
    Move move;
};

#define TRANSPOSITION_TABLE_BITS 20
#define TRANSPOSITION_TABLE_SIZE (1 << TRANSPOSITION_TABLE_BITS)

class TranspositionTable {
private:
    Transposition *table;

public:
    TranspositionTable();

    Transposition get(const uint64_t hash) const;

    void put(const Transposition &transposition) const;

    ~TranspositionTable();
};
