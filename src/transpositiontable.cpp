#include "transpositiontable.hpp"

#include <cstdlib>
#include <cassert>

#include "gamestate.hpp"

TranspositionTable::TranspositionTable() {
    table = (Transposition *) std::calloc(TRANSPOSITION_TABLE_SIZE, sizeof(Transposition));
}

TranspositionTable::~TranspositionTable() {
    std::free(table);
}

int calcIndex(const uint64_t hash) {
    int index = hash >> (64 - TRANSPOSITION_TABLE_BITS);

    assert(index < TRANSPOSITION_TABLE_SIZE);
    assert(index >= 0);

    return index;
}

Transposition TranspositionTable::get(const uint64_t hash) const {
    Transposition transposition = table[calcIndex(hash)];

    if (transposition.hash != hash) return {EMPTY};

    return transposition;
}

void TranspositionTable::put(const Transposition &transposition) const {
    table[calcIndex(transposition.hash)] = transposition;
}
