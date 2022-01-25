#include "transpositiontable.hpp"

#include <assert.h>
#include <cstdlib>

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

bool replace(const Transposition &stored, const Transposition &replacement) {
    if (stored.type == EMPTY) return true;
    if (replacement.depth >= stored.depth) return true;
    if (replacement.turn > stored.turn + stored.depth) return true;
    return false;
}

void TranspositionTable::put(const Transposition &transposition) const {
    int index = calcIndex(transposition.hash);
    Transposition storedTransposition = table[index];

    if (replace(storedTransposition, transposition)) {
        table[index] = transposition;
    }
}
