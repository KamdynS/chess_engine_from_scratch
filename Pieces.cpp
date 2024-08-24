#include "Pieces.h"
#include "ChessBoard.h"
#include "Game.h"
#include <iostream>
#include <algorithm>

PieceManager::PieceManager() {
    m_directions = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };
}

SquareDistances PieceManager::CalculateDistances(int squareIndex) const {
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
std::vector<Move> PieceManager::GenerateSlidingMoves(int indexOnBoard, int pieceType, const BoardState& board) const {
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
            newRow += m_directions[dirIndex].first;
            newCol += m_directions[dirIndex].second;
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
        for (int i = 4; i < m_directions.size(); i++) {
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

std::vector<Move> PieceManager::GenerateKnightMoves(int indexOnBoard, int pieceType, const BoardState& board) const {
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

std::vector<Move> PieceManager::GeneratePawnMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags) const {
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

std::vector<Move> PieceManager::GenerateKingMoves(int indexOnBoard, int pieceType, const BoardState& board, const GameRuleFlags& flags, const Game& game) const {
    std::vector<Move> moves;
    int currentRow = indexOnBoard / 8;
    int currentCol = indexOnBoard % 8;

    // Generate all moves surrounding king
    for (int i = 0; i < 8; i++) {
        int newRow = currentRow + m_directions[i].first;
        int newCol = currentCol + m_directions[i].second;

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
    if (this->CanCastle(pieceType, true, board, flags, game)) {  // Kingside
        Move castlingMove;
        castlingMove.startSquare = indexOnBoard;
        castlingMove.targetSquare = indexOnBoard + 2;
        castlingMove.isCastling = true;
        castlingMove.rookStartSquare = (pieceType & Piece::White) ? ChessSquares::H1 : ChessSquares::H8;
        castlingMove.rookTargetSquare = castlingMove.rookStartSquare - 2;
        moves.push_back(castlingMove);
    }

    if (this->CanCastle(pieceType, false, board, flags, game)) {  // Queenside
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


bool PieceManager::CanCastle(int kingType, bool kingSide, const BoardState& board, const GameRuleFlags& flags, const Game& game) const {
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
        if (game.IsSquareAttackedSimple(kingStartSquare + i * direction, oppositeColor)) return false;
    }

    return true;
}

// Standalone function to update bitboards
void PieceManager::UpdateBitboards(PieceBitboards& bitboards, const Move& move, int currentPiece) {
    // Helper function to get a reference to the appropriate Bitboard
    auto getBitboard = [&](int pieceType) -> Bitboard& {
        switch (pieceType) {
        case Piece::WhitePawn: return bitboards.WhitePawns;
        case Piece::WhiteKnight: return bitboards.WhiteKnights;
        case Piece::WhiteBishop: return bitboards.WhiteBishops;
        case Piece::WhiteRook: return bitboards.WhiteRooks;
        case Piece::WhiteQueen: return bitboards.WhiteQueens;
        case Piece::WhiteKing: return bitboards.WhiteKing;
        case Piece::BlackPawn: return bitboards.BlackPawns;
        case Piece::BlackKnight: return bitboards.BlackKnights;
        case Piece::BlackBishop: return bitboards.BlackBishops;
        case Piece::BlackRook: return bitboards.BlackRooks;
        case Piece::BlackQueen: return bitboards.BlackQueens;
        case Piece::BlackKing: return bitboards.BlackKing;
        default:
            throw std::invalid_argument("Invalid piece type in getBitboard");
        }
        };

    // Get the bitboard for the current piece
    Bitboard& currentBitboard = getBitboard(currentPiece);

    // Update the bitboard for the moving piece
    currentBitboard.clear(move.startSquare);
    currentBitboard.set(move.targetSquare);

    // Create a mask for clearing the captured piece (if any)
    Bitboard::BitboardType captureMask = ~(1ULL << move.targetSquare);

    // Clear the captured piece from all other bitboards
    for (int pieceType : {Piece::WhitePawn, Piece::WhiteKnight, Piece::WhiteBishop, Piece::WhiteRook, Piece::WhiteQueen, Piece::WhiteKing,
        Piece::BlackPawn, Piece::BlackKnight, Piece::BlackBishop, Piece::BlackRook, Piece::BlackQueen, Piece::BlackKing}) {
        if (pieceType != currentPiece) {
            getBitboard(pieceType).board &= captureMask;
        }
    }

    // Handle special moves
    if (move.isEnPassant) {
        int capturedPawnSquare = move.targetSquare + ((currentPiece & Piece::White) ? 8 : -8);
        getBitboard((currentPiece & Piece::White) ? Piece::BlackPawn : Piece::WhitePawn).clear(capturedPawnSquare);
    }
    else if (move.isCastling) {
        // Update rook position for castling
        Bitboard& rookBitboard = getBitboard((currentPiece & Piece::White) ? Piece::WhiteRook : Piece::BlackRook);
        rookBitboard.clear(move.rookStartSquare);
        rookBitboard.set(move.rookTargetSquare);
    }

    // Handle pawn promotion
    if (move.isPromotion) {
        currentBitboard.clear(move.targetSquare);  // Remove pawn from its bitboard
        getBitboard(move.promotionPiece).set(move.targetSquare);  // Add the promoted piece to its bitboard
    }
}

int PieceManager::FindKingLocation(int kingColor, const BoardState& board) const {
    int targetKing = (kingColor & Piece::White) ? Piece::WhiteKing : Piece::BlackKing;

    for (int i = 0; i < TOTAL_SQUARES; i++) {
        if (board[i] == targetKing) {
            return i;
        }
    }

    return -1;  // King not found (shouldn't happen in a valid chess position)
}

void PieceManager::ClearAllBitboards(PieceBitboards& bitboards) {
    // Implement the ClearAllBitboards function here
    // This function was declared in the header but not implemented in the original code
    bitboards.WhitePawns.board = 0;
    bitboards.WhiteKnights.board = 0;
    bitboards.WhiteBishops.board = 0;
    bitboards.WhiteRooks.board = 0;
    bitboards.WhiteQueens.board = 0;
    bitboards.WhiteKing.board = 0;
    bitboards.BlackPawns.board = 0;
    bitboards.BlackKnights.board = 0;
    bitboards.BlackBishops.board = 0;
    bitboards.BlackRooks.board = 0;
    bitboards.BlackQueens.board = 0;
    bitboards.BlackKing.board = 0;
}