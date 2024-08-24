#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include "CommonComponents.h"
#include "BitBoard.h"
#include "Game.h"

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
    static const int WhiteKnight = Knight | White;
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

class PieceManager {
public:
    PieceManager();

    SquareDistances CalculateDistances(int squareIndex) const;
    std::vector<Move> GenerateSlidingMoves(int indexOnBoard, int pieceType, const BoardState& board) const;
    std::vector<Move> GenerateKnightMoves(int indexOnBoard, int pieceType, const BoardState& board) const;
    std::vector<Move> GeneratePawnMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags) const;
    std::vector<Move> GenerateKingMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags, const Game& game) const;
    bool CanCastle(int kingType, bool kingSide, const BoardState& board, const GameRuleFlags& flags, const Game& game) const;
    int FindKingLocation(int kingColor, const BoardState& board) const;
    void UpdateBitboards(PieceBitboards& bitboards, const Move& move, int currentPiece);
    void ClearAllBitboards(PieceBitboards& bitboards);

private:
    std::vector<std::pair<int, int>> m_directions;
    std::vector<Move> m_moveList;
};

template <typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}