#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <vector>
#include "Pipeline.hpp"
#include "FastKernel.hpp"
#include "CycleInspector.hpp"

void playCustomGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Impossibile aprire il file.\n";
        return;
    }

    std::string line;
    std::string mazzo_str;
    std::string nome1, nome2;

    while (std::getline(file, line)) {
        if (line.find("Mazzo : ") == 0) {
            mazzo_str = line.substr(8);
            mazzo_str.erase(std::remove_if(mazzo_str.begin(), mazzo_str.end(), ::isspace), mazzo_str.end());
        } else if (line.find("Giocatori ") == 0) {
            std::stringstream ss(line.substr(10));
            ss >> nome1 >> nome2;
        }
    }

    if (mazzo_str.length() < 40) {
        std::cerr << "Mazzo non valido, carte insufficienti.\n";
        return;
    }
    if (nome1.empty() || nome2.empty()) {
        std::cerr << "Nomi giocatori non validi.\n";
        return;
    }

    GameState stato(0);
    stato.turno = 0; // Inizia Nome 1

    for (int i = 0; i < 20; ++i) {
        stato.deck_A[i] = mazzo_str[i] - '0';
        stato.deck_B[i] = mazzo_str[i + 20] - '0';
    }

    // Now play the game
    GameResult res = FastKernel::runGameLoop(stato, 10000); // 10000 is the default cutoff
    
    std::string vincitore = (res.status == GameStatus::P1_WINS) ? nome1 : ((res.status == GameStatus::P2_WINS) ? nome2 : "Nessuno (Cutoff)");
    std::cout << "Vincitore : " << vincitore << ", Nround = " << res.num_turns << "\n";
}

