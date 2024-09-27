#pragma once
#include "Bitboard.h"

namespace BitboardOps {
    Bitboard northOne(Bitboard b);
    Bitboard southOne(Bitboard b);
    Bitboard eastOne(Bitboard b);
    Bitboard westOne(Bitboard b);
    Bitboard northEastOne(Bitboard b);
    Bitboard northWestOne(Bitboard b);
    Bitboard southEastOne(Bitboard b);
    Bitboard southWestOne(Bitboard b);

    Bitboard getLSB(Bitboard b);
    Bitboard popLSB(Bitboard& b);
    int bitScanForward(Bitboard b);

    // Constants
    const Bitboard NOT_A_FILE = Bitboard(0xfefefefefefefefe);  // ~0x0101010101010101
    const Bitboard NOT_H_FILE = Bitboard(0x7f7f7f7f7f7f7f7f);  // ~0x8080808080808080
}