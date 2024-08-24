#include "raylib.h"
#include "Board.h"
#include "Pieces.h"
#include "Game.h"
#include "CommonComponents.h"
#include "ZobristHash.h"
#include <vector>
#include <iostream>

template <typename T>
T Clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Meta information
int moveCount{ 1 };
BoardState board = {};
GameRuleFlags gameFlags;
PieceBitboards gameBitboards;
std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
ZobristHash zobristHash;
std::vector<uint64_t> positionHistory;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess Engine with raylib");

    std::vector<Move> currentLegalMoves;
    InitializeBoard(board, gameBitboards, startingFEN);
    LoadPieceTextures();

    int selectedPieceIndex = -1;
    Vector2 dragOffset = { 0, 0 };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            int col = (int)(mousePos.x / SQUARE_SIZE);
            int row = (int)(mousePos.y / SQUARE_SIZE);
            int index = row * BOARD_SIZE + col;

            if (index >= 0 && index < TOTAL_SQUARES && board[index] != Piece::None) {
                selectedPieceIndex = index;
                dragOffset = { mousePos.x - (col * SQUARE_SIZE + SQUARE_SIZE / 2.0f),
                               mousePos.y - (row * SQUARE_SIZE + SQUARE_SIZE / 2.0f) };
                currentLegalMoves = GenerateLegalMoves(board[selectedPieceIndex], moveCount, selectedPieceIndex, board, gameFlags);
            }
        }

        DrawChessBoard();

        // Draw legal move highlights if a piece is selected
        if (selectedPieceIndex != -1) {
            DrawLegalMoveHighlights(currentLegalMoves);
        }

        DrawPieces(board, selectedPieceIndex);

        // Draw the dragged piece
        if (selectedPieceIndex != -1) {
            Vector2 mousePos = GetMousePosition();
            float scale = (float)SQUARE_SIZE / pieceTextures[board[selectedPieceIndex]].width * 0.8f;
            Vector2 centered = {
                mousePos.x - dragOffset.x - (pieceTextures[board[selectedPieceIndex]].width * scale) / 2,
                mousePos.y - dragOffset.y - (pieceTextures[board[selectedPieceIndex]].height * scale) / 2
            };
            DrawTextureEx(pieceTextures[board[selectedPieceIndex]], centered, 0.0f, scale, WHITE);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && selectedPieceIndex != -1) {
            Vector2 mousePos = GetMousePosition();
            int newCol = (int)(mousePos.x / SQUARE_SIZE);
            int newRow = (int)(mousePos.y / SQUARE_SIZE);

            newCol = Clamp(newCol, 0, BOARD_SIZE - 1);
            newRow = Clamp(newRow, 0, BOARD_SIZE - 1);

            int newIndex = newRow * BOARD_SIZE + newCol;

            std::cout << "Moving piece from " << selectedPieceIndex << " to " << newIndex << std::endl;
            std::cout << "Piece type: " << board[selectedPieceIndex] << std::endl;

            bool isLegalMove = false;
            Move selectedMove;
            for (const auto& move : currentLegalMoves) {
                if (move.targetSquare == newIndex) {
                    selectedMove = move;
                    isLegalMove = true;
                    break;
                }
            }

            if (newIndex != selectedPieceIndex && isLegalMove) {
                MakeMove(board, selectedMove, board[selectedPieceIndex], gameFlags);
                UpdateGameFlags(gameFlags, selectedPieceIndex);
                UpdateBitboards(gameBitboards, selectedMove, board[selectedPieceIndex]);
                uint64_t newHash = zobristHash.hash(board, moveCount % 2 == 0, gameFlags);
                positionHistory.push_back(newHash);
                moveCount++;
                std::cout << "Move count: " << moveCount << std::endl;
                BlackCheckmate(board[selectedPieceIndex], moveCount, selectedPieceIndex, board, gameFlags);
                WhiteCheckmate(board[selectedPieceIndex], moveCount, selectedPieceIndex, board, gameFlags);
            }
            else {
                // If the move is not legal, ensure the piece stays in its original position
                std::cout << "Illegal move. Piece remains at " << selectedPieceIndex << std::endl;
            }
           
            // Clear selection and legal moves
            selectedPieceIndex = -1;
            currentLegalMoves.clear();
        }

        EndDrawing();
    }

    UnloadPieceTextures();
    CloseWindow();

    return 0;
}