#pragma once
#include "raylib.h"
#include "CommonComponents.h"
#include <array>
#include <unordered_map>

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

extern Square boardSquares[TOTAL_SQUARES];
extern ChessPiece chessPieces[32];
extern std::unordered_map<int, Texture2D> pieceTextures;

// Function declarations
void InitializeBoard(BoardState& board);
void LoadPieceTextures();
void UnloadPieceTextures();
void DrawChessBoard();
void DrawPieces(const BoardState& board, int selectedPieceIndex);
void DrawLegalMoveHighlights(const std::vector<Move>& legalMoves);
void UpdateBoardState(BoardState& board, const Move& move, int pieceType);
void UpdateGameFlags(GameRuleFlags& gameFlags, int selectedPieceIndex);
void InitializeBoardSquares();
void InitializeChessPieces(const BoardState& board);
bool IsPieceWhite(int pieceType);