#include "include/Game.h"
#include "include/GameState.h"
#include "include/CommonComponents.h"
#include <limits>
#include <unordered_map>
#include <iostream>

Game::Game() : m_pieceManager() {
    // ChessBoard initialization is now handled by GameState
}

std::vector<Move> Game::GenerateLegalMoves(int currentPiece, int indexOnBoard) const {
    auto& gameState = GameState::getInstance();
    if (!IsCorrectMove(currentPiece)) {
        return {};  // Return an empty vector
    }
    std::vector<Move> allMoves;

    switch (currentPiece & 7) {  // Use bitwise AND with 7 to get just the piece type
    case Piece::Rook:
    case Piece::Queen:
    case Piece::Bishop:
    {
        allMoves = m_pieceManager.GenerateSlidingMoves(indexOnBoard, currentPiece);
        break;
    }
    case Piece::Knight:
    {
        allMoves = m_pieceManager.GenerateKnightMoves(indexOnBoard, currentPiece);
        break;
    }
    case Piece::Pawn:
    {
        allMoves = m_pieceManager.GeneratePawnMoves(indexOnBoard, currentPiece);
        break;
    }
    case Piece::King:
    {
        allMoves = m_pieceManager.GenerateKingMoves(indexOnBoard, currentPiece, *this);
        break;
    }
    default:
        std::cerr << "Unexpected piece type: " << currentPiece << std::endl;
        return {};
    }
    return FilterLegalMoves(allMoves, currentPiece);
}

std::vector<Move> Game::FilterLegalMoves(const std::vector<Move>& moves, int currentPlayer) const {
    auto& gameState = GameState::getInstance();
    std::vector<Move> legalMoves;
    for (const auto& move : moves) {
        BoardState tempBoard = gameState.board;
        GameRuleFlags tempFlags = gameState.gameFlags;
        int tempMoveCount = gameState.moveCount;

        Move moveCopy = move;  // Create a non-const copy
        const_cast<Game*>(this)->MakeMove(moveCopy, currentPlayer);

        int currentColor = currentPlayer & 0b11000;
        if (!IsKingInCheck(currentColor)) {
            legalMoves.push_back(move);
        }

        // Restore the original game state
        gameState.board = tempBoard;
        gameState.gameFlags = tempFlags;
        gameState.moveCount = tempMoveCount;
    }
    return legalMoves;
}

bool Game::IsPieceWhite(int pieceType) const {
    return (pieceType & Piece::White) != 0;
}

bool Game::IsWhiteMove() const {
    return GameState::getInstance().moveCount % 2 != 0;
}

bool Game::IsCorrectMove(int pieceType) const {
    return IsPieceWhite(pieceType) == IsWhiteMove();
}

bool Game::IsSquareAttackedSimple(int square, int attackingColor) const {
    auto& gameState = GameState::getInstance();
    // Check for pawn attacks
    int pawnDirection = (attackingColor == Piece::White) ? -1 : 1;
    int pawnAttackLeft = square + pawnDirection * 7;
    int pawnAttackRight = square + pawnDirection * 9;
    if (pawnAttackLeft >= 0 && pawnAttackLeft < 64 && gameState.board[pawnAttackLeft] == (Piece::Pawn | attackingColor)) return true;
    if (pawnAttackRight >= 0 && pawnAttackRight < 64 && gameState.board[pawnAttackRight] == (Piece::Pawn | attackingColor)) return true;

    // Check for knight attacks
    int knightMoves[] = { -17, -15, -10, -6, 6, 10, 15, 17 };
    for (int move : knightMoves) {
        int targetSquare = square + move;
        if (targetSquare >= 0 && targetSquare < 64 && gameState.board[targetSquare] == (Piece::Knight | attackingColor)) return true;
    }

    // Check for king attacks (for adjacent squares)
    int kingMoves[] = { -9, -8, -7, -1, 1, 7, 8, 9 };
    for (int move : kingMoves) {
        int targetSquare = square + move;
        if (targetSquare >= 0 && targetSquare < 64 && gameState.board[targetSquare] == (Piece::King | attackingColor)) return true;
    }

    // Check for sliding piece attacks (rook, bishop, queen)
    int slidingDirections[] = { -8, 8, -1, 1, -9, 9, -7, 7 };
    for (int direction : slidingDirections) {
        int targetSquare = square + direction;
        while (targetSquare >= 0 && targetSquare < 64) {
            int piece = gameState.board[targetSquare];
            if (piece != Piece::None) {
                if ((piece & attackingColor) &&
                    (piece == (Piece::Queen | attackingColor) ||
                        (abs(direction) == 8 || abs(direction) == 1) && piece == (Piece::Rook | attackingColor) ||
                        (abs(direction) == 7 || abs(direction) == 9) && piece == (Piece::Bishop | attackingColor))) {
                    return true;
                }
                break;
            }
            targetSquare += direction;
        }
    }

    return false;
}

bool Game::IsKingInCheck(int kingColor) const {
    auto& gameState = GameState::getInstance();
    int kingLocation = m_pieceManager.FindKingLocation(kingColor);
    if (kingLocation == -1) {
        std::cerr << "Error: King not found on the board" << std::endl;
        return false;
    }

    int oppositeColor = (kingColor == Piece::White) ? Piece::Black : Piece::White;
    return IsSquareAttackedSimple(kingLocation, oppositeColor);
}

