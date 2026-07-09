#include "FastKernel.hpp"
#include <random>
#include <algorithm>
#include <iostream>
#include <cstring>

GameResult FastKernel::playGame(uint64_t seed, uint16_t cutoff) {
    
    // ==========================================
    // FASE A: SETUP E SHUFFLE (Boilerplate C++)
    // ==========================================
    
    // 1. Creiamo il mazzo "vergine" ordinato (12 attacchi, 4 bloccanti, 24 lisce)
    // Mappatura: 1=Asso, 2=Due, 3=Tre, 4=Quattro, 0=Liscia
    uint8_t mazzo_base[40] = {
        1,1,1,1, 2,2,2,2, 3,3,3,3, 4,4,4,4,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    // 2. Inizializziamo il generatore pseudo-casuale con il seed
    std::mt19937_64 rng(seed);
    std::shuffle(std::begin(mazzo_base), std::end(mazzo_base), rng);

    // 3. Istanziamo lo stato di gioco e distribuiamo le carte fisicamente
    GameState stato(seed);
    for(int i = 0; i < 20; ++i) {
        stato.deck_A[i] = mazzo_base[i];
        stato.deck_B[i] = mazzo_base[i + 20];
    }

    // ==========================================
    // FASE B: IL CICLO DI VITA
    // ==========================================
    
    return runGameLoop(stato, cutoff);
}

GameResult FastKernel::runGameLoop(GameState& stato, uint16_t cutoff) {
    while (stato.num_turn < cutoff) {
        stato.num_turn++;

        // TRUCCO HPC: Per non scrivere tutto due volte (se tocca ad A fai questo, se tocca a B fai quello),
        // creiamo dei "puntatori" e delle "reference" (alias) al mazzo di chi sta giocando ora.
        uint8_t* mazzo_attivo = (stato.turno == 0) ? stato.deck_A : stato.deck_B;
        uint8_t& head_attivo  = (stato.turno == 0) ? stato.head_A : stato.head_B;
        
        // TODO 1: Estrai la carta attuale (mazzo_attivo all'indice head_attivo)
        uint8_t carta_pescata = mazzo_attivo[head_attivo];
        // TODO 2: Fai avanzare la head_attivo usando la maschera circolare: 
        head_attivo = (head_attivo + 1) & BUFFER_MASK;
        
        // TODO 3: Metti la carta pescata sul tavolo (stato.table) all'indice stato.table_size
        // e incrementa table_size. Decrementa cards_to_play.
        stato.table[stato.table_size] = carta_pescata;
        stato.table_size++;
        stato.cards_to_play--;

        // [Assumiamo che tu abbia già estratto la carta, 
// messa sul tavolo, e fatto: stato.cards_to_play--;]

if (carta_pescata == 4) {
    // BLOCCO: Il 4 ferma tutto istantaneamente.
    // L'attacco è annullato e si torna alla normale alternanza.
    stato.current_attacker = -1;
    stato.cards_to_play = 1;
    stato.turno = 1 - stato.turno; // Passo il turno all'avversario
} 
else if (carta_pescata > 0) {
    // ATTACCO (1, 2 o 3): Innesca o ribalta un attacco immediatamente!
    // Chi ha tirato questa carta diventa il nuovo attaccante.
    stato.current_attacker = stato.turno;
    
    // Il numero di carte da tirare diventa esattamente il valore della carta (1, 2 o 3).
    stato.cards_to_play = carta_pescata;
    
    // Il turno passa all'avversario che ora deve difendersi.
    stato.turno = 1 - stato.turno;
} 
else {
    // LISCIA (0)
    // Non innesca nulla. Dobbiamo solo vedere se abbiamo finito il nostro "dovere".
    if (stato.cards_to_play == 0) {
        
        if (stato.current_attacker != -1) {
            // PRESA! C'era un attacco in corso e il difensore ha tirato solo lisce.
            uint8_t* mazzo_vincitore = (stato.current_attacker == 0) ? stato.deck_A : stato.deck_B;
            uint8_t& tail_vincitore  = (stato.current_attacker == 0) ? stato.tail_A : stato.tail_B;

            // Il tuo ciclo di svuotamento perfetto:
            for(int i = 0; i < stato.table_size; i++){
                mazzo_vincitore[tail_vincitore] = stato.table[i];
                tail_vincitore = (tail_vincitore + 1) & BUFFER_MASK;
            }

            // Reset stato per il nuovo ciclo
            stato.table_size = 0;
            stato.cards_to_play = 1;
            stato.turno = stato.current_attacker; // Riparte chi ha vinto la presa!
            stato.current_attacker = -1;          // Reset stato attacco
        } 
        else {
            // ALTERNANZA NORMALE: Nessun attacco, ognuno tirava 1 carta a testa.
            stato.turno = 1 - stato.turno;
            stato.cards_to_play = 1;
        }
    }
    // NOTA: Se cards_to_play è > 0, questo ramo if termina senza fare nulla.
    // Il turno NON cambia e al prossimo giro del while lo stesso giocatore
    // sfilerà un'altra carta per continuare a difendersi.
}

        
        // ==========================================
        // FASE C: CONTROLLO VITTORIA
        // ==========================================
        
        // Calcoliamo quante carte hanno in mano i giocatori.
        // La dimensione in un buffer circolare 2^N è sempre (tail - head) & maschera.
        uint8_t carte_A = (stato.tail_A - stato.head_A) & BUFFER_MASK;
        uint8_t carte_B = (stato.tail_B - stato.head_B) & BUFFER_MASK;

        // Se un giocatore arriva a 0 carte, la partita finisce.
        if (carte_A == 0) {
            // Ritorna il seed, i turni esatti impiegati, e la vittoria del Giocatore 2 (B)
            return GameResult(stato.seed, stato.num_turn, GameStatus::P2_WINS);
        }
        else if (carte_B == 0) {
            // Ritorna il seed, i turni esatti impiegati, e la vittoria del Giocatore 1 (A)
            return GameResult(stato.seed, stato.num_turn, GameStatus::P1_WINS);
        }
    }

    // Se il while finisce in modo naturale, significa che abbiamo superato il limite di turni
    return GameResult(stato.seed, cutoff, GameStatus::CUTOFF_REACHED);
}

// ==========================================
// KERNEL OTTIMIZZATO PER LA SIMULAZIONE MASSIVA
// - SplitMix64 al posto di mt19937_64 (stato 8B vs 2.5KB)
// - Variabili locali al posto di GameState (register-friendly)
// - Branch structure appiattita
// - Controllo vittoria solo sul giocatore che ha pescato
// - XOR toggle per il turno
// ==========================================
GameResult FastKernel::playGameFast(uint64_t seed, uint16_t cutoff) {
    // Setup mazzo e shuffle con SplitMix64
    uint8_t deck[40] = {
        1,1,1,1, 2,2,2,2, 3,3,3,3, 4,4,4,4,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    fast_shuffle(deck, 40, seed);

    // Stato di gioco interamente in variabili locali (register-friendly)
    uint8_t deck_A[BUFFER_SIZE];
    uint8_t deck_B[BUFFER_SIZE];
    uint8_t table[MAX_TABLE];

    memcpy(deck_A, deck, 20);
    memcpy(deck_B, deck + 20, 20);

    uint8_t head_A = 0, tail_A = 20;
    uint8_t head_B = 0, tail_B = 20;
    uint8_t table_size = 0;
    uint8_t turno = 1;            // B inizia
    uint8_t cards_to_play = 1;
    int8_t current_attacker = -1;
    uint16_t num_turn = 0;

    while (num_turn < cutoff) {
        num_turn++;

        // Chi pesca questo turno (prima che turno cambi)
        uint8_t drawer = turno;

        // Pesca carta dal mazzo attivo
        uint8_t carta_pescata;
        if (turno == 0) {
            carta_pescata = deck_A[head_A];
            head_A = (head_A + 1) & BUFFER_MASK;
        } else {
            carta_pescata = deck_B[head_B];
            head_B = (head_B + 1) & BUFFER_MASK;
        }

        table[table_size++] = carta_pescata;
        cards_to_play--;

        if (carta_pescata == 4) {
            // BLOCCO
            current_attacker = -1;
            cards_to_play = 1;
            turno ^= 1;
        } else if (carta_pescata > 0) {
            // ATTACCO (1, 2 o 3)
            current_attacker = turno;
            cards_to_play = carta_pescata;
            turno ^= 1;
        } else if (cards_to_play == 0) {
            // LISCIA e dovere esaurito
            if (current_attacker != -1) {
                // PRESA: carte dal tavolo vanno all'attaccante
                if (current_attacker == 0) {
                    for (uint8_t i = 0; i < table_size; i++) {
                        deck_A[tail_A] = table[i];
                        tail_A = (tail_A + 1) & BUFFER_MASK;
                    }
                } else {
                    for (uint8_t i = 0; i < table_size; i++) {
                        deck_B[tail_B] = table[i];
                        tail_B = (tail_B + 1) & BUFFER_MASK;
                    }
                }
                table_size = 0;
                turno = current_attacker;
                current_attacker = -1;
            } else {
                // ALTERNANZA NORMALE
                turno ^= 1;
            }
            cards_to_play = 1;
        }
        // Se cards_to_play > 0 e liscia: non fare nulla, stesso giocatore continua

        // VITTORIA: solo chi ha pescato può esaurire le carte
        if (drawer == 0) {
            if (((tail_A - head_A) & BUFFER_MASK) == 0)
                return GameResult(seed, num_turn, GameStatus::P2_WINS);
        } else {
            if (((tail_B - head_B) & BUFFER_MASK) == 0)
                return GameResult(seed, num_turn, GameStatus::P1_WINS);
        }
    }

    return GameResult(seed, cutoff, GameStatus::CUTOFF_REACHED);
}

void FastKernel::traceGame(uint64_t seed) {
    // 1. Setup con SplitMix64 (coerente con playGameFast)
    uint8_t mazzo_base[40] = {
        1,1,1,1, 2,2,2,2, 3,3,3,3, 4,4,4,4,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    fast_shuffle(mazzo_base, 40, seed);
    
    GameState stato(seed);
    for(int i = 0; i < 20; ++i) {
        stato.deck_A[i] = mazzo_base[i];
        stato.deck_B[i] = mazzo_base[i + 20];
    }

    // Intestazione del file CSV
    std::cout << "Turno,Carte_A,Carte_B,Dim_Tavolo,Carta_Pescata,Blocco\n";

    while (stato.num_turn < 10000) {
        stato.num_turn++;
        
        uint8_t* mazzo_attivo = (stato.turno == 0) ? stato.deck_A : stato.deck_B;
        uint8_t& head_attivo  = (stato.turno == 0) ? stato.head_A : stato.head_B;
        
        uint8_t carta_pescata = mazzo_attivo[head_attivo];
        head_attivo = (head_attivo + 1) & BUFFER_MASK;
        
        stato.table[stato.table_size] = carta_pescata;
        stato.table_size++;
        stato.cards_to_play--;

        // Stampa lo stato CORRENTE nel CSV
        uint8_t carte_A = (stato.tail_A - stato.head_A) & BUFFER_MASK;
        uint8_t carte_B = (stato.tail_B - stato.head_B) & BUFFER_MASK;
        int is_blocco = (carta_pescata == 4 && stato.current_attacker != -1) ? 1 : 0;
        std::cout << stato.num_turn << "," << (int)carte_A << "," << (int)carte_B << "," << (int)stato.table_size << "," << (int)carta_pescata << "," << is_blocco << "\n";

        if (carta_pescata == 4) {
            stato.current_attacker = -1;
            stato.cards_to_play = 1;
            stato.turno = 1 - stato.turno;
        } 
        else if (carta_pescata > 0) {
            stato.current_attacker = stato.turno;
            stato.cards_to_play = carta_pescata;
            stato.turno = 1 - stato.turno;
        } 
        else {
            if (stato.cards_to_play == 0) {
                if (stato.current_attacker != -1) {
                    uint8_t* mazzo_vincitore = (stato.current_attacker == 0) ? stato.deck_A : stato.deck_B;
                    uint8_t& tail_vincitore  = (stato.current_attacker == 0) ? stato.tail_A : stato.tail_B;
        
                    for(int i = 0; i < stato.table_size; i++){
                        mazzo_vincitore[tail_vincitore] = stato.table[i];
                        tail_vincitore = (tail_vincitore + 1) & BUFFER_MASK;
                    }
        
                    stato.table_size = 0;
                    stato.cards_to_play = 1;
                    stato.turno = stato.current_attacker;
                    stato.current_attacker = -1;
                } 
                else {
                    stato.turno = 1 - stato.turno;
                    stato.cards_to_play = 1;
                }
            }
        }

        carte_A = (stato.tail_A - stato.head_A) & BUFFER_MASK;
        carte_B = (stato.tail_B - stato.head_B) & BUFFER_MASK;
        if (carte_A == 0 || carte_B == 0) break;
    }
}