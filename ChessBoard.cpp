#include "ChessBoard.h"
#include "Pieces.h"
#include <iostream>
#include <cctype>

ChessBoard::ChessBoard() {
    InitializeBoard();
}

void ChessBoard::InitializeBoard(const std::string& fen) {
    if (fen.empty()) {
        // Use default board setup
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
            m_board[i] = initialBoard[i];
        }
    }
    else {
        InitializeBoardFromFEN(fen);
    }

    UpdateBitboardsFromBoard();
    InitializeBoardSquares();
    InitializeChessPieces();
}

void ChessBoard::InitializeBoardFromFEN(const std::string& fen) {
    for (int i = 0; i < TOTAL_SQUARES; i++) {
        m_board[i] = Piece::None;
    } // Start with blank board

    std::unordered_map<char, int> fenToPiece = {
    {'p', Piece::BlackPawn},
    {'n', Piece::BlackKnight},
    {'b', Piece::BlackBishop},
    {'r', Piece::BlackRook},
    {'q', Piece::BlackQueen},
    {'k', Piece::BlackKing},
    {'P', Piece::WhitePawn},
    {'N', Piece::WhiteKnight},
    {'B', Piece::WhiteBishop},
    {'R', Piece::WhiteRook},
    {'Q', Piece::WhiteQueen},
    {'K', Piece::WhiteKing}
    };

    int indexCount = 0;

    size_t firstSpace = fen.find(' ');
    std::string fenPosition = fen.substr(0, firstSpace);
    for (char c: fenPosition) {
        if (std::isdigit(c)) {
            int emptySquares = c - '0';  // Convert char to int
            indexCount += emptySquares;  // Skip this many squares
        }
        else if (std::isalpha(c)) {
            m_board[indexCount] = fenToPiece[c];
            indexCount++;
        }
        else if (c == '/') {
            indexCount += 8;
        }
        else {
            std::cout << "FEN has unidentified character.";
        }
    }
}

int ChessBoard::ConvertToBitboardIndex(int boardIndex) const {
    int rank = boardIndex / 8;  // Get the rank (row) of the piece
    int file = boardIndex % 8;  // Get the file (column) of the piece
    return file + (7 - rank) * 8;  // Flip the rank and recalculate the index
}

void ChessBoard::UpdateBitboardsFromBoard() {
    PieceManager::ClearAllBitboards(m_bitboards);

    // Create a mapping of piece types to bitboard pointers
    std::unordered_map<int, Bitboard*> pieceToBitboard = {
    {Piece::WhitePawn, &m_bitboards.WhitePawns},
    {Piece::WhiteKnight, &m_bitboards.WhiteKnights},
    {Piece::WhiteBishop, &m_bitboards.WhiteBishops},
    {Piece::WhiteRook, &m_bitboards.WhiteRooks},
    {Piece::WhiteQueen, &m_bitboards.WhiteQueens},
    {Piece::WhiteKing, &m_bitboards.WhiteKing},
    {Piece::BlackPawn, &m_bitboards.BlackPawns},
    {Piece::BlackKnight, &m_bitboards.BlackKnights},
    {Piece::BlackBishop, &m_bitboards.BlackBishops},
    {Piece::BlackRook, &m_bitboards.BlackRooks},
    {Piece::BlackQueen, &m_bitboards.BlackQueens},
    {Piece::BlackKing, &m_bitboards.BlackKing}
    };

    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        int piece = m_board[i];
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
    m_pieceTextures[Piece::BlackRook] = LoadTexture("resources/black_rook.png");
    m_pieceTextures[Piece::BlackPawn] = LoadTexture("resources/black_pawn.png");
    m_pieceTextures[Piece::BlackKnight] = LoadTexture("resources/black_knight.png");
    m_pieceTextures[Piece::BlackBishop] = LoadTexture("resources/black_bishop.png");
    m_pieceTextures[Piece::BlackQueen] = LoadTexture("resources/black_queen.png");
    m_pieceTextures[Piece::BlackKing] = LoadTexture("resources/black_king.png");
    m_pieceTextures[Piece::WhitePawn] = LoadTexture("resources/white_pawn.png");
    m_pieceTextures[Piece::WhiteRook] = LoadTexture("resources/white_rook.png");
    m_pieceTextures[Piece::WhiteKnight] = LoadTexture("resources/white_knight.png");
    m_pieceTextures[Piece::WhiteBishop] = LoadTexture("resources/white_bishop.png");
    m_pieceTextures[Piece::WhiteQueen] = LoadTexture("resources/white_queen.png");
    m_pieceTextures[Piece::WhiteKing] = LoadTexture("resources/white_king.png");
}

void ChessBoard::UnloadPieceTextures() {
    for (auto& texture : m_pieceTextures) {
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
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (m_board[i] != Piece::None && i != selectedPieceIndex) {
            int row = i / BOARD_SIZE;
            int col = i % BOARD_SIZE;
            Vector2 position = {
                (float)(col * SQUARE_SIZE),
                (float)(row * SQUARE_SIZE)
            };
            float scale = (float)SQUARE_SIZE / m_pieceTextures[m_board[i]].width * 0.8f;
            Vector2 centered = {
                position.x + (SQUARE_SIZE - m_pieceTextures[m_board[i]].width * scale) / 2,
                position.y + (SQUARE_SIZE - m_pieceTextures[m_board[i]].height * scale) / 2
            };
            DrawTextureEx(m_pieceTextures[m_board[i]], centered, 0.0f, scale, WHITE);
        }
    }
}

