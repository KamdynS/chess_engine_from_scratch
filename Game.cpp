#include "Board.h"
#include "Pieces.h"
#include "Game.h"
#include <limits>

std::vector<Move> GenerateLegalMoves(int currentPiece, int moveCount, int indexOnBoard, const BoardState& board, const GameRuleFlags& flags) {
    if (!IsCorrectMove(currentPiece, moveCount)) {
        return {};  // Return an empty vector
    }

    switch (currentPiece & 7) {  // Use bitwise AND with 7 to get just the piece type
    case Piece::Rook:
    case Piece::Queen:
    case Piece::Bishop:
        return GenerateSlidingMoves(indexOnBoard, currentPiece, board);
    case Piece::Knight:
        return GenerateKnightMoves(indexOnBoard, currentPiece, board);
    case Piece::Pawn:
        return GeneratePawnMoves(indexOnBoard, currentPiece, board, flags);
    case Piece::King:
        return GenerateKingMoves(indexOnBoard, currentPiece, board, flags);
    default:
        // Handle unexpected piece type
        std::cerr << "Unexpected piece type: " << currentPiece << std::endl;
        return {};
    }
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

