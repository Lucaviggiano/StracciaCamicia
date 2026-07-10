#include "CycleInspector.hpp"
#include "FastKernel.hpp"
#include <iostream>
#include <vector>

bool CycleInspector::statesEqual(const GameState& s1, const GameState& s2) {
    if (s1.turno != s2.turno) return false;

    uint8_t sizeA1 = (s1.tail_A - s1.head_A) & BUFFER_MASK;
    uint8_t sizeA2 = (s2.tail_A - s2.head_A) & BUFFER_MASK;
    if (sizeA1 != sizeA2) return false;

    uint8_t sizeB1 = (s1.tail_B - s1.head_B) & BUFFER_MASK;
    uint8_t sizeB2 = (s2.tail_B - s2.head_B) & BUFFER_MASK;
    if (sizeB1 != sizeB2) return false;

    for (uint8_t i = 0; i < sizeA1; ++i) {
        if (s1.deck_A[(s1.head_A + i) & BUFFER_MASK] != s2.deck_A[(s2.head_A + i) & BUFFER_MASK])
            return false;
    }
    for (uint8_t i = 0; i < sizeB1; ++i) {
        if (s1.deck_B[(s1.head_B + i) & BUFFER_MASK] != s2.deck_B[(s2.head_B + i) & BUFFER_MASK])
            return false;
    }
    return true;
}

bool CycleInspector::advanceToNextMacroState(GameState& stato) {
    // A macro state is reached when the table becomes empty (after the first turn).
    do {
        stato.num_turn++;
        uint8_t drawer = stato.turno;

        uint8_t carta_pescata;
        if (stato.turno == 0) {
            carta_pescata = stato.deck_A[stato.head_A];
            stato.head_A = (stato.head_A + 1) & BUFFER_MASK;
        } else {
            carta_pescata = stato.deck_B[stato.head_B];
            stato.head_B = (stato.head_B + 1) & BUFFER_MASK;
        }

        stato.table[stato.table_size++] = carta_pescata;
        stato.cards_to_play--;

        if (carta_pescata > 0) {
            stato.current_attacker = stato.turno;
            stato.cards_to_play = carta_pescata;
            stato.turno ^= 1;
        } else if (stato.cards_to_play == 0) {
            if (stato.current_attacker != -1) {
                if (stato.current_attacker == 0) {
                    for (uint8_t i = 0; i < stato.table_size; i++) {
                        stato.deck_A[stato.tail_A] = stato.table[i];
                        stato.tail_A = (stato.tail_A + 1) & BUFFER_MASK;
                    }
                } else {
                    for (uint8_t i = 0; i < stato.table_size; i++) {
                        stato.deck_B[stato.tail_B] = stato.table[i];
                        stato.tail_B = (stato.tail_B + 1) & BUFFER_MASK;
                    }
                }
                stato.table_size = 0;
                stato.turno = stato.current_attacker;
                stato.current_attacker = -1;
            } else {
                stato.turno ^= 1;
            }
            stato.cards_to_play = 1;
        }

        // Check if game ended
        if (drawer == 0 && (((stato.tail_A - stato.head_A) & BUFFER_MASK) == 0)) return false;
        if (drawer == 1 && (((stato.tail_B - stato.head_B) & BUFFER_MASK) == 0)) return false;

    } while (stato.table_size != 0);

    return true;
}

void CycleInspector::analyzeSeed(uint64_t seed) {
    std::cout << "Analisi ciclo per il seed: " << seed << std::endl;

    // Setup initial state identically to FastKernel
    uint8_t mazzo_base[40] = {
        1,1,1,1, 2,2,2,2, 3,3,3,3, 0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    fast_shuffle(mazzo_base, 40, seed);
    
    GameState startState(seed);
    for(int i = 0; i < 20; ++i) {
        startState.deck_A[i] = mazzo_base[i];
        startState.deck_B[i] = mazzo_base[i + 20];
    }
    
    GameState tortoise = startState;
    GameState hare = startState;
    
    bool tortoiseActive = true;
    bool hareActive = true;
    
    uint64_t steps = 0;

    // Phase 1: Finding intersection point
    while (true) {
        // Tortoise moves 1 step
        tortoiseActive = advanceToNextMacroState(tortoise);
        if (!tortoiseActive) break;
        
        // Hare moves 2 steps
        hareActive = advanceToNextMacroState(hare);
        if (!hareActive) break;
        hareActive = advanceToNextMacroState(hare);
        if (!hareActive) break;
        
        steps++;

        if (statesEqual(tortoise, hare)) {
            // Cycle detected!
            
            // Phase 2: Find cycle start (mu)
            uint64_t mu = 0;
            tortoise = startState;
            while (!statesEqual(tortoise, hare)) {
                advanceToNextMacroState(tortoise);
                advanceToNextMacroState(hare);
                mu++;
            }
            
            // Phase 3: Find cycle length (lambda)
            uint64_t lambda = 1;
            hare = tortoise;
            advanceToNextMacroState(hare);
            while (!statesEqual(tortoise, hare)) {
                advanceToNextMacroState(hare);
                lambda++;
            }
            
            std::cout << "  -> CICLO TROVATO!\n";
            std::cout << "  -> Inizio ciclo (macro-stati): " << mu << "\n";
            std::cout << "  -> Lunghezza ciclo (macro-stati): " << lambda << "\n";
            
            // Calculate total turns involved in one cycle loop
            uint16_t turns_in_cycle = hare.num_turn - tortoise.num_turn;
            if(turns_in_cycle == 0) { // Safety check to print positive turns
                 // If the calculation gives negative or 0 due to overflow or something, 
                 // we re-run from tortoise for lambda macro-states.
                 GameState counter_state = tortoise;
                 uint32_t start_turn = counter_state.num_turn;
                 for(uint64_t i=0; i<lambda; i++) {
                     advanceToNextMacroState(counter_state);
                 }
                 turns_in_cycle = counter_state.num_turn - start_turn;
            }
            
            std::cout << "  -> Durata in turni del ciclo: " << turns_in_cycle << "\n";
            return;
        }
        
        // Anti-stall just in case
        if (steps > 1000000) {
            std::cout << "  -> Ricerca interrotta (troppi macro-stati esplorati senza chiudere un ciclo).\n";
            return;
        }
    }
    
    std::cout << "  -> Nessun ciclo! La partita si è conclusa.\n";
}
