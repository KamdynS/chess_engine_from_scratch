#pragma once
#include "raylib.h"
#include "CommonComponents.h"
#include <array>
#include <unordered_map>
#include <string>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int SQUARE_SIZE = SCREEN_WIDTH / BOARD_SIZE;

struct Square {
    Rectangle bounds;
    Vector2 center;
};

struct ChessPiece {
    int type;
    Vector2 position;
    Vector2 midpoint;
};

class ChessBoard {
public:
    ChessBoard();
    void InitializeBoard(const std::string& fen = "");
    void LoadPieceTextures();
    const std::unordered_map<int, Texture2D>& GetPieceTextures() const { return m_pieceTextures; }
    void UnloadPieceTextures();
    void DrawChessBoard();
    void DrawPieces(int selectedPieceIndex) const;
    void DrawLegalMoveHighlights(const std::vector<Move>& legalMoves);
    void MakeMove(Move& move, int currentPiece);
    void UpdateGameFlags(int selectedPieceIndex);
    int ConvertToBitboardIndex(int boardIndex) const;
    bool IsPieceWhite(int pieceType) const;

    BoardState& GetBoardState() { return m_board; }
    GameRuleFlags& GetGameFlags() { return m_gameFlags; }
    PieceBitboards& GetBitboards() { return m_bitboards; }

private:
    void InitializeBoardFromFEN(const std::string& fen);
    void UpdateBitboardsFromBoard();
    void InitializeBoardSquares();
    void InitializeChessPieces();

    BoardState m_board;
    PieceBitboards m_bitboards;
    GameRuleFlags m_gameFlags;
    Square m_boardSquares[TOTAL_SQUARES];
    ChessPiece m_chessPieces[32];
    std::unordered_map<int, Texture2D> m_pieceTextures;
};