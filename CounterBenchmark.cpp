#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <mutex>
#include <atomic>
#include <thread>

constexpr long IterationsPerThread = 10'000'000;
constexpr int NumberOfRuns = 10;

// Shared state intentionally left unsynchronised.
// This allows the benchmark to expose lost updates caused by a race condition.


// ==================== Shared State ====================

long count = 0;
std::mutex counterMutex;
std::atomic<long> atomicCount = 0;


// ==================== Worker Functions ====================

void worker()
{
    for (long i = 0; i < IterationsPerThread; ++i)
    {
        count++;
    }
}

void mutexWorker()
{
    for (long i = 0; i < IterationsPerThread; ++i)
    {
        std::lock_guard<std::mutex> lock(counterMutex);
        count++;
    }
}

void atomicWorker()
{
    for (long i = 0; i < IterationsPerThread; ++i)
    {
        atomicCount++;
    }
}

// ==================== Counter Helpers ====================

void resetCount()
{
    count = 0;
}

long getCount()
{
    return count;
}

void resetAtomicCount()
{
    atomicCount = 0;
}

long getAtomicCount()
{
    return atomicCount.load();
}

// ==================== Benchmark Runner ====================

void runBenchmark(
    const char* name,
    void (*workerFunction)(),
    void (*resetFunction)(),
    long (*getCountFunction)())
{
    const long expectedCount = IterationsPerThread * 2;

    int correctRuns = 0;
    long long totalFinalCount = 0;

    double totalTime = 0.0;
    double minTime = std::numeric_limits<double>::max();
    double maxTime = 0.0;

    std::cout << "\n=== " << name << " Benchmark ===\n";

    for (int run = 1; run <= NumberOfRuns; ++run)
    {
        resetFunction();

        const auto start = std::chrono::steady_clock::now();

        std::thread threadA(workerFunction);
        std::thread threadB(workerFunction);

        threadA.join();
        threadB.join();

        const auto end = std::chrono::steady_clock::now();

        const double elapsed =
            std::chrono::duration<double, std::milli>(end - start).count();

        const long finalCount = getCountFunction();

        if (finalCount == expectedCount)
        {
            correctRuns++;
        }

        totalFinalCount += finalCount;
        totalTime += elapsed;

        if (elapsed < minTime)
        {
            minTime = elapsed;
        }

        if (elapsed > maxTime)
        {
            maxTime = elapsed;
        }

        std::cout << "Run " << run
            << " | Final count: " << finalCount
            << " | Correct: "
            << (finalCount == expectedCount ? "Yes" : "No")
            << " | Time: " << elapsed << " ms\n";
    }

    const double averageFinalCount =
        static_cast<double>(totalFinalCount) / NumberOfRuns;

    const double averageTime =
        totalTime / NumberOfRuns;

    const double correctnessRate =
        (static_cast<double>(correctRuns) / NumberOfRuns) * 100.0;

    std::cout << "\n" << name << " Summary\n";

    std::cout << "Correct runs: "
        << correctRuns << "/" << NumberOfRuns << '\n';

    std::cout << "Correctness rate: "
        << correctnessRate << "%\n";

    std::cout << "Average final count: "
        << std::fixed << std::setprecision(0)
        << averageFinalCount << '\n';

    std::cout << std::setprecision(4);

    std::cout << "Average runtime: "
        << averageTime << " ms\n";

    std::cout << "Minimum runtime: "
        << minTime << " ms\n";

    std::cout << "Maximum runtime: "
        << maxTime << " ms\n";
}




// ==================== Program Entry Point ====================

int main()
{
    runBenchmark(
        "Unsynchronised",
        worker,
        resetCount,
        getCount);

    runBenchmark(
        "Mutex",
        mutexWorker,
        resetCount,
        getCount);

    runBenchmark(
        "Atomic",
        atomicWorker,
        resetAtomicCount,
        getAtomicCount);

    return 0;
}