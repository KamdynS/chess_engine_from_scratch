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
        return GeneratePawnMoves(indexOnBoard, currentPiece, board);
    case Piece::King:
        return GenerateKingMove(indexOnBoard, currentPiece, board, flags);
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

std::vector<int> PiecesAttackingCurrentSquare(int indexOfSquare, const BoardState& board, const GameRuleFlags& flags) {
    std::vector<int> results;
    int pieceType = board[indexOfSquare];
    int moveCount = std::numeric_limits<int>::max(); // Positive infinity

    for (int i = 0; i < 64; i++) {
        int currentPiece = board[i];
        if (currentPiece != Piece::None && (currentPiece & pieceType) == 0) {
            std::vector<Move> tempMoves = GenerateLegalMoves(currentPiece, moveCount, i, board, flags);
            for (const auto& move : tempMoves) {
                if (move.targetSquare == indexOfSquare) {
                    results.push_back(i);
                    break;  // Found an attacking move, no need to check others
                }
            }
        }
    }
    return results;
}

bool IsSquareAttacked(int indexOfSquare, const BoardState& board, const GameRuleFlags& flags) {
    if (PiecesAttackingCurrentSquare(indexOfSquare, board, flags).size() == 0) {
        return false;
    }
    else {
        return true;
    }
}