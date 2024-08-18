#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include "CommonComponents.h"


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


extern std::vector<Move> moveList;

extern const std::vector<std::pair<int, int>> DIRECTIONS;

// Function declarations
SquareDistances CalculateDistances(int squareIndex);

std::vector<Move> GenerateSlidingMoves(int indexOnBoard, int pieceType, const BoardState& board);
std::vector<Move> GenerateKnightMoves(int indexOnBoard, int pieceType, const BoardState& board);
std::vector<Move> GeneratePawnMoves(int indexOnBoard, int pieceType, const BoardState& board);
std::vector<Move> GenerateKingMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags);
bool CanCastle(int kingType, bool kingSide, const BoardState& board, const GameRuleFlags& flags);
int FindKingLocation(int kingColor, const BoardState& board);