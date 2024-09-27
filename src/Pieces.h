#pragma once
#include "CommonComponents.h"
#include "Bitboard.h"
#include "GameState.h"
#include <vector>

class Game;  // Forward declaration

class PieceManager {
public:
    PieceManager();

    std::vector<Move> GenerateSlidingMoves(int indexOnBoard, int pieceType) const;
    std::vector<Move> GenerateKnightMoves(int indexOnBoard, int pieceType) const;
    std::vector<Move> GeneratePawnMoves(int indexOnBoard, int pieceType) const;
    std::vector<Move> GenerateKingMoves(int indexOnBoard, int pieceType, const Game& game) const;
    bool CanCastle(int kingType, bool kingSide, const Game& game) const;
    int FindKingLocation(int kingColor) const;
    void UpdateBitboards(const Move& move, int currentPiece);
    static void ClearAllBitboards();

private:
    std::vector<std::pair<int, int>> m_directions;
};

// Piece definitions
class Piece {
public:
    static const int None = 0;
    static const int Pawn = 1;
    static const int Knight = 2;
    static const int Bishop = 3;
    static const int Rook = 4;
    static const int Queen = 5;
    static const int King = 6;
    static const int White = 8;
    static const int Black = 16;
    static const int WhitePawn = Piece::Pawn | Piece::White;
    static const int WhiteKnight = Piece::Knight | Piece::White;
    static const int WhiteBishop = Piece::Bishop | Piece::White;
    static const int WhiteRook = Piece::Rook | Piece::White;
    static const int WhiteQueen = Piece::Queen | Piece::White;
    static const int WhiteKing = Piece::King | Piece::White;
    static const int BlackPawn = Piece::Pawn | Piece::Black;
    static const int BlackKnight = Piece::Knight | Piece::Black;
    static const int BlackBishop = Piece::Bishop | Piece::Black;
    static const int BlackRook = Piece::Rook | Piece::Black;
    static const int BlackQueen = Piece::Queen | Piece::Black;
    static const int BlackKing = Piece::King | Piece::Black;
};

template <typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}