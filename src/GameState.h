#pragma once

#include "CommonComponents.h"
#include "Bitboard.h"
#include <vector>
#include <unordered_map>
#include "raylib.h"


class GameState {
public:
    static GameState& getInstance() {
        static GameState instance;
        return instance;
    }

    BoardState board;
    PieceBitboards bitboards;
    GameRuleFlags gameFlags;
    std::unordered_map<int, Texture2D> pieceTextures;
    int moveCount;
    std::vector<uint64_t> positionHistory;
    int selectedPieceIndex;

    // Add any other shared state variables here

private:
    GameState() 
    : board(),
      bitboards(),
      gameFlags(),
      pieceTextures(),
      moveCount(1),
     positionHistory(),
    selectedPieceIndex(-1) 
    {} 

    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;
};