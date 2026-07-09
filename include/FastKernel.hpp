#pragma once
#include "GameTypes.hpp"

struct FastKernel{
    static GameResult playGame(uint64_t seed, uint16_t cutoff);
    static GameResult playGameFast(uint64_t seed, uint16_t cutoff);
    static void traceGame(uint64_t seed);
    static GameResult runGameLoop(GameState& stato, uint16_t cutoff);
};
