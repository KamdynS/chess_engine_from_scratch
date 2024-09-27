#include "include/ChessBoard.h"
#include "include/Pieces.h"
#include "include/Game.h"
#include "include/GameState.h"
#include "include/CommonComponents.h"
#include <iostream>
#include <cctype>

ChessBoard::ChessBoard() {
    InitializeBoard();
}

void ChessBoard::InitializeBoard() {
    auto& gameState = GameState::getInstance();
    const std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    SetBoardFromFEN(startingFEN);
    UpdateBitboardsFromBoard();
    InitializeBoardSquares();
    InitializeChessPieces();
}

void ChessBoard::SetBoardFromFEN(const std::string& fen) {
    auto& gameState = GameState::getInstance();
    std::unordered_map<char, int> fenToPiece = {
        {'p', Piece::BlackPawn}, {'n', Piece::BlackKnight}, {'b', Piece::BlackBishop},
        {'r', Piece::BlackRook}, {'q', Piece::BlackQueen}, {'k', Piece::BlackKing},
        {'P', Piece::WhitePawn}, {'N', Piece::WhiteKnight}, {'B', Piece::WhiteBishop},
        {'R', Piece::WhiteRook}, {'Q', Piece::WhiteQueen}, {'K', Piece::WhiteKing}
    };

    int index = 0;
    for (char c : fen.substr(0, fen.find(' '))) {
        if (isdigit(c)) {
            index += c - '0';
        } else if (c != '/') {
            gameState.board[index++] = fenToPiece[c];
        }
    }
}

int ChessBoard::ConvertToBitboardIndex(int boardIndex) const {
    int rank = boardIndex / 8;  // Get the rank (row) of the piece
    int file = boardIndex % 8;  // Get the file (column) of the piece
    return file + (7 - rank) * 8;  // Flip the rank and recalculate the index
}

void ChessBoard::UpdateBitboardsFromBoard() {
    auto& gameState = GameState::getInstance();
    m_pieceManager.ClearAllBitboards();

    // Create a mapping of piece types to bitboard pointers
    std::unordered_map<int, Bitboard*> pieceToBitboard {
        {Piece::WhitePawn, &gameState.bitboards.WhitePawns},
        {Piece::WhiteKnight, &gameState.bitboards.WhiteKnights},
        {Piece::WhiteBishop, &gameState.bitboards.WhiteBishops},
        {Piece::WhiteRook, &gameState.bitboards.WhiteRooks},
        {Piece::WhiteQueen, &gameState.bitboards.WhiteQueens},
        {Piece::WhiteKing, &gameState.bitboards.WhiteKing},
        {Piece::BlackPawn, &gameState.bitboards.BlackPawns},
        {Piece::BlackKnight, &gameState.bitboards.BlackKnights},
        {Piece::BlackBishop, &gameState.bitboards.BlackBishops},
        {Piece::BlackRook, &gameState.bitboards.BlackRooks},
        {Piece::BlackQueen, &gameState.bitboards.BlackQueens},
        {Piece::BlackKing, &gameState.bitboards.BlackKing}
    };

    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        int piece = gameState.board[i];
        if (piece != Piece::None) {
            int bitboardIndex = ConvertToBitboardIndex(i);

            auto it = pieceToBitboard.find(piece);
            if (it != pieceToBitboard.end()) {
                it->second->set(bitboardIndex);
            }
        }
    }
}

void ChessBoard::LoadPieceTextures() {
    auto& gameState = GameState::getInstance();
    gameState.pieceTextures[Piece::BlackRook] = LoadTexture("resources/black_rook.png");
    gameState.pieceTextures[Piece::BlackPawn] = LoadTexture("resources/black_pawn.png");
    gameState.pieceTextures[Piece::BlackKnight] = LoadTexture("resources/black_knight.png");
    gameState.pieceTextures[Piece::BlackBishop] = LoadTexture("resources/black_bishop.png");
    gameState.pieceTextures[Piece::BlackQueen] = LoadTexture("resources/black_queen.png");
    gameState.pieceTextures[Piece::BlackKing] = LoadTexture("resources/black_king.png");
    gameState.pieceTextures[Piece::WhitePawn] = LoadTexture("resources/white_pawn.png");
    gameState.pieceTextures[Piece::WhiteRook] = LoadTexture("resources/white_rook.png");
    gameState.pieceTextures[Piece::WhiteKnight] = LoadTexture("resources/white_knight.png");
    gameState.pieceTextures[Piece::WhiteBishop] = LoadTexture("resources/white_bishop.png");
    gameState.pieceTextures[Piece::WhiteQueen] = LoadTexture("resources/white_queen.png");
    gameState.pieceTextures[Piece::WhiteKing] = LoadTexture("resources/white_king.png");
}

void ChessBoard::UnloadPieceTextures() {
    auto& gameState = GameState::getInstance();
    for (auto& texture : gameState.pieceTextures) {
        UnloadTexture(texture.second);
    }
}

