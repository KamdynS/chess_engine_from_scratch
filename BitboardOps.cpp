#include "BitboardOps.h"

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
}

    Bitboard getLSB(Bitboard b) {
        Bitboard result;
        result.board = b.board & (-b.board);
        return result;
    }

    Bitboard popLSB(Bitboard& b) {
        // Implement this function
    }

    int bitScanForward(Bitboard b) {
        // Implement this function
    }

} // namespace BitboardOps