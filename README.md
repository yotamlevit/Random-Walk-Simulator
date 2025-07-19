# Random Walk Simulator

**Random Walk Simulator** is a high-performance, parallelized C++ program for simulating millions of random walks using OpenMP. Designed for scalability and speed, it outputs step statistics for each walk into per-thread CSV files for easy analysis.

---

## Features

- ⚡ **High Performance:** Efficient use of multi-threading via OpenMP.
- 🧮 **Massive Scale:** Simulates millions of random walks.
- 📝 **Per-Thread Output:** Each thread writes to its own output file.
- 🔎 **Progress Reporting:** Console progress updates per thread.
- 🔧 **Configurable:** Easily adjust number of walks, threads, and output settings.
- 🎲 **Advanced Random Number Generation:** Each thread uses an independent, high-quality 64-bit Mersenne Twister engine with efficient bit-buffering for maximal speed and statistical quality.

---

## Technical Details: Random Number Generation

This simulator uses a **64-bit Mersenne Twister engine** (`std::mt19937_64`) to generate high-quality random numbers.  
Each thread creates its own random engine, seeded with a mix of `std::random_device` and the thread ID for independence between threads.

Instead of generating a new random number for every step, the simulator **buffers 64 random bits at once**. Each bit determines the next move (left or right), making the process much faster by reducing calls to the random engine.

**Advantages:**

- **High Quality:** `std::mt19937_64` provides a long period and excellent randomness properties, avoiding biases that simpler RNGs might introduce.
- **Thread-Safe:** Each thread maintains its own random engine, eliminating contention or repeated results.
- **Efficient:** By buffering 64 bits at a time, the program dramatically reduces overhead from random number generation—critical for massive simulations.

**Why not use rand() or a simpler approach?**

- Standard `rand()` has poor randomness quality and a short period, leading to patterns and bias in large simulations.
- Using per-thread random engines and bit-buffering maximizes both performance and statistical reliability.

---

## Technical Details: Parallelization

To maximize efficiency and scalability, the simulator assigns **one random walk per job** in the parallel loop, rather than batching multiple walks together. This fine-grained approach has several benefits:

- **Dynamic Load Balancing:** By assigning a single walk per job, OpenMP can dynamically distribute work among threads, preventing idle threads and making sure the workload is balanced—even if some walks take much longer than others.
- **Scalability:** Fine-grained jobs scale better as the number of threads or simulations increases.
- **Responsiveness:** Progress reporting and data flushing are more frequent and granular, providing better real-time feedback.

This strategy is especially effective for workloads with unpredictable or highly variable processing times per simulation.

---

## Usage

### Prerequisites

- C++17 or later
- OpenMP support (e.g., GCC, Clang, or MSVC)

### Build

```bash
g++ -std=c++17 -O3 -fopenmp random_walk.cpp -o random_walk_simulator
```

### Run

```bash
./random_walk_simulator
```

You can control the thread count by setting the `OMP_NUM_THREADS` environment variable:

```bash
export OMP_NUM_THREADS=8
./random_walk_simulator
```

---

## Output

Each thread produces a CSV file named like `thread_0_output.csv`, `thread_1_output.csv`, etc.  
Each line:  
```
simulation_index,steps_taken
```

---

## Configuration

You can change the following constants in the source file for different behaviors:

- `num_simulations` – Number of random walks to simulate
- `progress_interval` – Interval for progress reports and output flushes
- `default_thread_count` – Default number of OpenMP threads

---

## Example

Output sample (`thread_0_output.csv`):

```
0,14
1,22
2,7
...
```

Console output:

```
Using default thread limit: 12
[Thread 0] Started.
[Thread 1] Started.
...
[Thread 0] Wrote 1000 results at 0.12 minutes.
...
Finished all walks.
Used 12 threads.
Total runtime: 45.23 seconds
```

---

## License

[MIT License](LICENSE) (add your license here)

---

## Acknowledgments

- [OpenMP](https://www.openmp.org/)
- Inspired by classic random walk algorithms
