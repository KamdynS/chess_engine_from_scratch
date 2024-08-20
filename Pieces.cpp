#include "Pieces.h"
#include "Board.h"
#include "Game.h"
#include <iostream>
#include <algorithm>

const std::vector<std::pair<int, int>> DIRECTIONS = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0},  
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
};

SquareDistances CalculateDistances(int squareIndex) {
    SquareDistances distances;

    // Calculate row and column from squareIndex
    int row = squareIndex / 8;
    int col = squareIndex % 8;

    // Calculate distances to edges
    distances.top = row;
    distances.bottom = 7 - row;
    distances.left = col;
    distances.right = 7 - col;

    // Calculate diagonal distances
    // Use std::min to find the shorter distance for diagonals
    distances.topLeft = std::min(distances.top, distances.left);
    distances.topRight = std::min(distances.top, distances.right);
    distances.bottomLeft = std::min(distances.bottom, distances.left);
    distances.bottomRight = std::min(distances.bottom, distances.right);

    return distances;
}
 //Generate moves that slide. Goes all the way until it hits a piece or edge of board. Will work with Queen, Bishop, or Knight.
std::vector<Move> GenerateSlidingMoves(int indexOnBoard, int pieceType, const BoardState& board) {
    std::vector<Move> moves;
    int currentRow = indexOnBoard / 8;
    int currentCol = indexOnBoard % 8;
    int type = pieceType & 7;
    bool checkDiagonal = (type == Piece::Bishop || type == Piece::Queen);
    bool checkStraight = (type == Piece::Rook || type == Piece::Queen);

    auto checkDirection = [&](int dirIndex) {
        int newRow = currentRow;
        int newCol = currentCol;
        while (true) {
            newRow += DIRECTIONS[dirIndex].first;
            newCol += DIRECTIONS[dirIndex].second;
            if (newRow < 0 || newRow >= 8 || newCol < 0 || newCol >= 8) {
                break;  // Off the board
            }
            int target = newRow * 8 + newCol;
            if (board[target] != Piece::None) {
                // Add capture move if it's an opponent's piece
                if ((board[target] & Piece::White) != (pieceType & Piece::White)) {
                    moves.push_back({ indexOnBoard, target });
                }
                break;
            }
            moves.push_back({ indexOnBoard, target });
        }
        };

    if (checkDiagonal) {
        for (int i = 4; i < DIRECTIONS.size(); i++) {
            checkDirection(i);
        }
    }
    if (checkStraight) {
        for (int i = 0; i < 4; i++) {
            checkDirection(i);
        }
    }

    return moves;
}

std::vector<Move> GenerateKnightMoves(int indexOnBoard, int pieceType, const BoardState& board) {
    std::vector<Move> moves;
    int currentRow = indexOnBoard / 8;
    int currentCol = indexOnBoard % 8;

    // All possible knight moves
    const std::vector<std::pair<int, int>> knightOffsets = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };

    for (const auto& offset : knightOffsets) {
        int newRow = currentRow + offset.first;
        int newCol = currentCol + offset.second;

        // Check if the new position is on the board
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
            int target = newRow * 8 + newCol;

            // Check if the square is empty or occupied by an opponent's piece
            if (board[target] == Piece::None ||
                ((board[target] & Piece::White) != (pieceType & Piece::White))) {
                moves.push_back({ indexOnBoard, target });
            }
        }
    }

    return moves;
}

std::vector<Move> GeneratePawnMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags) {
    std::vector<Move> moves;
    int currentRow = indexOnBoard / 8;
    int currentCol = indexOnBoard % 8;

    // Determine direction based on color
    int direction = (pieceType & Piece::White) ? -1 : 1;

    // Check forward move
    int newRow = currentRow + direction;
    if (newRow >= 0 && newRow < 8) {
        int target = newRow * 8 + currentCol;
        if (board[target] == Piece::None) {
            Move move = { indexOnBoard, target };
            moves.push_back(move);
            // Check double move if it's the pawn's first move
            if ((direction == -1 && currentRow == 6) || (direction == 1 && currentRow == 1)) {
                newRow += direction;
                target = newRow * 8 + currentCol;
                if (board[target] == Piece::None) {
                    moves.push_back({ indexOnBoard, target });
                }
            }
        }
    }

    // Check diagonal captures
    for (int colOffset : {-1, 1}) {
        int newCol = currentCol + colOffset;
        if (newCol >= 0 && newCol < 8) {
            int target = (currentRow + direction) * 8 + newCol;
            if (board[target] != Piece::None &&
                ((board[target] & Piece::White) != (pieceType & Piece::White))) {
                Move move = { indexOnBoard, target };
                // Check for promotion
                if ((direction == -1 && newRow == 0) || (direction == 1 && newRow == 7)) {
                    move.isPromotion = true;
                }
                moves.push_back(move);
            }
        }
    }

    if (flags.enPassantTargetSquare != -1) {
        // Check if the current pawn is in the correct position to make an en passant capture
        if ((pieceType & Piece::White && indexOnBoard / 8 == 3) ||
            (pieceType & Piece::Black && indexOnBoard / 8 == 4)) {
            // Check if the en passant target is adjacent to the pawn
            if (abs(indexOnBoard % 8 - flags.enPassantTargetSquare % 8) == 1) {
                Move enPassantMove;
                enPassantMove.startSquare = indexOnBoard;
                enPassantMove.targetSquare = flags.enPassantTargetSquare;
                enPassantMove.isEnPassant = true;
                moves.push_back(enPassantMove);
            }
        }
    }

    return moves;
}

