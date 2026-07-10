#pragma once
#include "GameTypes.hpp"
#include <cstdint>

class CycleInspector {
public:
    static void analyzeSeed(uint64_t seed);
    
private:
    static bool statesEqual(const GameState& s1, const GameState& s2);
    static bool advanceToNextMacroState(GameState& stato);
};
