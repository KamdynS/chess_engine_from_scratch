#pragma once
#include "raylib.h"
#include "ChessBoard.h"
#include "Game.h"
#include "Pieces.h"
#include "ZobristHash.h"
#include <vector>

class GameManager {
public:
    GameManager();
    void run();

private:
    void initialize();
    void processInput();
    void update();
    void render();
    void cleanup();

    ChessBoard m_board;
    Game m_game;
    PieceManager m_pieceManager;
    ZobristHash m_zobristHash;
    std::vector<Move> m_currentLegalMoves;
    int m_selectedPieceIndex;
    Vector2 m_dragOffset;
    std::vector<uint64_t> m_positionHistory;
};