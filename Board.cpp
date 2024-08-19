#include "Board.h"
#include "Pieces.h"
#include "CommonComponents.h"
#include <iostream>

Square boardSquares[TOTAL_SQUARES];
ChessPiece chessPieces[32];
std::unordered_map<int, Texture2D> pieceTextures;

void InitializeBoard(BoardState& board) {
    const int initialBoard[TOTAL_SQUARES] = {
        Piece::BlackRook, Piece::BlackKnight, Piece::BlackBishop, Piece::BlackQueen, Piece::BlackKing, Piece::BlackBishop, Piece::BlackKnight, Piece::BlackRook,
        Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn,
        Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None,
        Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None,
        Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None,
        Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None, Piece::None,
        Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn,
        Piece::WhiteRook, Piece::WhiteKnight, Piece::WhiteBishop, Piece::WhiteQueen, Piece::WhiteKing, Piece::WhiteBishop, Piece::WhiteKnight, Piece::WhiteRook
    };

    for (int i = 0; i < TOTAL_SQUARES; i++) {
        board[i] = initialBoard[i];
    }

    InitializeBoardSquares();
    InitializeChessPieces(board);
}

void LoadPieceTextures() {
    pieceTextures[Piece::BlackPawn] = LoadTexture("resources/black_pawn.png");
    pieceTextures[Piece::BlackRook] = LoadTexture("resources/black_rook.png");
    pieceTextures[Piece::BlackKnight] = LoadTexture("resources/black_knight.png");
    pieceTextures[Piece::BlackBishop] = LoadTexture("resources/black_bishop.png");
    pieceTextures[Piece::BlackQueen] = LoadTexture("resources/black_queen.png");
    pieceTextures[Piece::BlackKing] = LoadTexture("resources/black_king.png");
    pieceTextures[Piece::WhitePawn] = LoadTexture("resources/white_pawn.png");
    pieceTextures[Piece::WhiteRook] = LoadTexture("resources/white_rook.png");
    pieceTextures[Piece::WhiteKnight] = LoadTexture("resources/white_knight.png");
    pieceTextures[Piece::WhiteBishop] = LoadTexture("resources/white_bishop.png");
    pieceTextures[Piece::WhiteQueen] = LoadTexture("resources/white_queen.png");
    pieceTextures[Piece::WhiteKing] = LoadTexture("resources/white_king.png");
}

void UnloadPieceTextures() {
    for (auto& texture : pieceTextures) {
        UnloadTexture(texture.second);
    }
}

void DrawChessBoard() {
    Color lightSquares = { 255, 228, 196, 255 }; // Light color
    Color darkSquares = { 205, 133, 63, 255 };   // Dark color

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            Color squareColor = ((row + col) % 2 == 0) ? lightSquares : darkSquares;
            DrawRectangle(col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, squareColor);
        }
    }
}

void DrawPieces(const BoardState& board, int selectedPieceIndex) {
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (board[i] != Piece::None && i != selectedPieceIndex) {
            int row = i / BOARD_SIZE;
            int col = i % BOARD_SIZE;
            Vector2 position = {
                (float)(col * SQUARE_SIZE),
                (float)(row * SQUARE_SIZE)
            };
            float scale = (float)SQUARE_SIZE / pieceTextures[board[i]].width * 0.8f;
            Vector2 centered = {
                position.x + (SQUARE_SIZE - pieceTextures[board[i]].width * scale) / 2,
                position.y + (SQUARE_SIZE - pieceTextures[board[i]].height * scale) / 2
            };
            DrawTextureEx(pieceTextures[board[i]], centered, 0.0f, scale, WHITE);
        }
    }
}

