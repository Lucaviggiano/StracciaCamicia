#pragma once
#include <cstdint>

// Usiamo costanti globali constexpr per il dimensionamento fisso
constexpr uint8_t BUFFER_SIZE = 64; 
constexpr uint8_t BUFFER_MASK = 63; // Maschera per il bitwise AND (64-1)
constexpr uint8_t MAX_TABLE   = 40; // Nessuna partita può avere più di 40 carte a terra

// Definizione chiara degli stati di uscita
enum class GameStatus : int8_t {
    PLAYING = 0,
    P1_WINS = 1,
    P2_WINS = 2,
    CUTOFF_REACHED = -2 //"sospetto loop"
};

// STRUTTURA DELLO STATO
// POD: nessuna allocazione dinamica, copiabile banalmente in memoria con memcpy
struct GameState {
    // 1. Array di grandi dimensioni (64 byte l'uno)
    uint8_t deck_A[BUFFER_SIZE];
    uint8_t deck_B[BUFFER_SIZE];
    uint8_t table[MAX_TABLE];

    // 2. Variabili multi-byte
    uint64_t seed;
    uint16_t num_turn;

    // 3. Puntatori logici (indici circolari) e contatori (1 byte)
    uint8_t head_A;
    uint8_t tail_A;
    uint8_t head_B;
    uint8_t tail_B;
    uint8_t table_size;
    
    // 4. Macchina a stati (1 byte)
    uint8_t turno;          // 0 per A, 1 per B
    uint8_t cards_to_play;  // 1, 2 o 3
    int8_t current_attacker;// 0 per A, 1 per B, -1 per Nessuno in attacco

    GameState(uint64_t start_seed) 
        : seed(start_seed), 
          num_turn(0), 
          head_A(0), tail_A(20), // A inizia con le prime 20 carte
          head_B(0), tail_B(20), // B inizia con le seconde 20 carte
          table_size(0), 
          turno(1),              // B inizia a giocare
          cards_to_play(1), 
          current_attacker(-1)   // Nessun attacco in corso
    {

    }
};

// STRUTTURA DEL RISULTATO
// serve solo per riportare i dati dalla simulazione all'host
struct GameResult {
    uint64_t seed;
    uint16_t num_turns;
    GameStatus status;

    GameResult() 
        : seed(0), num_turns(0), status(GameStatus::PLAYING) {}

    GameResult(uint64_t s, uint16_t t, GameStatus st) 
        : seed(s), num_turns(t), status(st) {}
};