int main(int argc, char* argv[]) {
    // Gestione comando di salvataggio mazzo da seed: ./StracciaCamicia save [seed] [nome_file.txt]
    if (argc == 4 && std::string(argv[1]) == "save") {
        try {
            uint64_t seed = std::stoull(argv[2]);
            std::string filename = argv[3];

            uint8_t mazzo_base[40] = {
                1,1,1,1, 2,2,2,2, 3,3,3,3, 4,4,4,4,
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
            };

            fast_shuffle(mazzo_base, 40, seed);

            std::ofstream out(filename);
            if (!out.is_open()) {
                std::cerr << "Impossibile creare il file: " << filename << "\n";
                return 1;
            }

            out << "Mazzo : ";
            for (int i = 0; i < 40; ++i) {
                out << (int)mazzo_base[i];
            }
            out << "\nGiocatori A B\n";
            out.close();
            std::cout << "Mazzo salvato correttamente in " << filename << "\n";
            return 0;
        } catch (...) {
            std::cerr << "Errore nella generazione o scrittura del mazzo.\n";
            return 1;
        }
    }

    // Gestione comando ispezione: ./StracciaCamicia inspect [file_sospetti.txt] [cutoff]
    if (argc == 4 && std::string(argv[1]) == "inspect") {
        std::string filename = argv[2];
        uint16_t cutoff = std::stoul(argv[3]);
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Impossibile aprire il file dei sospetti: " << filename << "\n";
            return 1;
        }

        std::string line;
        std::vector<uint64_t> seeds;
        while (std::getline(file, line)) {
            if (line.find("- Seed: ") != std::string::npos) {
                size_t pos = line.find("- Seed: ") + 8;
                seeds.push_back(std::stoull(line.substr(pos)));
            }
        }
        
        std::cout << "Trovati " << seeds.size() << " seed sospetti nel file. Inizio ispezione con cutoff " << cutoff << "...\n";
        
        for (uint64_t s : seeds) {
            std::cout << "-------------------------------------\n";
            std::cout << "Simulo seed: " << s << " ...\n";
            GameResult res = FastKernel::playGameFast(s, cutoff);
            if (res.status == GameStatus::CUTOFF_REACHED) {
                std::cout << "-> CUTOFF RAGGIUNTO (" << cutoff << " turni).\n";
                CycleInspector::analyzeSeed(s);
            } else {
                std::cout << "-> Partita terminata in " << res.num_turns << " turni (vittoria, no ciclo infinito).\n";
            }
        }
        return 0;
    }

    // Modalità BATCH: ./StracciaCamicia batch [file.txt]
    if (argc == 3 && std::string(argv[1]) == "batch") {
        std::string filename = argv[2];
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Impossibile aprire il file: " << filename << "\n";
            return 1;
        }

        std::string line;
        uint64_t count = 0;
        uint64_t loop_count = 0;
        
        std::cout << "Analisi Batch in corso sul file: " << filename << "\n";
        
        while (std::getline(file, line)) {
            // Ignora righe vuote o malformate
            if (line.empty() || line.length() < 40) continue;
            
            GameState stato(0);
            stato.turno = 0; // Inizia il Giocatore A
            
            for (int i = 0; i < 20; ++i) {
                stato.deck_A[i] = line[i] - '0';
                stato.deck_B[i] = line[i + 20] - '0';
            }
            
            // Runnamo con cutoff a 10000 (più che sufficiente per validare il loop)
            GameResult res = FastKernel::runGameLoop(stato, 10000);
            
            if (res.status == GameStatus::CUTOFF_REACHED) {
                std::cout << "LOOP TROVATO! Mazzo: " << line << " (sopravvissuto a " << res.num_turns << " turni)\n";
                loop_count++;
            }
            
            count++;
            if (count % 5000 == 0) {
                std::cout << "Processate " << count << " combinazioni...\n";
            }
        }
        
        std::cout << "Analisi completata! Testate " << count << " varianti.\n";
        std::cout << "Loop totali sopravvissuti: " << loop_count << "\n";
        return 0;
    }

    // Se c'è solo un argomento (es. ./StracciaCamicia 424242), facciamo il trace o parse file
    if (argc == 2) {
        std::ifstream file(argv[1]);
        if (file.good()) {
            file.close();
            playCustomGame(argv[1]);
            return 0;
        }

        try {
            uint64_t seed = std::stoull(argv[1]);
            FastKernel::traceGame(seed);
        } catch (...) {
            std::cerr << "Errore nel parsing del seed per il trace.\n";
            return 1;
        }
        return 0;
    }

    // Valori di default
    uint64_t num_games = 1000000; // 1 Milione di partite di default
    uint16_t cutoff = 10000;      // 10.000 turni di default
    
    // Rileva automaticamente il numero massimo di thread hardware del tuo processore
    int num_threads = std::thread::hardware_concurrency(); 
    if (num_threads <= 0) num_threads = 1; // Fallback di sicurezza

    // Parsing degli argomenti da riga di comando per la simulazione standard
    try {
        if (argc > 1) num_games = std::stoull(argv[1]);
        if (argc > 2) cutoff = std::stoul(argv[2]);
        if (argc > 3) num_threads = std::stoi(argv[3]);
    } catch (const std::exception& e) {
        std::cerr << "Errore nel formato degli argomenti. Uso:\n"
                  << "  Simulazione: ./StracciaCamicia [num_partite] [cutoff] [num_thread]\n"
                  << "  Tracciamento: ./StracciaCamicia [seed]\n";
        return 1;
    }

    // Generiamo un seed di partenza dinamico combinando random_device e clock di sistema
    std::random_device rd;
    uint64_t start_seed = (static_cast<uint64_t>(rd()) << 32) | rd();
    start_seed ^= std::chrono::high_resolution_clock::now().time_since_epoch().count();

    std::cout << "====================================\n";
    std::cout << "   STRACCIACAMICIA - HPC SIMULATOR (with 4 rule)\n";
    std::cout << "====================================\n";
    std::cout << "Partite da simulare : " << num_games << "\n";
    std::cout << "Limite turni (Cutoff): " << cutoff << "\n";
    std::cout << "Thread hardware     : " << num_threads << "\n";
    std::cout << "Seed di partenza    : " << start_seed << "\n\n";
    
    // Lanciamo l'orchestratore!
    Pipeline::run_simulation(start_seed, num_games, cutoff, num_threads);

    return 0;
}