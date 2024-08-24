#include "Board.h"
#include "Pieces.h"
#include "Game.h"
#include <limits>
#include <unordered_map>

std::vector<Move> GenerateLegalMoves(int currentPiece, int moveCount, int indexOnBoard, const BoardState& board, const GameRuleFlags& flags) {
    if (!IsCorrectMove(currentPiece, moveCount)) {
        return {};  // Return an empty vector
    }
    std::vector<Move> allMoves;

    switch (currentPiece & 7) {  // Use bitwise AND with 7 to get just the piece type
    case Piece::Rook:
    case Piece::Queen:
    case Piece::Bishop:
    {
        std::vector<Move> bishopMoves = GenerateSlidingMoves(indexOnBoard, currentPiece, board);
        allMoves.insert(allMoves.end(), bishopMoves.begin(), bishopMoves.end());
        break;
    }
    case Piece::Knight:
    {
        std::vector<Move> knightMoves = GenerateKnightMoves(indexOnBoard, currentPiece, board);
        allMoves.insert(allMoves.end(), knightMoves.begin(), knightMoves.end());
        break;
    }
    case Piece::Pawn:
    {
        std::vector<Move> pawnMoves = GeneratePawnMoves(indexOnBoard, currentPiece, board, flags);
        allMoves.insert(allMoves.end(), pawnMoves.begin(), pawnMoves.end());
        break;
    }
    case Piece::King:
    {
        std::vector<Move> kingMoves = GenerateKingMoves(indexOnBoard, currentPiece, board, flags);
        allMoves.insert(allMoves.end(), kingMoves.begin(), kingMoves.end());
        break;
    }
    default:
        // Handle unexpected piece type
        std::cerr << "Unexpected piece type: " << currentPiece << std::endl;
        return {};
    }
    return FilterLegalMoves(board, allMoves, currentPiece, flags);
}

std::vector<Move> FilterLegalMoves(const BoardState& board, const std::vector<Move>& moves, int currentPlayer, GameRuleFlags flags) {
    std::vector<Move> legalMoves;
    for (const auto& move : moves) {
        // Make the move on a temporary board
        BoardState tempBoard = board;
        MakeMove(tempBoard, move, currentPlayer, flags); 

        // If the move doesn't leave the king in check, it's legal
        int currentColor = currentPlayer & 0b11000;
        // int kingColor = (board[FindKingLocation(currentColor, board)] & 11000);
        if (!IsKingInCheck(tempBoard, currentColor)) {
            legalMoves.push_back(move);
        }
        else {
            std::cout << "Move" << move.startSquare << "is illegal because king is in check";
        }
    }
    return legalMoves;
}


bool IsPieceWhite(int pieceType) {
    return (pieceType & Piece::White) != 0;
}

bool IsWhiteMove(int move) {
    return move % 2 != 0;
}

//Will return true if it is this piece's turn to move
bool IsCorrectMove(int pieceType, int moveCount) {
    int positive_infinity = std::numeric_limits<int>::max();
    if (moveCount == positive_infinity) { //Hacky way to be able to say "use GenerateLegalMoves function outside of regular game loop"
        return true;
    }
    else {
        return !IsPieceWhite(pieceType) && !IsWhiteMove(moveCount) || IsPieceWhite(pieceType) && IsWhiteMove(moveCount);
    }
}

bool IsSquareAttackedSimple(int square, int attackingColor, const BoardState& board) {
    // Check for pawn attacks
    int pawnDirection = (attackingColor == Piece::White) ? -1 : 1;
    int pawnAttackLeft = square + pawnDirection * 7;
    int pawnAttackRight = square + pawnDirection * 9;
    if (pawnAttackLeft >= 0 && pawnAttackLeft < 64 && board[pawnAttackLeft] == (Piece::Pawn | attackingColor)) return true;
    if (pawnAttackRight >= 0 && pawnAttackRight < 64 && board[pawnAttackRight] == (Piece::Pawn | attackingColor)) return true;

    // Check for knight attacks
    int knightMoves[] = { -17, -15, -10, -6, 6, 10, 15, 17 };
    for (int move : knightMoves) {
        int targetSquare = square + move;
        if (targetSquare >= 0 && targetSquare < 64 && board[targetSquare] == (Piece::Knight | attackingColor)) return true;
    }

    // Check for king attacks (for adjacent squares)
    int kingMoves[] = { -9, -8, -7, -1, 1, 7, 8, 9 };
    for (int move : kingMoves) {
        int targetSquare = square + move;
        if (targetSquare >= 0 && targetSquare < 64 && board[targetSquare] == (Piece::King | attackingColor)) return true;
    }

    // Check for sliding piece attacks (rook, bishop, queen)
    int slidingDirections[] = { -8, 8, -1, 1, -9, 9, -7, 7 };
    for (int direction : slidingDirections) {
        int targetSquare = square + direction;
        while (targetSquare >= 0 && targetSquare < 64) {
            int piece = board[targetSquare];
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

bool IsKingInCheck(const BoardState& board, int kingColor) {
    int kingLocation = FindKingLocation(kingColor, board);
    if (kingLocation == -1) {
        // This shouldn't happen in a valid chess position
        std::cerr << "Error: King not found on the board" << std::endl;
        return false;
    }

    int oppositeColor = (kingColor == Piece::White) ? Piece::Black : Piece::White;
    return IsSquareAttackedSimple(kingLocation, oppositeColor, board);
}

bool BlackCheckmate(int currentPiece, int moveCount, int indexOnBoard, const BoardState& board, const GameRuleFlags& flags) {
    std::vector<Move> possibleMoves = GenerateLegalMoves(currentPiece, moveCount, indexOnBoard, board, flags);
    if (possibleMoves.empty() && IsKingInCheck(board, Piece::White)) {
        std::cout << "Black Wins!";
        return true;
    }
    else {
        return false;
    }
}

bool WhiteCheckmate(int currentPiece, int moveCount, int indexOnBoard, const BoardState& board, const GameRuleFlags& flags) {
    std::vector<Move> possibleMoves = GenerateLegalMoves(currentPiece, moveCount, indexOnBoard, board, flags);
    if (possibleMoves.empty() && IsKingInCheck(board, Piece::Black)) {
        std::cout << "White Wins!";
        return true;
    }
    else {
        return false;
    }
}

bool GameDrawStaleMate(int moveCount, int indexOnBoard, const BoardState& board, const GameRuleFlags& flags) {
    std::vector<Move> possibleMoves;
    for (int i = 0; i <= 63; i++) {
        std::vector<Move> kingMoves = GenerateLegalMoves(board[i], moveCount, indexOnBoard, board, flags);
        possibleMoves.insert(possibleMoves.end(), kingMoves.begin(), kingMoves.end());
    }
    if (possibleMoves.empty() && (!IsKingInCheck(board, Piece::White) && !IsKingInCheck(board, Piece::Black))) {
        std::cout << "The Game is a draw";
        return true;
    }
    else {
        return false;
    }
}

bool GameDrawInsufficientMaterial(const PieceBitboards& bitboards) {
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

bool GameDrawFiftyMove(const GameRuleFlags& flags) {
    return flags.halfMoveClock >= 100;  // 50 full moves = 100 half-moves
}

bool GameDrawThreefold(const std::vector<uint64_t>& positionHistory) {
    if (positionHistory.size() < 5) return false; // Need at least 5 moves for a threefold repetition

    std::unordered_map<uint64_t, int> positionCounts;

    for (const auto& hash : positionHistory) {
        positionCounts[hash]++;
        if (positionCounts[hash] >= 3) {
            return true;
        }
    }

    return false;
}