bool Game::BlackCheckmate() const {
    auto& gameState = GameState::getInstance();
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (gameState.board[i] & Piece::Black) {
            std::vector<Move> possibleMoves = GenerateLegalMoves(gameState.board[i], i);
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

bool Game::WhiteCheckmate() const {
    auto& gameState = GameState::getInstance();
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (gameState.board[i] & Piece::White) {
            std::vector<Move> possibleMoves = GenerateLegalMoves(gameState.board[i], i);
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

bool Game::GameDrawStaleMate() const{
    auto& gameState = GameState::getInstance();
    std::vector<Move> possibleMoves;
    for (int i = 0; i < TOTAL_SQUARES; ++i) {
        if (gameState.board[i] != Piece::None) {
            std::vector<Move> pieceMoves = GenerateLegalMoves(gameState.board[i], i);
            possibleMoves.insert(possibleMoves.end(), pieceMoves.begin(), pieceMoves.end());
        }
    }
    if (possibleMoves.empty() && (!IsKingInCheck(Piece::White) && !IsKingInCheck(Piece::Black))) {
        std::cout << "The game is a draw by stalemate" << std::endl;
        return true;
    }
    return false;
}

bool Game::GameDrawFiftyMove() const{
    return GameState::getInstance().gameFlags.halfMoveClock >= 100;  // 50 full moves = 100 half-moves
}

bool Game::GameDrawThreefold() const {
    auto& gameState = GameState::getInstance();
    if (gameState.positionHistory.size() < 5) return false; // Need at least 5 moves for a threefold repetition

    std::unordered_map<uint64_t, int> positionCounts;

    for (const auto& hash : gameState.positionHistory) {
        positionCounts[hash]++;
        if (positionCounts[hash] >= 3) {
            return true;
        }
    }

    return false;
}

bool Game::GameDrawInsufficientMaterial() const {
    auto& gameState = GameState::getInstance();
    // Combine all pieces except kings into a single bitboard
    Bitboard::BitboardType allPieces =
        gameState.bitboards.WhitePawns.board | gameState.bitboards.WhiteKnights.board |
        gameState.bitboards.WhiteBishops.board | gameState.bitboards.WhiteRooks.board |
        gameState.bitboards.WhiteQueens.board |
        gameState.bitboards.BlackPawns.board | gameState.bitboards.BlackKnights.board |
        gameState.bitboards.BlackBishops.board | gameState.bitboards.BlackRooks.board |
        gameState.bitboards.BlackQueens.board;

    // If no pieces left besides kings, it's a draw
    if (allPieces == 0) return true;

    // If more than 3 pieces, it's not insufficient material
    if (__builtin_popcountll(allPieces) > 3) return false;

    // Check for single minor piece (bishop or knight)
    if (__builtin_popcountll(allPieces) == 1) {
        return (allPieces & (gameState.bitboards.WhiteKnights.board | gameState.bitboards.BlackKnights.board |
            gameState.bitboards.WhiteBishops.board | gameState.bitboards.BlackBishops.board)) != 0;
    }

    // Check for two knights
    if (__builtin_popcountll(allPieces) == 2) {
        return allPieces == gameState.bitboards.WhiteKnights.board || allPieces == gameState.bitboards.BlackKnights.board;
    }

    // Check for bishops
    Bitboard::BitboardType bishopPieces = gameState.bitboards.WhiteBishops.board | gameState.bitboards.BlackBishops.board;
    if (allPieces == bishopPieces) {
        // Check if bishops are on the same color
        const Bitboard::BitboardType LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;
        Bitboard::BitboardType lightSquareBishops = allPieces & LIGHT_SQUARES;
        return (lightSquareBishops == 0 || lightSquareBishops == allPieces);
    }

    return false;
}

void Game::MakeMove(Move& move, int currentPiece) {
    auto& gameState = GameState::getInstance();
    if (move.startSquare != move.targetSquare) {
        bool isPawnMove = (currentPiece & 7) == Piece::Pawn;
        bool isCapture = gameState.board[move.targetSquare] != Piece::None || move.isEnPassant;

        if (isPawnMove || isCapture) {
            gameState.gameFlags.halfMoveClock = 0;
        } else {
            gameState.gameFlags.halfMoveClock++;
        }

        gameState.board[move.startSquare] = Piece::None;

        if (move.isPromotion) {
            gameState.board[move.targetSquare] = (currentPiece & Piece::White) ? Piece::WhiteQueen : Piece::BlackQueen;
            move.promotionPiece = (currentPiece & Piece::White) ? Piece::WhiteQueen : Piece::BlackQueen;
        } else {
            gameState.board[move.targetSquare] = currentPiece;
        }

        if (move.isEnPassant) {
            int capturedPawnSquare = move.targetSquare + ((currentPiece & Piece::White) ? 8 : -8);
            gameState.board[capturedPawnSquare] = Piece::None;
        } else if (move.isCastling) {
            gameState.board[move.rookStartSquare] = Piece::None;
            gameState.board[move.rookTargetSquare] = (currentPiece & Piece::White) ? Piece::WhiteRook : Piece::BlackRook;
        }

        m_pieceManager.UpdateBitboards(move, currentPiece);
        gameState.moveCount++;

        // This was previously in ChessBoard, consider if it's still needed and where it should be called
        // UpdateChessPieces();
    }
}