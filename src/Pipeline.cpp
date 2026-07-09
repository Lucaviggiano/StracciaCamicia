#include "Pipeline.hpp"
#include "FastKernel.hpp"
#include <iostream>
#include <thread>
#include <chrono> // Per misurare le performance
#include <algorithm>

// ==========================================
// 1. IL LAVORATORE (Eseguito in parallelo)
// ==========================================
void Pipeline::worker_task(uint64_t seed_start, uint64_t seed_end, uint16_t cutoff, WorkerResult* out_result) {
    
    // Il thread si fa semplicemente il suo ciclo for sul suo "fazzoletto" di seed assegnato.
    for (uint64_t current_seed = seed_start; current_seed < seed_end; ++current_seed) {
        
        // Lancia il simulatore
        GameResult res = FastKernel::playGame(current_seed, cutoff);
        
        // AGGIORNAMENTO TELEMETRIA
        out_result->total_turns += res.num_turns;
        out_result->max_turns = std::max(out_result->max_turns, res.num_turns);

        // Aggiorna i contatori locali (nessun mutex necessario!)
        if (res.status == GameStatus::P1_WINS) {
            out_result->p1_wins++;
        } 
        else if (res.status == GameStatus::P2_WINS) {
            out_result->p2_wins++;
        } 
        else if (res.status == GameStatus::CUTOFF_REACHED) {
            out_result->loop_seeds.push_back(current_seed);
        }
    }
}

// ==========================================
// 2. L'ORCHESTRATORE (Eseguito dal Main)
// ==========================================
void Pipeline::run_simulation(uint64_t start_seed, uint64_t num_games, uint16_t cutoff, int num_threads) {
    
    std::cout << "Avvio simulazione di " << num_games << " partite su " << num_threads << " thread...\n";
    
    // Faccio partire il cronometro
    auto start_time = std::chrono::high_resolution_clock::now();
 
    // Vettori per memorizzare i risultati locali e i thread fisici
    std::vector<WorkerResult> results(num_threads);
    std::vector<std::thread> threads;
    
    // Calcolo quante partite deve fare ogni thread
    uint64_t chunk_size = num_games / num_threads;
    uint64_t remainder = num_games % num_threads; // Se la divisione non è perfetta
    
    uint64_t current_start = start_seed;
 
    // Fase di MAP: Lanciamo i thread
    for (int i = 0; i < num_threads; ++i) {
        // L'ultimo thread si prende anche il resto delle partite
        uint64_t current_end = current_start + chunk_size + (i == num_threads - 1 ? remainder : 0);
        
        // Creiamo il thread. Passiamo la funzione da eseguire e i suoi parametri.
        // Attenzione al &results[i]: passiamo l'indirizzo di memoria del "foglio" di questo thread.
        threads.emplace_back(std::thread(worker_task, current_start, current_end, cutoff, &results[i]));
        
        current_start = current_end; // Aggiorno la partenza per il thread successivo
    }
 
    // Aspettiamo che tutti i thread finiscano il loro lavoro
    for (int i = 0; i < num_threads; ++i) {
        threads[i].join(); // Il main thread si mette in pausa qui finché il thread i-esimo non ha finito
    }
 
    // Fase di REDUCE: Aggreghiamo i risultati
    uint64_t total_p1_wins = 0;
    uint64_t total_p2_wins = 0;
    uint64_t global_total_turns = 0;
    uint16_t global_max_turns = 0;
    std::vector<uint64_t> all_loop_seeds;
 
    for (int i = 0; i < num_threads; ++i) {
        total_p1_wins += results[i].p1_wins;
        total_p2_wins += results[i].p2_wins;
        
        // Uniamo la telemetria
        global_total_turns += results[i].total_turns;
        global_max_turns = std::max(global_max_turns, results[i].max_turns);
        
        all_loop_seeds.insert(all_loop_seeds.end(), results[i].loop_seeds.begin(), results[i].loop_seeds.end());
    }
    
    // Fermo il cronometro
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    double throughput = (num_games / 1000000.0) / elapsed.count(); // Milioni di simulazioni al secondo

    // ==========================================
    // 3. REPORT FINALE
    // ==========================================
    std::cout << "--- SIMULAZIONE COMPLETATA ---\n";
    std::cout << "Tempo impiegato : " << elapsed.count() << " secondi\n";
    std::cout << "Throughput      : " << throughput << " Milioni di sim/sec\n";
    std::cout << "Vittorie P1     : " << total_p1_wins << "\n";
    std::cout << "Vittorie P2     : " << total_p2_wins << "\n";
    std::cout << "Turni medi/partita: " << (global_total_turns / (double)num_games) << "\n";
    std::cout << "Turni MAX toccati : " << global_max_turns << "\n";
    std::cout << "Partite in loop : " << all_loop_seeds.size() << " (Cutoff a " << cutoff << " turni)\n";

    // Se abbiamo trovato loop, stampiamo i primi 5 seed per permetterti di analizzarli dopo
    if (!all_loop_seeds.empty()) {
        std::cout << "\nSeed sospetti trovati! Esempi:\n";
        for (size_t i = 0; i < std::min((size_t)5, all_loop_seeds.size()); ++i) {
            std::cout << "- Seed: " << all_loop_seeds[i] << "\n";
        }
    }
}