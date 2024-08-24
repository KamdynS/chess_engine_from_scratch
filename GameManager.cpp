#include "GameManager.h"
#include <iostream>

GameManager::GameManager()
    : m_selectedPieceIndex(-1), m_dragOffset{ 0, 0 } {
}

void GameManager::initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess Engine with raylib");
    m_board.InitializeBoard();
    m_board.LoadPieceTextures();
}

void GameManager::processInput() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        int col = (int)(mousePos.x / SQUARE_SIZE);
        int row = (int)(mousePos.y / SQUARE_SIZE);
        int index = row * BOARD_SIZE + col;

        if (index >= 0 && index < TOTAL_SQUARES && m_board.GetBoardState()[index] != Piece::None) {
            m_selectedPieceIndex = index;
            m_dragOffset = { mousePos.x - (col * SQUARE_SIZE + SQUARE_SIZE / 2.0f),
                             mousePos.y - (row * SQUARE_SIZE + SQUARE_SIZE / 2.0f) };
            m_currentLegalMoves = m_game.GenerateLegalMoves(m_board.GetBoardState()[m_selectedPieceIndex], m_selectedPieceIndex);
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && m_selectedPieceIndex != -1) {
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

        if (newIndex != m_selectedPieceIndex && isLegalMove) {
            m_board.MakeMove(selectedMove, m_board.GetBoardState()[m_selectedPieceIndex]);
            m_board.UpdateGameFlags(m_selectedPieceIndex);
            m_pieceManager.UpdateBitboards(m_board.GetBitboards(), selectedMove, m_board.GetBoardState()[m_selectedPieceIndex]);
            uint64_t newHash = m_zobristHash.hash(m_board.GetBoardState(), m_game.GetMoveCount() % 2 == 0, m_board.GetGameFlags());
            m_positionHistory.push_back(newHash);
            m_game.IncrementMoveCount();
        }

        m_selectedPieceIndex = -1;
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
    BeginDrawing();
    ClearBackground(RAYWHITE);

    m_board.DrawChessBoard();

    if (m_selectedPieceIndex != -1) {
        m_board.DrawLegalMoveHighlights(m_currentLegalMoves);
    }

    m_board.DrawPieces(m_selectedPieceIndex);

    if (m_selectedPieceIndex != -1) {
        Vector2 mousePos = GetMousePosition();
        const auto& pieceTextures = m_board.GetPieceTextures();
        float scale = (float)SQUARE_SIZE / pieceTextures.at(m_board.GetBoardState()[m_selectedPieceIndex]).width * 0.8f;
        Vector2 centered = {
            mousePos.x - m_dragOffset.x - (pieceTextures.at(m_board.GetBoardState()[m_selectedPieceIndex]).width * scale) / 2,
            mousePos.y - m_dragOffset.y - (pieceTextures.at(m_board.GetBoardState()[m_selectedPieceIndex]).height * scale) / 2
        };
        DrawTextureEx(pieceTextures.at(m_board.GetBoardState()[m_selectedPieceIndex]), centered, 0.0f, scale, WHITE);
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