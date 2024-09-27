#pragma once

#include "CommonComponents.h"
#include "Bitboard.h"
#include "Pieces.h"
#include "GameState.h"
#include <unordered_map>
#include "raylib.h"
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int SQUARE_SIZE = SCREEN_WIDTH / BOARD_SIZE;

class ChessBoard {
public:
    ChessBoard();
    void InitializeBoard();
    void LoadPieceTextures();
    void UnloadPieceTextures();
    void DrawChessBoard();
    void DrawPieces(int selectedPieceIndex) const;
    void DrawLegalMoveHighlights(const std::vector<Move>& legalMoves);
    void UpdateChessPieces();
    void UpdateGameFlags(int selectedPieceIndex);


private:
    void SetBoardFromFEN(const std::string& fen);
    void UpdateBitboardsFromBoard();
    void InitializeBoardSquares();
    void InitializeChessPieces();
    int ConvertToBitboardIndex(int boardIndex) const;

    std::array<Square, TOTAL_SQUARES> m_boardSquares;
    std::array<ChessPiece, 32> m_chessPieces;
    PieceManager m_pieceManager;
};