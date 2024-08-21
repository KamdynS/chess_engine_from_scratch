#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include "CommonComponents.h"
#include "BitBoard.h"


class Piece {
public:
    // Piece Types
    static const int None = 0;
    static const int Pawn = 1;
    static const int Knight = 2;
    static const int Bishop = 3;
    static const int Rook = 4;
    static const int Queen = 5;
    static const int King = 6;
    // Piece Colours
    static const int White = 8;
    static const int Black = 16;
    // Pieces
    static const int WhitePawn = Pawn | White;
    static const int WhiteKnight = Knight | White; // 10
    static const int WhiteBishop = Bishop | White;
    static const int WhiteRook = Rook | White;
    static const int WhiteQueen = Queen | White;
    static const int WhiteKing = King | White;
    static const int BlackPawn = Pawn | Black;
    static const int BlackKnight = Knight | Black;
    static const int BlackBishop = Bishop | Black;
    static const int BlackRook = Rook | Black;
    static const int BlackQueen = Queen | Black;
    static const int BlackKing = King | Black;
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
    Bitboard WhitePawns, WhiteKnights, WhiteBishops, WhiteRooks, WhiteQueens, WhiteKing;
    Bitboard BlackPawns, BlackKnights, BlackBishops, BlackRooks, BlackQueens, BlackKing;
};

// Standalone function to update bitboards
void UpdateBitboards(PieceBitboards& bitboards, const Move& move, int currentPiece) {
    // Helper function to get a reference to the appropriate Bitboard
    auto getBitboard = [&](int pieceType) -> Bitboard& {
        switch (pieceType) {
        case Piece::WhitePawn: return bitboards.WhitePawns;
        case Piece::WhiteKnight: return bitboards.WhiteKnights;
        case Piece::WhiteBishop: return bitboards.WhiteBishops;
        case Piece::WhiteRook: return bitboards.WhiteRooks;
        case Piece::WhiteQueen: return bitboards.WhiteQueens;
        case Piece::WhiteKing: return bitboards.WhiteKing;
        case Piece::BlackPawn: return bitboards.BlackPawns;
        case Piece::BlackKnight: return bitboards.BlackKnights;
        case Piece::BlackBishop: return bitboards.BlackBishops;
        case Piece::BlackRook: return bitboards.BlackRooks;
        case Piece::BlackQueen: return bitboards.BlackQueens;
        case Piece::BlackKing: return bitboards.BlackKing;
        default:
            throw std::invalid_argument("Invalid piece type in getBitboard");
        }
        };

    // Get the bitboard for the current piece
    Bitboard& currentBitboard = getBitboard(currentPiece);

    // Update the bitboard for the moving piece
    currentBitboard.clear(move.startSquare);
    currentBitboard.set(move.targetSquare);

    // Create a mask for clearing the captured piece (if any)
    Bitboard::BitboardType captureMask = ~(1ULL << move.targetSquare);

    // Clear the captured piece from all other bitboards
    for (int pieceType : {Piece::WhitePawn, Piece::WhiteKnight, Piece::WhiteBishop, Piece::WhiteRook, Piece::WhiteQueen, Piece::WhiteKing,
        Piece::BlackPawn, Piece::BlackKnight, Piece::BlackBishop, Piece::BlackRook, Piece::BlackQueen, Piece::BlackKing}) {
        if (pieceType != currentPiece) {
            getBitboard(pieceType).board &= captureMask;
        }
    }

    // Handle special moves
    if (move.isEnPassant) {
        int capturedPawnSquare = move.targetSquare + ((currentPiece & Piece::White) ? 8 : -8);
        getBitboard((currentPiece & Piece::White) ? Piece::BlackPawn : Piece::WhitePawn).clear(capturedPawnSquare);
    }
    else if (move.isCastling) {
        // Update rook position for castling
        Bitboard& rookBitboard = getBitboard((currentPiece & Piece::White) ? Piece::WhiteRook : Piece::BlackRook);
        rookBitboard.clear(move.rookStartSquare);
        rookBitboard.set(move.rookTargetSquare);
    }

    // Handle pawn promotion
    if (move.isPromotion) {
        currentBitboard.clear(move.targetSquare);  // Remove pawn from its bitboard
        getBitboard(move.promotionPiece).set(move.targetSquare);  // Add the promoted piece to its bitboard
    }
}

extern std::vector<Move> moveList;

extern const std::vector<std::pair<int, int>> DIRECTIONS;

// Function declarations
SquareDistances CalculateDistances(int squareIndex);

std::vector<Move> GenerateSlidingMoves(int indexOnBoard, int pieceType, const BoardState& board);
std::vector<Move> GenerateKnightMoves(int indexOnBoard, int pieceType, const BoardState& board);
std::vector<Move> GeneratePawnMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags);
std::vector<Move> GenerateKingMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags);
bool CanCastle(int kingType, bool kingSide, const BoardState& board, const GameRuleFlags& flags);
int FindKingLocation(int kingColor, const BoardState& board);