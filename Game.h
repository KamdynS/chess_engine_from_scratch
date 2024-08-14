#pragma once
#include "CommonComponents.h"


std::vector<Move> GenerateLegalMoves(int currentPiece, int moveCount, int indexOnBoard, const BoardState& board, const GameRuleFlags& flags);
bool IsPieceWhite(int pieceType);
bool IsWhiteMove(int pieceType);
bool IsCorrectMove(int pieceType, int moveCount);
std::vector<int> PiecesAttackingCurrentSquare(int indexOfSquare, const BoardState& board, const GameRuleFlags&);
bool IsSquareAttacked(int indexOfSquare, const BoardState& board, const GameRuleFlags&);