std::vector<Move> GenerateKingMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags) {
    std::vector<Move> moves;
    int currentRow = indexOnBoard / 8;
    int currentCol = indexOnBoard % 8;

    // Generate all moves surrounding king
    for (int i = 0; i < 8; i++) {
        int newRow = currentRow + DIRECTIONS[i].first;
        int newCol = currentCol + DIRECTIONS[i].second;

        if (newRow < 0 || newRow >= 8 || newCol < 0 || newCol >= 8) {
            continue;  // Off the board
        }
        int target = (newRow * 8) + newCol;
        if (board[target] != Piece::None) {
            // Add capture move if it's an opponent's piece
            if ((board[target] & Piece::White) != (pieceType & Piece::White)) {
                moves.push_back({ indexOnBoard, target });
            }
            continue;
        }
        moves.push_back({ indexOnBoard, target });
    }

    // Castle if available
    if (CanCastle(pieceType, true, board, flags)) {  // Kingside
        Move castlingMove;
        castlingMove.startSquare = indexOnBoard;
        castlingMove.targetSquare = indexOnBoard + 2;
        castlingMove.isCastling = true;
        castlingMove.rookStartSquare = (pieceType & Piece::White) ? ChessSquares::H1 : ChessSquares::H8;
        castlingMove.rookTargetSquare = castlingMove.rookStartSquare - 2;
        moves.push_back(castlingMove);
    }

    if (CanCastle(pieceType, false, board, flags)) {  // Queenside
        Move castlingMove;
        castlingMove.startSquare = indexOnBoard;
        castlingMove.targetSquare = indexOnBoard - 2;
        castlingMove.isCastling = true;
        castlingMove.rookStartSquare = (pieceType & Piece::White) ? ChessSquares::A1 : ChessSquares::A8;
        castlingMove.rookTargetSquare = castlingMove.rookStartSquare + 3;
        moves.push_back(castlingMove);
    }
    std::cout << "King Moves Generated: " << moves.size() << std::endl;
    return moves;
}

bool CanCastle(int kingType, bool kingSide, const BoardState& board, const GameRuleFlags& flags) {
    int kingColor = (kingType & Piece::White) ? Piece::White : Piece::Black;
    int oppositeColor = (kingColor == Piece::White) ? Piece::Black : Piece::White;
    int kingStartSquare = (kingColor == Piece::White) ? ChessSquares::E1 : ChessSquares::E8;

    // Check if king or rook has moved
    if (kingColor == Piece::White) {
        if (flags.whiteKingHasMoved) return false;
        if (kingSide && flags.h1RookHasMoved) return false;
        if (!kingSide && flags.a1RookHasMoved) return false;
    }
    else {
        if (flags.blackKingHasMoved) return false;
        if (kingSide && flags.h8RookHasMoved) return false;
        if (!kingSide && flags.a8RookHasMoved) return false;
    }

    // Check if squares between king and rook are empty
    int direction = kingSide ? 1 : -1;
    for (int i = 1; i <= (kingSide ? 2 : 3); i++) {
        if (board[kingStartSquare + i * direction] != Piece::None) return false;
    }

    // Check if king is in check or passes through attacked square
    for (int i = 0; i <= 2; i++) {
        if (IsSquareAttackedSimple(kingStartSquare + i * direction, oppositeColor, board)) return false;
    }

    return true;
}

int FindKingLocation(int kingColor, const BoardState& board) {
    int targetKing = (kingColor & Piece::White) ? Piece::WhiteKing : Piece::BlackKing;

    for (int i = 0; i < TOTAL_SQUARES; i++) {
        if (board[i] == targetKing) {
            return i;
        }
    }

    return -1;  // King not found (shouldn't happen in a valid chess position)
}