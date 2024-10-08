#pragma once
#include <array>
#include "raylib.h"

const int BOARD_SIZE = 8;
const int TOTAL_SQUARES = 64;

using BoardState = std::array<int, TOTAL_SQUARES>;

struct Move {
    int startSquare;
    int targetSquare;
    int moveCount;
    bool isCastling = false;
    int rookStartSquare = -1;
    int rookTargetSquare = -1;
    bool isEnPassant = false;
    bool isPromotion = false;
    int promotionPiece;
};

struct GameRuleFlags {
    // White side castling
    bool a1RookHasMoved = false;
    bool h1RookHasMoved = false;
    bool whiteKingHasMoved = false;

    // Black side castling
    bool a8RookHasMoved = false;
    bool h8RookHasMoved = false;
    bool blackKingHasMoved = false;

    int enPassantTargetSquare;

    int halfMoveClock = 0;
};

struct ChessSquares {
    static const int A1 = 56, B1 = 57, C1 = 58, D1 = 59, E1 = 60, F1 = 61, G1 = 62, H1 = 63;
    static const int A2 = 48, B2 = 49, C2 = 50, D2 = 51, E2 = 52, F2 = 53, G2 = 54, H2 = 55;
    static const int A3 = 40, B3 = 41, C3 = 42, D3 = 43, E3 = 44, F3 = 45, G3 = 46, H3 = 47;
    static const int A4 = 32, B4 = 33, C4 = 34, D4 = 35, E4 = 36, F4 = 37, G4 = 38, H4 = 39;
    static const int A5 = 24, B5 = 25, C5 = 26, D5 = 27, E5 = 28, F5 = 29, G5 = 30, H5 = 31;
    static const int A6 = 16, B6 = 17, C6 = 18, D6 = 19, E6 = 20, F6 = 21, G6 = 22, H6 = 23;
    static const int A7 = 8, B7 = 9, C7 = 10, D7 = 11, E7 = 12, F7 = 13, G7 = 14, H7 = 15;
    static const int A8 = 0, B8 = 1, C8 = 2, D8 = 3, E8 = 4, F8 = 5, G8 = 6, H8 = 7;
};

struct SquareDistances {
    int top;
    int bottom;
    int left;
    int right;
    int topLeft;
    int topRight;
    int bottomLeft;
    int bottomRight;
};

struct PieceBitboards {
    Bitboard WhitePawns = 0ULL;
    Bitboard BlackPawns = 0ULL;
    Bitboard WhiteKnights = 0ULL;
    Bitboard BlackKnights = 0ULL;
    Bitboard WhiteBishops = 0ULL;
    Bitboard BlackBishops = 0ULL;
    Bitboard WhiteRooks = 0ULL;
    Bitboard BlackRooks = 0ULL;
    Bitboard WhiteQueens = 0ULL;
    Bitboard BlackQueens = 0ULL;
    Bitboard WhiteKing = 0ULL;
    Bitboard BlackKing = 0ULL;
};

struct Square {
    Rectangle bounds;
    Vector2 center;
};

struct ChessPiece {
    int type;
    Vector2 position;
    Vector2 midpoint;
};

static void iterateAllBitboards(PieceBitboards& bitboards,
    const std::function<void(Bitboard&)>& operation,
    int excludePiece = 0) {
    auto applyIfNotExcluded = [&](Bitboard& bb, int pieceType) {
        if (excludePiece != pieceType) {
            operation(bb);
        }
    };

    applyIfNotExcluded(bitboards.WhitePawns, Piece::WhitePawn);
    applyIfNotExcluded(bitboards.WhiteKnights, Piece::WhiteKnight);
    applyIfNotExcluded(bitboards.WhiteBishops, Piece::WhiteBishop);
    applyIfNotExcluded(bitboards.WhiteRooks, Piece::WhiteRook);
    applyIfNotExcluded(bitboards.WhiteQueens, Piece::WhiteQueen);
    applyIfNotExcluded(bitboards.WhiteKing, Piece::WhiteKing);
    applyIfNotExcluded(bitboards.BlackPawns, Piece::BlackPawn);
    applyIfNotExcluded(bitboards.BlackKnights, Piece::BlackKnight);
    applyIfNotExcluded(bitboards.BlackBishops, Piece::BlackBishop);
    applyIfNotExcluded(bitboards.BlackRooks, Piece::BlackRook);
    applyIfNotExcluded(bitboards.BlackQueens, Piece::BlackQueen);
    applyIfNotExcluded(bitboards.BlackKing, Piece::BlackKing);
}

