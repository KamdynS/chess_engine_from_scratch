// Game.cpp
#include "Game.h"
#include <limits>
#include <unordered_map>
#include <iostream>

Game::Game() : m_moveCount(1) {
    m_board.InitializeBoard();
}

std::vector<Move> Game::GenerateLegalMoves(int currentPiece, int indexOnBoard) const {
    if (!IsCorrectMove(currentPiece)) {
        return {};  // Return an empty vector
    }
    std::vector<Move> allMoves;

    switch (currentPiece & 7) {  // Use bitwise AND with 7 to get just the piece type
    case Piece::Rook:
    case Piece::Queen:
    case Piece::Bishop:
    {
        std::vector<Move> bishopMoves = m_pieceManager.GenerateSlidingMoves(indexOnBoard, currentPiece, m_board.GetBoardState());
        allMoves.insert(allMoves.end(), bishopMoves.begin(), bishopMoves.end());
        break;
    }
    case Piece::Knight:
    {
        std::vector<Move> knightMoves = m_pieceManager.GenerateKnightMoves(indexOnBoard, currentPiece, m_board.GetBoardState());
        allMoves.insert(allMoves.end(), knightMoves.begin(), knightMoves.end());
        break;
    }
    case Piece::Pawn:
    {
        std::vector<Move> pawnMoves = m_pieceManager.GeneratePawnMoves(indexOnBoard, currentPiece, m_board.GetBoardState(), m_board.GetGameFlags());
        allMoves.insert(allMoves.end(), pawnMoves.begin(), pawnMoves.end());
        break;
    }
    case Piece::King:
    {
        std::vector<Move> kingMoves = m_pieceManager.GenerateKingMoves(indexOnBoard, currentPiece, m_board.GetBoardState(), m_board.GetGameFlags());
        allMoves.insert(allMoves.end(), kingMoves.begin(), kingMoves.end());
        break;
    }
    default:
        // Handle unexpected piece type
        std::cerr << "Unexpected piece type: " << currentPiece << std::endl;
        return {};
    }
    return FilterLegalMoves(allMoves, currentPiece);
}

std::vector<Move> Game::FilterLegalMoves(const std::vector<Move>& moves, int currentPlayer) const {
    std::vector<Move> legalMoves;
    for (const auto& move : moves) {
        // Make the move on a temporary board
        ChessBoard tempBoard = m_board;
        tempBoard.MakeMove(move, currentPlayer);

        // If the move doesn't leave the king in check, it's legal
        int currentColor = currentPlayer & 0b11000;
        if (!IsKingInCheck(currentColor)) {
            legalMoves.push_back(move);
        }
        else {
            std::cout << "Move " << move.startSquare << " is illegal because king is in check" << std::endl;
        }
    }
    return legalMoves;
}

bool Game::IsPieceWhite(int pieceType) const {
    return (pieceType & Piece::White) != 0;
}

bool Game::IsWhiteMove() const {
    return m_moveCount % 2 != 0;
}

bool Game::IsCorrectMove(int pieceType) const {
    return IsPieceWhite(pieceType) == IsWhiteMove();
}

bool Game::IsSquareAttackedSimple(int square, int attackingColor) const {
    const BoardState& board = m_board.GetBoardState();

    // ... (rest of the implementation remains the same)
}

bool Game::IsKingInCheck(int kingColor) const {
    const BoardState& board = m_board.GetBoardState();
    int kingLocation = m_pieceManager.FindKingLocation(kingColor, board);
    if (kingLocation == -1) {
        // This shouldn't happen in a valid chess position
        std::cerr << "Error: King not found on the board" << std::endl;
        return false;
    }

    int oppositeColor = (kingColor == Piece::White) ? Piece::Black : Piece::White;
    return IsSquareAttackedSimple(kingLocation, oppositeColor);
}

