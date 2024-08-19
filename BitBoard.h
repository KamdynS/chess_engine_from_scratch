#pragma once
#include <cstdint>

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
};