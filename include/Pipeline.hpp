#pragma once
#include <vector>
#include <cstdint>

// Questa è la struttura che ogni Thread restituirà al Main Thread alla fine del suo lavoro.
// È il "foglio degli appunti" privato di cui parlavamo.
struct WorkerResult {
    uint64_t p1_wins = 0;
    uint64_t p2_wins = 0;
    
    // NUOVE VARIABILI PER LA TELEMETRIA
    uint64_t total_turns = 0; 
    uint16_t max_turns = 0;   
    
    // Un vector dinamico per salvare i seed che finiscono in cutoff.
    // Essendo locale al thread, non serve nessun mutex!
    std::vector<uint64_t> loop_seeds; 
};

// La classe statica che gestisce l'orchestra
class Pipeline {
public:
    // La funzione che il main chiamerà. 
    // Prende il seed di partenza, quante partite totali simulare, il cutoff e il numero di thread.
    static void run_simulation(uint64_t start_seed, uint64_t num_games, uint16_t cutoff, int num_threads);

private:
    // Questa è la funzione che verrà eseguita letteralmente da ogni singolo Thread.
    // Riceve il proprio range di competenza (da seed_start a seed_end)
    // e un puntatore al proprio WorkerResult dove scrivere i dati.
    static void worker_task(uint64_t seed_start, uint64_t seed_end, uint16_t cutoff, WorkerResult* out_result);
};