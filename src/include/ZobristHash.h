#pragma once

#include <array>
#include <random>
#include "CommonComponents.h"
#include "Pieces.h"

class ZobristHash {
public:
    ZobristHash();
    uint64_t hash(const BoardState& board, bool isBlackToMove, const GameRuleFlags& flags);

private:
    static const int PIECE_TYPES = 12; // 6 piece types for each color
    static const int SQUARES = 64;
    std::array<std::array<uint64_t, SQUARES>, PIECE_TYPES> pieceHashes;
    uint64_t blackToMove;
    std::array<uint64_t, 4> castlingRights; // KQkq
    std::array<uint64_t, 8> enPassantFile;

    void initializeRandomNumbers();
};