bool Game::BlackCheckmate() {
    const BoardState& board = m_board.GetBoardState();
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (board[i] & Piece::Black) {
            std::vector<Move> possibleMoves = GenerateLegalMoves(board[i], i);
            if (!possibleMoves.empty()) {
                return false;
            }
        }
    }
    if (IsKingInCheck(Piece::Black)) {
        std::cout << "Black is checkmated. White wins!" << std::endl;
        return true;
    }
    return false;
}

bool Game::WhiteCheckmate() {
    const BoardState& board = m_board.GetBoardState();
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (board[i] & Piece::White) {
            std::vector<Move> possibleMoves = GenerateLegalMoves(board[i], i);
            if (!possibleMoves.empty()) {
                return false;
            }
        }
    }
    if (IsKingInCheck(Piece::White)) {
        std::cout << "White is checkmated. Black wins!" << std::endl;
        return true;
    }
    return false;
}

bool Game::GameDrawStaleMate() {
    const BoardState& board = m_board.GetBoardState();
    std::vector<Move> possibleMoves;
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (board[i] != Piece::None) {
            std::vector<Move> pieceMoves = GenerateLegalMoves(board[i], i);
            possibleMoves.insert(possibleMoves.end(), pieceMoves.begin(), pieceMoves.end());
        }
    }
    if (possibleMoves.empty() && (!IsKingInCheck(Piece::White) && !IsKingInCheck(Piece::Black))) {
        std::cout << "The game is a draw by stalemate" << std::endl;
        return true;
    }
    return false;
}

bool Game::GameDrawInsufficientMaterial() const {
    const PieceBitboards& bitboards = m_board.GetBitboards();
    // ... (rest of the implementation remains the same)
}

bool Game::GameDrawFiftyMove() const {
    return m_board.GetGameFlags().halfMoveClock >= 100;  // 50 full moves = 100 half-moves
}

bool Game::GameDrawThreefold() const {
    if (m_positionHistory.size() < 5) return false; // Need at least 5 moves for a threefold repetition

    std::unordered_map<uint64_t, int> positionCounts;

    for (const auto& hash : m_positionHistory) {
        positionCounts[hash]++;
        if (positionCounts[hash] >= 3) {
            return true;
        }
    }

    return false;
}

bool Game::GameDrawInsufficientMaterial() const {
    // Combine all pieces except kings into a single bitboard
    Bitboard::BitboardType allPieces =
        bitboards.WhitePawns.board | bitboards.WhiteKnights.board |
        bitboards.WhiteBishops.board | bitboards.WhiteRooks.board |
        bitboards.WhiteQueens.board |
        bitboards.BlackPawns.board | bitboards.BlackKnights.board |
        bitboards.BlackBishops.board | bitboards.BlackRooks.board |
        bitboards.BlackQueens.board;

    // If no pieces left besides kings, it's a draw
    if (allPieces == 0) return true;

    // If more than 3 pieces, it's not insufficient material
    if (__builtin_popcountll(allPieces) > 3) return false;

    // Check for single minor piece (bishop or knight)
    if (__builtin_popcountll(allPieces) == 1) {
        return (allPieces & (bitboards.WhiteKnights.board | bitboards.BlackKnights.board |
            bitboards.WhiteBishops.board | bitboards.BlackBishops.board)) != 0;
    }

    // Check for two knights
    if (__builtin_popcountll(allPieces) == 2) {
        return allPieces == bitboards.WhiteKnights.board || allPieces == bitboards.BlackKnights.board;
    }

    // Check for bishops
    Bitboard::BitboardType bishopPieces = bitboards.WhiteBishops.board | bitboards.BlackBishops.board;
    if (allPieces == bishopPieces) {
        // Check if bishops are on the same color
        const Bitboard::BitboardType LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;
        Bitboard::BitboardType lightSquareBishops = allPieces & LIGHT_SQUARES;
        return (lightSquareBishops == 0 || lightSquareBishops == allPieces);
    }

    return false;
}

