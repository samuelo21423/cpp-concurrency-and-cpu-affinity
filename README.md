# C++ Concurrency, Synchronisation and CPU Affinity Benchmarks

A systems-oriented C++ benchmarking project exploring how synchronisation strategies and CPU scheduling decisions affect correctness and performance in multithreaded workloads.

## Current Features

- Unsynchronised shared counter benchmark
- Mutex-protected counter using `std::mutex`
- Atomic counter using `std::atomic`
- Repeated benchmark trials
- Correctness tracking
- Average, minimum and maximum runtime measurement
- Modular benchmark runner and worker implementations

## Current Results

| Mode | Correct Runs | Average Runtime |
|------|-------------:|----------------:|
| Unsynchronised | 0/10 | ~1.9 ms |
| Mutex | 10/10 | ~366 ms |
| Atomic | 10/10 | ~174 ms |

> Results are from the current Release x64 build and will vary by machine and run.

The unsynchronised implementation contains a C++ data race and therefore has undefined behaviour. Its timings are recorded as an experimental observation rather than treated as a valid performance baseline.

## Project Structure

- `main.cpp` — benchmark orchestration
- `benchmark.cpp/.h` — reusable benchmark framework
- `workers.cpp/.h` — synchronisation strategies and counter access
- `config.h` — shared benchmark configuration

## Planned Work

- Structured benchmark result objects
- Cross-mode comparison metrics
- CSV export
- Command-line configuration
- CPU affinity experiments
- Hardware/build metadata
- Result visualisation
- x86 atomic instruction analysis
- Detailed methodology and limitations

## Build

Currently developed and tested with Visual Studio in Release x64 mode.
