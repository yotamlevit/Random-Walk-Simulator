#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>
#include <cstdint>
#include <cstdlib> // for getenv
#include <cstring>    // For strcmp
#include <sstream>    // For istringstream
#include <string>     // For string

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif


// === CONFIG ===
const int64_t default_num_simulations = 1'000'000;
const int64_t progress_interval = 1'000;
const int start_index_offset = 0;  // For partial runs
const std::string output_prefix = "data/thread_";  // Output: thread_0_output.csv, etc.
const int default_thread_count = 12;
int64_t num_simulations = default_num_simulations;

#define ALL_PREMMISIONS 0777
// ==============


void CreateDataDir() {
    // Check and create "data" directory if needed
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        #ifdef _WIN32
            _mkdir("data");
        #else
            mkdir("data", ALL_PREMMISIONS);
        #endif
        std::cout << "Created data directory.\n";
    }
}

int main(int argc, char* argv[]) {
    CreateDataDir();

    int thread_limit = default_thread_count;

    // Parse --threads N argument
    for (int i = 1; i < argc; ++i) {
        if ((std::strcmp(argv[i], "--threads") == 0 || std::strcmp(argv[i], "-t") == 0) && i + 1 < argc) {
            std::istringstream iss(argv[i + 1]);
            if (!(iss >> thread_limit) || thread_limit <= 0) {
                std::cerr << "Invalid thread count after --threads/-t\n";
                return 1;
            }
            ++i; // skip the value
        } else if ((std::strcmp(argv[i], "--walks") == 0 || std::strcmp(argv[i], "-w") == 0) && i + 1 < argc) {
            std::istringstream iss(argv[i + 1]);
            if (!(iss >> num_simulations) || num_simulations <= 0) {
                std::cerr << "Invalid number after --walks/-w\n";
                return 1;
            }
            ++i; // skip the value
        }
    }

    omp_set_num_threads(thread_limit);
    std::cout << "Using thread limit: " << thread_limit << "\n";

    auto start_time = std::chrono::steady_clock::now();

    int thread_count = 0;  // Will store actual number of threads used

    #pragma omp parallel
    {
        #pragma omp single
        thread_count = omp_get_num_threads();  // Set once from a single thread

        int thread_id = omp_get_thread_num();
        std::random_device rd;
        std::mt19937_64 gen(rd() ^ (thread_id + 1));  // 64-bit engine

        std::vector<std::pair<int64_t, uint64_t>> local_buffer;
        int64_t local_counter = 0;

        std::string filename = output_prefix + std::to_string(thread_id) + "_output.csv";

        #pragma omp critical
        std::cout << "[Thread " << thread_id << "] Started.\n";

        #pragma omp for schedule(dynamic, 1)
        for (int64_t i = 0; i < num_simulations; ++i) {
            int pos = 5;
            uint64_t steps = 0;
            uint64_t bit_buffer = 0;
            int bit_index = 64;

            while (pos > 0) {
                if (bit_index == 64) {
                    bit_buffer = gen();  // Generate 64 bits
                    bit_index = 0;
                }

                bool move_right = (bit_buffer >> bit_index) & 1;
                pos += move_right ? 1 : -1;

                ++bit_index;
                ++steps;
            }

            local_buffer.emplace_back(start_index_offset + i, steps);
            ++local_counter;

            if (local_counter % progress_interval == 0) {
                auto now = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsed = now - start_time;

                std::ofstream out(filename, std::ios::app);
                for (const auto& [idx, step] : local_buffer)
                    out << idx << "," << step << "\n";
                out.close();
                local_buffer.clear();

                #pragma omp critical
                std::cout << "[Thread " << thread_id << "] Wrote "
                          << local_counter << " results at "
                          << (elapsed.count() / 60.0) << " minutes.\n";
            }
        }

        // Final flush
        if (!local_buffer.empty()) {
            std::ofstream out(filename, std::ios::app);
            for (const auto& [idx, step] : local_buffer)
                out << idx << "," << step << "\n";
            out.close();
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> total = end_time - start_time;

    std::cout << "Finished all walks.\n";
    std::cout << "Used " << thread_count << " threads.\n";
    std::cout << "Total runtime: " << total.count() << " seconds\n";

    return 0;
}
