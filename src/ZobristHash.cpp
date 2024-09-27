#include "ZobristHash.h"

ZobristHash::ZobristHash() {
    initializeRandomNumbers();
}

void ZobristHash::initializeRandomNumbers() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for (int i = 0; i < PIECE_TYPES; ++i)
        for (int j = 0; j < SQUARES; ++j)
            pieceHashes[i][j] = dis(gen);

    blackToMove = dis(gen);
    for (int i = 0; i < 4; ++i)
        castlingRights[i] = dis(gen);
    for (int i = 0; i < 8; ++i)
        enPassantFile[i] = dis(gen);
}

uint64_t ZobristHash::hash(const BoardState& board, bool isBlackToMove, const GameRuleFlags& flags) {
    uint64_t h = 0;

    // Hash pieces
    for (int sq = 0; sq < SQUARES; ++sq) {
        int piece = board[sq];
        if (piece != Piece::None) {
            int pieceIndex = (piece & 7) - 1 + ((piece & Piece::Black) ? 6 : 0);
            h ^= pieceHashes[pieceIndex][sq];
        }
    }

    // Hash side to move
    if (isBlackToMove)
        h ^= blackToMove;

    // Hash castling rights
    if (!flags.whiteKingHasMoved && !flags.h1RookHasMoved) h ^= castlingRights[0];
    if (!flags.whiteKingHasMoved && !flags.a1RookHasMoved) h ^= castlingRights[1];
    if (!flags.blackKingHasMoved && !flags.h8RookHasMoved) h ^= castlingRights[2];
    if (!flags.blackKingHasMoved && !flags.a8RookHasMoved) h ^= castlingRights[3];

    // Hash en passant
    if (flags.enPassantTargetSquare != -1) {
        int file = flags.enPassantTargetSquare % 8;
        h ^= enPassantFile[file];
    }

    return h;
}