void UpdateBoardState(BoardState& board, const Move& move, int pieceType, GameRuleFlags& flags) {
    if (move.startSquare != move.targetSquare) {  // Only update if the piece actually moved
        board[move.startSquare] = Piece::None;
        board[move.targetSquare] = pieceType;

        // Update chessPieces array
        for (int i = 0; i < 32; i++) {
            if (chessPieces[i].position.x == (move.startSquare % BOARD_SIZE) * SQUARE_SIZE &&
                chessPieces[i].position.y == (move.startSquare / BOARD_SIZE) * SQUARE_SIZE) {
                chessPieces[i].position.x = (move.targetSquare % BOARD_SIZE) * SQUARE_SIZE;
                chessPieces[i].position.y = (move.targetSquare / BOARD_SIZE) * SQUARE_SIZE;
                chessPieces[i].midpoint.x = chessPieces[i].position.x + SQUARE_SIZE / 2.0f;
                chessPieces[i].midpoint.y = chessPieces[i].position.y + SQUARE_SIZE / 2.0f;
                chessPieces[i].type = pieceType;
                break;
            }
        }

        std::cout << "Moved piece from " << move.startSquare << " to " << move.targetSquare << std::endl;
        for (int i = 0; i < TOTAL_SQUARES; i++) {
            std::cout << board[i] << " ";
            if ((i + 1) % BOARD_SIZE == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    if (move.isCastling) {
        // Move the rook
        board[move.rookStartSquare] = Piece::None;
        board[move.rookTargetSquare] = (pieceType & Piece::White) ? Piece::WhiteRook : Piece::BlackRook;

        // Update chessPieces array for the rook
        for (int i = 0; i < 32; i++) {
            if (chessPieces[i].position.x == (move.rookStartSquare % BOARD_SIZE) * SQUARE_SIZE &&
                chessPieces[i].position.y == (move.rookStartSquare / BOARD_SIZE) * SQUARE_SIZE) {
                chessPieces[i].position.x = (move.rookTargetSquare % BOARD_SIZE) * SQUARE_SIZE;
                chessPieces[i].position.y = (move.rookTargetSquare / BOARD_SIZE) * SQUARE_SIZE;
                chessPieces[i].midpoint.x = chessPieces[i].position.x + SQUARE_SIZE / 2.0f;
                chessPieces[i].midpoint.y = chessPieces[i].position.y + SQUARE_SIZE / 2.0f;
                break;
            }
        }

        std::cout << "Castling: Moved rook from " << move.rookStartSquare << " to " << move.rookTargetSquare << std::endl;
    }
    if ((pieceType & Piece::Pawn) && abs(move.startSquare - move.targetSquare) == 16) {
        flags.enPassantTargetSquare = (move.startSquare + move.targetSquare) / 2;
    }
    else {
        flags.enPassantTargetSquare = -1;
    }

    if (move.isEnPassant) {
        int capturedPawnSquare = move.targetSquare + (IsPieceWhite(pieceType) ? 8 : -8);
        board[capturedPawnSquare] = Piece::None;

        // Remove the captured pawn from chessPieces array
        for (int i = 0; i < 32; i++) {
            if (chessPieces[i].position.x == (capturedPawnSquare % BOARD_SIZE) * SQUARE_SIZE &&
                chessPieces[i].position.y == (capturedPawnSquare / BOARD_SIZE) * SQUARE_SIZE) {
                chessPieces[i].type = Piece::None;
                break;
            }
        }

        std::cout << "En passant capture: Removed pawn at " << capturedPawnSquare << std::endl;
    }
}

void UpdateGameFlags(GameRuleFlags& gameFlags, int selectedPieceIndex) {
    if (selectedPieceIndex == 0 && !gameFlags.a8RookHasMoved) {
        gameFlags.a8RookHasMoved = true;
    }
    else if (selectedPieceIndex == 4 && !gameFlags.blackKingHasMoved) {
        gameFlags.blackKingHasMoved = true;
    }
    else if (selectedPieceIndex == 7 && !gameFlags.h8RookHasMoved) {
        gameFlags.h8RookHasMoved = true;
    }
    else if (selectedPieceIndex == 63 && !gameFlags.h1RookHasMoved) {
        gameFlags.h1RookHasMoved = true;
    }
    else if (selectedPieceIndex == 60 && !gameFlags.whiteKingHasMoved) {
        gameFlags.whiteKingHasMoved = true;
    }
    else if (selectedPieceIndex == 56 && !gameFlags.a1RookHasMoved) {
        gameFlags.a1RookHasMoved = true;
    }
}


void InitializeBoardSquares() {
    for (int i = 0; i < TOTAL_SQUARES; i++) {
        int x = (i % BOARD_SIZE) * SQUARE_SIZE;
        int y = (i / BOARD_SIZE) * SQUARE_SIZE;
        boardSquares[i].bounds = Rectangle{ (float)x, (float)y, (float)SQUARE_SIZE, (float)SQUARE_SIZE };
        boardSquares[i].center = Vector2{ x + SQUARE_SIZE / 2.0f, y + SQUARE_SIZE / 2.0f };
    }
}

void InitializeChessPieces(const BoardState& board) {
    int pieceCount = 0;
    for (int i = 0; i < TOTAL_SQUARES; i++) {
        if (board[i] != Piece::None) {
            chessPieces[pieceCount].type = board[i];
            chessPieces[pieceCount].position = Vector2{
                (float)((i % BOARD_SIZE) * SQUARE_SIZE),
                (float)((i / BOARD_SIZE) * SQUARE_SIZE)
            };
            chessPieces[pieceCount].midpoint = Vector2{
                chessPieces[pieceCount].position.x + SQUARE_SIZE / 2.0f,
                chessPieces[pieceCount].position.y + SQUARE_SIZE / 2.0f
            };
            pieceCount++;
        }
    }
}

void DrawLegalMoveHighlights(const std::vector<Move>& legalMoves) {
    Color highlightColor = { 0, 255, 0, 100 }; // Semi-transparent green
    for (const auto& move : legalMoves) {
        int row = move.targetSquare / BOARD_SIZE;
        int col = move.targetSquare % BOARD_SIZE;
        DrawRectangle(col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, highlightColor);
    }
}