#include "GameManager.h"
#include "GameState.h"
#include <iostream>

GameManager::GameManager()
    : m_dragOffset{ 0, 0 } {
}

void GameManager::initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess Engine with raylib");
    m_board.InitializeBoard();
    m_board.LoadPieceTextures();

    // Register the UpdateChessPieces as an observer
    registerMoveObserver([this]() { m_board.UpdateChessPieces(); });
}

void GameManager::processInput() {
    auto& gameState = GameState::getInstance();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        int col = (int)(mousePos.x / SQUARE_SIZE);
        int row = (int)(mousePos.y / SQUARE_SIZE);
        int index = row * BOARD_SIZE + col;

        if (index >= 0 && index < TOTAL_SQUARES && gameState.board[index] != Piece::None) {
            gameState.selectedPieceIndex = index;
            m_dragOffset = { mousePos.x - (col * SQUARE_SIZE + SQUARE_SIZE / 2.0f),
                             mousePos.y - (row * SQUARE_SIZE + SQUARE_SIZE / 2.0f) };
            m_currentLegalMoves = m_game.GenerateLegalMoves(gameState.board[gameState.selectedPieceIndex], gameState.selectedPieceIndex);
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && gameState.selectedPieceIndex != -1) {
        Vector2 mousePos = GetMousePosition();
        int newCol = Clamp((int)(mousePos.x / SQUARE_SIZE), 0, BOARD_SIZE - 1);
        int newRow = Clamp((int)(mousePos.y / SQUARE_SIZE), 0, BOARD_SIZE - 1);
        int newIndex = newRow * BOARD_SIZE + newCol;

        bool isLegalMove = false;
        Move selectedMove;
        for (const auto& move : m_currentLegalMoves) {
            if (move.targetSquare == newIndex) {
                selectedMove = move;
                isLegalMove = true;
                break;
            }
        }

        if (newIndex != gameState.selectedPieceIndex && isLegalMove) {
            m_game.MakeMove(selectedMove, gameState.board[gameState.selectedPieceIndex]);
            m_board.UpdateGameFlags(gameState.selectedPieceIndex);
            uint64_t newHash = m_zobristHash.hash(gameState.board, gameState.moveCount % 2 == 0, gameState.gameFlags);
            gameState.positionHistory.push_back(newHash);
            
            // Notify observers after a move is made
            notifyMoveObservers();
        }

        gameState.selectedPieceIndex = -1;
        m_currentLegalMoves.clear();
    }
}

void GameManager::update() {
    if (m_game.BlackCheckmate() || m_game.WhiteCheckmate() || m_game.GameDrawStaleMate() ||
        m_game.GameDrawInsufficientMaterial() || m_game.GameDrawFiftyMove() || m_game.GameDrawThreefold()) {
        // Handle game over conditions
    }
}

void GameManager::render() {
    auto& gameState = GameState::getInstance();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    m_board.DrawChessBoard();

    if (gameState.selectedPieceIndex != -1) {
        m_board.DrawLegalMoveHighlights(m_currentLegalMoves);
    }

    m_board.DrawPieces(gameState.selectedPieceIndex);

    if (gameState.selectedPieceIndex != -1) {
        Vector2 mousePos = GetMousePosition();
        const auto& pieceTextures = gameState.pieceTextures;
        float scale = (float)SQUARE_SIZE / pieceTextures.at(gameState.board[gameState.selectedPieceIndex]).width * 0.8f;
        Vector2 centered = {
            mousePos.x - m_dragOffset.x - (pieceTextures.at(gameState.board[gameState.selectedPieceIndex]).width * scale) / 2,
            mousePos.y - m_dragOffset.y - (pieceTextures.at(gameState.board[gameState.selectedPieceIndex]).height * scale) / 2
        };
        DrawTextureEx(pieceTextures.at(gameState.board[gameState.selectedPieceIndex]), centered, 0.0f, scale, WHITE);
    }

    EndDrawing();
}

void GameManager::cleanup() {
    m_board.UnloadPieceTextures();
    CloseWindow();
}

void GameManager::run() {
    initialize();

    while (!WindowShouldClose()) {
        processInput();
        update();
        render();
    }

    cleanup();
}

void GameManager::registerMoveObserver(std::function<void()> observer) {
    m_moveObservers.push_back(observer);
}

void GameManager::notifyMoveObservers() {
    for (const auto& observer : m_moveObservers) {
        observer();
    }
}