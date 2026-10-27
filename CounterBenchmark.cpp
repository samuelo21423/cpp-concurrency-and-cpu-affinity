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
long count = 0;
std::mutex counterMutex;
std::atomic<long> atomicCount = 0;

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

int main()
{
    const long expectedCount = IterationsPerThread * 2;

    int correctRuns = 0;
    long long totalFinalCount = 0;

    double totalTime = 0.0;
    double minTime = std::numeric_limits<double>::max();
    double maxTime = 0.0;

    // ==================== Unsynchronised Benchmark ====================

    std::cout << "Unsynchronised Counter Benchmark\n";
    std::cout << "Threads: 2\n";
    std::cout << "Iterations per thread: " << IterationsPerThread << '\n';
    std::cout << "Expected count: " << expectedCount << "\n\n";

    for (int run = 1; run <= NumberOfRuns; ++run)
    {
        count = 0;

        const auto start = std::chrono::steady_clock::now();

        std::thread threadA(worker);
        std::thread threadB(worker);

        threadA.join();
        threadB.join();

        const auto end = std::chrono::steady_clock::now();

        const double elapsed =
            std::chrono::duration<double, std::milli>(end - start).count();

        if (count == expectedCount)
        {
            correctRuns++;
        }

        totalFinalCount += count;
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
            << " | Final count: " << count
            << " | Correct: " << (count == expectedCount ? "Yes" : "No")
            << " | Time: " << elapsed << " ms\n";
    }

    const double averageFinalCount =
        static_cast<double>(totalFinalCount) / NumberOfRuns;

    const double averageTime =
        totalTime / NumberOfRuns;

    const double correctnessRate =
        (static_cast<double>(correctRuns) / NumberOfRuns) * 100.0;

    std::cout << "\nSummary\n";

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

    // ==================== Mutex Benchmark ====================

    std::cout << "\n=== Mutex Benchmark ===\n";

    correctRuns = 0;
    totalFinalCount = 0;
    totalTime = 0.0;
    minTime = std::numeric_limits<double>::max();
    maxTime = 0.0;

    for (int run = 1; run <= NumberOfRuns; ++run)
    {
        count = 0;

        const auto start = std::chrono::steady_clock::now();

        std::thread threadA(mutexWorker);
        std::thread threadB(mutexWorker);

        threadA.join();
        threadB.join();

        const auto end = std::chrono::steady_clock::now();

        const double elapsed =
            std::chrono::duration<double, std::milli>(end - start).count();

        if (count == expectedCount)
        {
            correctRuns++;
        }

        totalFinalCount += count;
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
            << " | Final count: " << count
            << " | Correct: " << (count == expectedCount ? "Yes" : "No")
            << " | Time: " << elapsed << " ms\n";
    }

    const double mutexAverageFinalCount =
        static_cast<double>(totalFinalCount) / NumberOfRuns;

    const double mutexAverageTime =
        totalTime / NumberOfRuns;

    const double mutexCorrectnessRate =
        (static_cast<double>(correctRuns) / NumberOfRuns) * 100.0;

    std::cout << "\nMutex Summary\n";

    std::cout << "Correct runs: "
        << correctRuns << "/" << NumberOfRuns << '\n';

    std::cout << "Correctness rate: "
        << mutexCorrectnessRate << "%\n";

    std::cout << "Average final count: "
        << std::fixed << std::setprecision(0)
        << mutexAverageFinalCount << '\n';

    std::cout << std::setprecision(4);

    std::cout << "Average runtime: "
        << mutexAverageTime << " ms\n";

    std::cout << "Minimum runtime: "
        << minTime << " ms\n";

    std::cout << "Maximum runtime: "
        << maxTime << " ms\n";

    // ==================== Atomic Benchmark ====================

    std::cout << "\n=== Atomic Benchmark ===\n";

    correctRuns = 0;
    totalFinalCount = 0;
    totalTime = 0.0;
    minTime = std::numeric_limits<double>::max();
    maxTime = 0.0;

    for (int run = 1; run <= NumberOfRuns; ++run)
    {
        atomicCount = 0;

        const auto start = std::chrono::steady_clock::now();

        std::thread threadA(atomicWorker);
        std::thread threadB(atomicWorker);

        threadA.join();
        threadB.join();

        const auto end = std::chrono::steady_clock::now();

        const double elapsed =
            std::chrono::duration<double, std::milli>(end - start).count();

        const long finalCount = atomicCount.load();

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
            << " | Correct: " << (finalCount == expectedCount ? "Yes" : "No")
            << " | Time: " << elapsed << " ms\n";
    }

    const double atomicAverageFinalCount =
        static_cast<double>(totalFinalCount) / NumberOfRuns;

    const double atomicAverageTime =
        totalTime / NumberOfRuns;

    const double atomicCorrectnessRate =
        (static_cast<double>(correctRuns) / NumberOfRuns) * 100.0;

    std::cout << "\nAtomic Summary\n";

    std::cout << "Correct runs: "
        << correctRuns << "/" << NumberOfRuns << '\n';

    std::cout << "Correctness rate: "
        << atomicCorrectnessRate << "%\n";

    std::cout << "Average final count: "
        << std::fixed << std::setprecision(0)
        << atomicAverageFinalCount << '\n';

    std::cout << std::setprecision(4);

    std::cout << "Average runtime: "
        << atomicAverageTime << " ms\n";

    std::cout << "Minimum runtime: "
        << minTime << " ms\n";

    std::cout << "Maximum runtime: "
        << maxTime << " ms\n";

    return 0;
}