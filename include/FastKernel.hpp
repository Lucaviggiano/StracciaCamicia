#pragma once
#include "GameTypes.hpp"

struct FastKernel{
    static GameResult playGame(uint64_t seed, uint16_t cutoff);
};
