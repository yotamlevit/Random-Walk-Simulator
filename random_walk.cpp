#include <iostream>
#include <vector>
#include <random>
#include <omp.h>
#include <chrono>
#include <fstream>
#include <cstdint>

int main() {
    const int num_simulations = 1'000'000;
    std::vector<uint64_t> steps_to_zero(num_simulations);

    auto start = std::chrono::steady_clock::now();  // Start timer

    #pragma omp parallel
    {
        std::random_device rd;
        std::mt19937 gen(rd() ^ omp_get_thread_num()); // thread-safe RNG
        std::uniform_real_distribution<> dis(0.0, 1.0);

        #pragma omp for
        for (int i = 0; i < num_simulations; ++i) {
            int pos = 5;
            uint64_t steps = 0;
            while (pos > 0) {
                double rnd = dis(gen);
                pos += (rnd < 0.5) ? -1 : 1;
                ++steps;
            }
            steps_to_zero[i] = steps;

            // Print progress every 10,000 walks
            if (i % 10'000 == 0) {
                auto now = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsed = now - start;
                #pragma omp critical
                {
                    std::cout << "[Progress] Walk " << i
                              << " completed at " << (elapsed.count() / 60.0)
                              << " minutes.\n";
                }
            }
        }
    }

    auto end = std::chrono::steady_clock::now();  // End timer
    std::chrono::duration<double> total_elapsed = end - start;
    std::cout << "Total runtime: " << total_elapsed.count() << " seconds\n";
    std::cout << "Used " << omp_get_max_threads() << " threads.\n";

    // Save results to file
    std::ofstream out_file("random_walk_results.csv");
    out_file << "walk_index,steps\n";
    for (int i = 0; i < num_simulations; ++i) {
        out_file << i << "," << steps_to_zero[i] << "\n";
    }
    out_file.close();
    std::cout << "Results written to random_walk_results.csv\n";

    // Compute and print average
    uint64_t total_steps = 0;
    for (auto steps : steps_to_zero) total_steps += steps;
    std::cout << "Average steps to reach 0: "
              << (double)total_steps / num_simulations << "\n";

    return 0;
}
