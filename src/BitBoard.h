#pragma once
#include <cstdint>
#include <functional>
#include "Pieces.h"

// Forward declaration of PieceBitboards
struct PieceBitboards;

class Bitboard {
public:
    using BitboardType = uint64_t;

    // Constants for piece types and colors
    static constexpr BitboardType EMPTY = 0ULL;
    static constexpr BitboardType UNIVERSE = ~0ULL;

    // Constructors
    BitboardType board;

    Bitboard() : board(EMPTY) {}
    explicit Bitboard(BitboardType value) : board(value) {}

    // Basic operations
    void set(int square);
    void clear(int square);
    bool isOccupied(int square) const;

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
};