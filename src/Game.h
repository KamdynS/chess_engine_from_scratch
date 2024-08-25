#pragma once
#include "CommonComponents.h"
#include "ChessBoard.h"
#include "Pieces.h"
#include <vector>

class Game {
public:
    Game();

    std::vector<Move> GenerateLegalMoves(int currentPiece, int indexOnBoard) const;
    std::vector<Move> FilterLegalMoves(const std::vector<Move>& moves, int currentPlayer) const;
    bool IsPieceWhite(int pieceType) const;
    bool IsWhiteMove() const;
    bool IsCorrectMove(int pieceType) const;
    bool IsSquareAttackedSimple(int square, int attackingColor) const;
    bool IsKingInCheck(int kingColor) const;
    bool BlackCheckmate() const;
    bool WhiteCheckmate() const;
    bool GameDrawStaleMate() const;
    bool GameDrawInsufficientMaterial() const;
    bool GameDrawFiftyMove() const;
    bool GameDrawThreefold() const;
    void MakeMove(Move& move, int currentPiece);

private:
    PieceManager m_pieceManager;
};