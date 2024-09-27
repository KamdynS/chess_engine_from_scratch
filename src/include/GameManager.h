#pragma once
#include "raylib.h"
#include "ChessBoard.h"
#include "Game.h"
#include "Pieces.h"
#include "ZobristHash.h"
#include <vector>
#include <functional>

class GameManager {
public:
    GameManager();
    void run();

    // Observer pattern
    void registerMoveObserver(std::function<void()> observer);

private:
    void initialize();
    void processInput();
    void update();
    void render();
    void cleanup();
    void notifyMoveObservers();

    ChessBoard m_board;
    Game m_game;
    ZobristHash m_zobristHash;
    std::vector<Move> m_currentLegalMoves;
    Vector2 m_dragOffset;
    std::vector<std::function<void()>> m_moveObservers;
};