void ChessBoard::DrawChessBoard() {
    Color lightSquares = { 255, 228, 196, 255 }; // Light color
    Color darkSquares = { 205, 133, 63, 255 };   // Dark color

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            Color squareColor = ((row + col) % 2 == 0) ? lightSquares : darkSquares;
            DrawRectangle(col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, squareColor);
        }
    }
}

void ChessBoard::DrawPieces(int selectedPieceIndex) const {
    auto& gameState = GameState::getInstance();
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (gameState.board[i] != Piece::None && i != selectedPieceIndex) {
            int row = i / BOARD_SIZE;
            int col = i % BOARD_SIZE;
            Vector2 position = {
                (float)(col * SQUARE_SIZE),
                (float)(row * SQUARE_SIZE)
            };
            float scale = (float)SQUARE_SIZE / gameState.pieceTextures.at(gameState.board[i]).width * 0.8f;
            Vector2 centered = {
                position.x + (SQUARE_SIZE - gameState.pieceTextures.at(gameState.board[i]).width * scale) / 2,
                position.y + (SQUARE_SIZE - gameState.pieceTextures.at(gameState.board[i]).height * scale) / 2
            };
            DrawTextureEx(gameState.pieceTextures.at(gameState.board[i]), centered, 0.0f, scale, WHITE);
        }
    }
}


void ChessBoard::UpdateChessPieces() {
    auto& gameState = GameState::getInstance();
    int pieceCount = 0;
    for (int i = 0; i < TOTAL_SQUARES; i++) {
        if (gameState.board[i] != Piece::None) {
            m_chessPieces[pieceCount].type = gameState.board[i];
            m_chessPieces[pieceCount].position = Vector2{
                (float)((i % BOARD_SIZE) * SQUARE_SIZE),
                (float)((i / BOARD_SIZE) * SQUARE_SIZE)
            };
            m_chessPieces[pieceCount].midpoint = Vector2{
                m_chessPieces[pieceCount].position.x + SQUARE_SIZE / 2.0f,
                m_chessPieces[pieceCount].position.y + SQUARE_SIZE / 2.0f
            };
            pieceCount++;
        }
    }
    // Clear any remaining pieces
    for (; pieceCount < 32; pieceCount++) {
        m_chessPieces[pieceCount].type = Piece::None;
    }
}

void ChessBoard::UpdateGameFlags(int selectedPieceIndex) {
    auto& gameState = GameState::getInstance();
    if (selectedPieceIndex == 0 && !gameState.gameFlags.a8RookHasMoved) {
        gameState.gameFlags.a8RookHasMoved = true;
    }
    else if (selectedPieceIndex == 4 && !gameState.gameFlags.blackKingHasMoved) {
        gameState.gameFlags.blackKingHasMoved = true;
    }
    else if (selectedPieceIndex == 7 && !gameState.gameFlags.h8RookHasMoved) {
        gameState.gameFlags.h8RookHasMoved = true;
    }
    else if (selectedPieceIndex == 63 && !gameState.gameFlags.h1RookHasMoved) {
        gameState.gameFlags.h1RookHasMoved = true;
    }
    else if (selectedPieceIndex == 60 && !gameState.gameFlags.whiteKingHasMoved) {
        gameState.gameFlags.whiteKingHasMoved = true;
    }
    else if (selectedPieceIndex == 56 && !gameState.gameFlags.a1RookHasMoved) {
        gameState.gameFlags.a1RookHasMoved = true;
    }
}

void ChessBoard::InitializeBoardSquares() {
    for (int i = 0; i < TOTAL_SQUARES; i++) {
        int x = (i % BOARD_SIZE) * SQUARE_SIZE;
        int y = (i / BOARD_SIZE) * SQUARE_SIZE;
        m_boardSquares[i].bounds = Rectangle{ (float)x, (float)y, (float)SQUARE_SIZE, (float)SQUARE_SIZE };
        m_boardSquares[i].center = Vector2{ x + SQUARE_SIZE / 2.0f, y + SQUARE_SIZE / 2.0f };
    }
}

void ChessBoard::InitializeChessPieces() {
    auto& gameState = GameState::getInstance();
    int pieceCount = 0;
    for (int i = 0; i < TOTAL_SQUARES; i++) {
        if (gameState.board[i] != Piece::None) {
            m_chessPieces[pieceCount].type = gameState.board[i];
            m_chessPieces[pieceCount].position = Vector2{
                (float)((i % BOARD_SIZE) * SQUARE_SIZE),
                (float)((i / BOARD_SIZE) * SQUARE_SIZE)
            };
            m_chessPieces[pieceCount].midpoint = Vector2{
                m_chessPieces[pieceCount].position.x + SQUARE_SIZE / 2.0f,
                m_chessPieces[pieceCount].position.y + SQUARE_SIZE / 2.0f
            };
            pieceCount++;
        }
    }
}

void ChessBoard::DrawLegalMoveHighlights(const std::vector<Move>& legalMoves) {
    Color highlightColor = { 0, 255, 0, 100 }; // Semi-transparent green
    for (const auto& move : legalMoves) {
        int row = move.targetSquare / BOARD_SIZE;
        int col = move.targetSquare % BOARD_SIZE;
        DrawRectangle(col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, highlightColor);
    }
}