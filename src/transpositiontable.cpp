#include "transpositiontable.hpp"

#include <cstdlib>

#include "gamestate.hpp"

TranspositionTable::TranspositionTable() {
    int size = sizeof(Transposition) * TRANSPOSITION_TABLE_SIZE;
    table = (Transposition *) std::malloc(size);
}

TranspositionTable::~TranspositionTable() {
    std::free(table);
}

int calcIndex(const uint64_t hash) {
    return hash >> (64 - TRANSPOSITION_TABLE_BITS);
}

Transposition TranspositionTable::get(const uint64_t hash) const {
    Transposition transposition = table[calcIndex(hash)];

    if (transposition.hash != hash) return {EMPTY};

    return transposition;
}

void TranspositionTable::put(const Transposition &transposition) const {
    table[calcIndex(transposition.hash)] = transposition;
};