void ChessBoard::MakeMove(Move& move, int currentPiece) {
    if (move.startSquare != move.targetSquare) {  // Only update if the piece actually moved
        bool isPawnMove = (currentPiece & 7) == Piece::Pawn;
        bool isCapture = m_board[move.targetSquare] != Piece::None || move.isEnPassant;

        if (isPawnMove || isCapture) {
            m_gameFlags.halfMoveClock = 0;
        }
        else {
            m_gameFlags.halfMoveClock++;
        }

        m_board[move.startSquare] = Piece::None;

        if (move.isPromotion) {
            // Promote to Queen (you can expand this later for other pieces)
            m_board[move.targetSquare] = (currentPiece & Piece::White) ? Piece::WhiteQueen : Piece::BlackQueen;
            move.promotionPiece = (currentPiece & Piece::White) ? Piece::WhiteQueen : Piece::BlackQueen;
        }
        else {
            m_board[move.targetSquare] = currentPiece;
            std::cout << "did not promote";
        }

        // Update chessPieces array
        for (int i = 0; i < 32; i++) {
            if (m_chessPieces[i].position.x == (move.startSquare % BOARD_SIZE) * SQUARE_SIZE &&
                m_chessPieces[i].position.y == (move.startSquare / BOARD_SIZE) * SQUARE_SIZE) {
                m_chessPieces[i].position.x = (move.targetSquare % BOARD_SIZE) * SQUARE_SIZE;
                m_chessPieces[i].position.y = (move.targetSquare / BOARD_SIZE) * SQUARE_SIZE;
                m_chessPieces[i].midpoint.x = m_chessPieces[i].position.x + SQUARE_SIZE / 2.0f;
                m_chessPieces[i].midpoint.y = m_chessPieces[i].position.y + SQUARE_SIZE / 2.0f;
                m_chessPieces[i].type = currentPiece;
                break;
            }
        }

        std::cout << "Moved piece from " << move.startSquare << " to " << move.targetSquare << std::endl;
        for (int i = 0; i < TOTAL_SQUARES; i++) {
            std::cout << m_board[i] << " ";
            if ((i + 1) % BOARD_SIZE == 0) std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    if (move.isCastling) {
        // Move the rook
        m_board[move.rookStartSquare] = Piece::None;
        m_board[move.rookTargetSquare] = (currentPiece & Piece::White) ? Piece::WhiteRook : Piece::BlackRook;

        // Update chessPieces array for the rook
        for (int i = 0; i < 32; i++) {
            if (m_chessPieces[i].position.x == (move.rookStartSquare % BOARD_SIZE) * SQUARE_SIZE &&
                m_chessPieces[i].position.y == (move.rookStartSquare / BOARD_SIZE) * SQUARE_SIZE) {
                m_chessPieces[i].position.x = (move.rookTargetSquare % BOARD_SIZE) * SQUARE_SIZE;
                m_chessPieces[i].position.y = (move.rookTargetSquare / BOARD_SIZE) * SQUARE_SIZE;
                m_chessPieces[i].midpoint.x = m_chessPieces[i].position.x + SQUARE_SIZE / 2.0f;
                m_chessPieces[i].midpoint.y = m_chessPieces[i].position.y + SQUARE_SIZE / 2.0f;
                break;
            }
        }

        std::cout << "Castling: Moved rook from " << move.rookStartSquare << " to " << move.rookTargetSquare << std::endl;
    }
    if ((currentPiece & Piece::Pawn) && abs(move.startSquare - move.targetSquare) == 16) {
        m_gameFlags.enPassantTargetSquare = (move.startSquare + move.targetSquare) / 2;
    }
    else {
        m_gameFlags.enPassantTargetSquare = -1;
    }

    if (move.isEnPassant) {
        int capturedPawnSquare = move.targetSquare + (IsPieceWhite(currentPiece) ? 8 : -8);
        m_board[capturedPawnSquare] = Piece::None;

        // Remove the captured pawn from chessPieces array
        for (int i = 0; i < 32; i++) {
            if (m_chessPieces[i].position.x == (capturedPawnSquare % BOARD_SIZE) * SQUARE_SIZE &&
                m_chessPieces[i].position.y == (capturedPawnSquare / BOARD_SIZE) * SQUARE_SIZE) {
                m_chessPieces[i].type = Piece::None;
                break;
            }
        }

        std::cout << "En passant capture: Removed pawn at " << capturedPawnSquare << std::endl;
    }

}

void ChessBoard::UpdateGameFlags(int selectedPieceIndex) {
    if (selectedPieceIndex == 0 && !m_gameFlags.a8RookHasMoved) {
        m_gameFlags.a8RookHasMoved = true;
    }
    else if (selectedPieceIndex == 4 && !m_gameFlags.blackKingHasMoved) {
        m_gameFlags.blackKingHasMoved = true;
    }
    else if (selectedPieceIndex == 7 && !m_gameFlags.h8RookHasMoved) {
        m_gameFlags.h8RookHasMoved = true;
    }
    else if (selectedPieceIndex == 63 && !m_gameFlags.h1RookHasMoved) {
        m_gameFlags.h1RookHasMoved = true;
    }
    else if (selectedPieceIndex == 60 && !m_gameFlags.whiteKingHasMoved) {
        m_gameFlags.whiteKingHasMoved = true;
    }
    else if (selectedPieceIndex == 56 && !m_gameFlags.a1RookHasMoved) {
        m_gameFlags.a1RookHasMoved = true;
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
    int pieceCount = 0;
    for (int i = 0; i < TOTAL_SQUARES; i++) {
        if (m_board[i] != Piece::None) {
            m_chessPieces[pieceCount].type = m_board[i];
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