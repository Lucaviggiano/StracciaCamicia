#include <iostream>
#include <string>
#include <thread>
#include "Pipeline.hpp"

int main(int argc, char* argv[]) {
    // Valori di default
    uint64_t num_games = 1000000; // 1 Milione di partite di default
    uint16_t cutoff = 10000;      // 10.000 turni di default
    
    // Rileva automaticamente il numero massimo di thread hardware del tuo processore
    int num_threads = std::thread::hardware_concurrency(); 
    if (num_threads <= 0) num_threads = 1; // Fallback di sicurezza

    // Parsing degli argomenti da riga di comando (opzionali)
    try {
        if (argc > 1) num_games = std::stoull(argv[1]);
        if (argc > 2) cutoff = std::stoul(argv[2]);
        if (argc > 3) num_threads = std::stoi(argv[3]);
    } catch (const std::exception& e) {
        std::cerr << "Errore nel formato degli argomenti. Uso: ./StracciaCamicia [num_partite] [cutoff] [num_thread]\n";
        return 1;
    }

    std::cout << "====================================\n";
    std::cout << "   STRACCIACAMICIA - HPC SIMULATOR  \n";
    std::cout << "====================================\n";
    std::cout << "Partite da simulare : " << num_games << "\n";
    std::cout << "Limite turni (Cutoff): " << cutoff << "\n";
    std::cout << "Thread hardware     : " << num_threads << "\n\n";

    // Decidiamo di partire dal seed 1 per la prima tranche
    uint64_t start_seed = 1;
    
    // Lanciamo l'orchestratore!
    Pipeline::run_simulation(start_seed, num_games, cutoff, num_threads);

    return 0;
}