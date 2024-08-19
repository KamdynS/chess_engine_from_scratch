#pragma once
#include "CommonComponents.h"

struct MoveInfo {
    int capturedPiece;
    bool wasEnPassant;
    bool wasCastling;
    int oldEnPassantSquare;
    GameRuleFlags oldFlags;
};

std::vector<Move> GenerateLegalMoves(int currentPiece, int moveCount, int indexOnBoard, const BoardState& board, const GameRuleFlags& flags);
bool IsPieceWhite(int pieceType);
bool IsWhiteMove(int pieceType);
bool IsCorrectMove(int pieceType, int moveCount);
bool IsSquareAttackedSimple(int square, int attackingColor, const BoardState& board);
bool IsKingInCheck(const BoardState& board, int kingColor);