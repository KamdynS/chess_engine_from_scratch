#include "include/BitBoard.h"
#include "include/CommonComponents.h"


void Bitboard::set(int square) {
    if (square >= 0 && square < 64) {
        board |= (1ULL << square);
    }
}

void Bitboard::clear(int square) {
    if (square >= 0 && square < 64) {
        board &= ~(1ULL << square);
    }
}

bool Bitboard::isOccupied(int square) const {
    if (square >= 0 && square < 64) {
        return (board & (1ULL << square)) != 0;
    }
    return false;
}

