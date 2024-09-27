#include "include/BitboardOps.h"
#include "include/CommonComponents.h"

namespace BitboardOps {

    Bitboard northOne(Bitboard b) {
        Bitboard result;
        result.board = b.board << 8;
        return result;
    }

    Bitboard southOne(Bitboard b) {
        Bitboard result;
        result.board = b.board >> 8;
        return result;
    }

    Bitboard eastOne(Bitboard b) {
        Bitboard result;
        result.board = (b.board << 1) & NOT_A_FILE.board;
        return result;
    }

    Bitboard westOne(Bitboard b) {
        Bitboard result;
        result.board = (b.board >> 1) & NOT_H_FILE.board;
        return result;
    }

    Bitboard northEastOne(Bitboard b) {
        Bitboard result;
        result.board = (b.board << 9) & NOT_A_FILE.board;
        return result;
    }

    Bitboard northWestOne(Bitboard b) {
        Bitboard result;
        result.board = (b.board << 7) & NOT_H_FILE.board;
        return result;
    }

    Bitboard southEastOne(Bitboard b) {
        Bitboard result;
        result.board = (b.board >> 7) & NOT_A_FILE.board;
        return result;
    }

    Bitboard southWestOne(Bitboard b) {
        Bitboard result;
        result.board = (b.board >> 9) & NOT_H_FILE.board;
        return result;
    }

    Bitboard getLSB(Bitboard b) {
        Bitboard result;
        result.board = b.board & (~b.board + 1);
        return result;
    }

    Bitboard popLSB(Bitboard& b) {
        // Return LSB
        Bitboard result;
        result.board = b.board & (~b.board + 1);

        // Also get rid of it from the input
        b.board ^= result.board;
        return result;
    }
    // Method using de Bruijn sequence. Very efficient. Not my own code but super fascinating and works well
    // Can be found here: https://www.chessprogramming.org/BitScan#DeBruijnMultiplation
    // Referencing for my own sake

    const int index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
    };

    int bitScanForward(Bitboard b) {
        const uint64_t debruijn64 = 0x03f79d71b4cb0a89ULL;
        if (b.board == 0) return -1;  // Special case for empty bitboard

        // Isolate LSB. The rest of this cannot be explained with inline comments
        return index64[((b.board & (~b.board + 1)) * debruijn64) >> 58];
    }

}