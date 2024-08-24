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
    bool BlackCheckmate();
    bool WhiteCheckmate();
    bool GameDrawStaleMate();
    bool GameDrawInsufficientMaterial() const;
    bool GameDrawFiftyMove() const;
    bool GameDrawThreefold() const;
    void IncrementMoveCount() { m_moveCount++; };

    const ChessBoard& GetBoard() const { return m_board; }
    const GameRuleFlags& GetFlags() const { return m_flags; }
    int GetMoveCount() const { return m_moveCount; }

private:
    ChessBoard m_board;
    GameRuleFlags m_flags;
    int m_moveCount;
    std::vector<uint64_t> m_positionHistory;
    PieceManager m_pieceManager;
};