# Random-Walk-Simulator

**Random Walk Simulator** is a high-performance, parallelized C++ program for simulating millions of random walks using OpenMP. Designed for scalability and speed, it outputs step statistics for each walk into per-thread CSV files for easy analysis.

---

## Features

- ⚡ **High Performance:** Efficient use of multi-threading via OpenMP.
- 🧮 **Massive Scale:** Simulates millions of random walks.
- 📝 **Per-Thread Output:** Each thread writes to its own output file.
- 🔎 **Progress Reporting:** Console progress updates per thread.
- 🔧 **Configurable:** Easily adjust number of walks, threads, and output settings.

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
