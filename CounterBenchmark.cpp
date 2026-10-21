#include <chrono>
#include <iostream>
#include <limits>
#include <thread>

constexpr long IterationsPerThread = 10'000'000;
constexpr int NumberOfRuns = 10;

// Shared state intentionally left unsynchronised.
// This allows the benchmark to expose lost updates caused by a race condition.
long count = 0;

void worker()
{
    for (long i = 0; i < IterationsPerThread; ++i)
    {
        count++;
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

    std::cout << "Unsynchronised Counter Benchmark\n";
    std::cout << "Threads: 2\n";
    std::cout << "Iterations per thread: " << IterationsPerThread << '\n';
    std::cout << "Expected count: " << expectedCount << "\n\n";

    for (int run = 1; run <= NumberOfRuns; ++run)
    {
        // Reset the shared counter so each trial starts from the same state.
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
        << averageFinalCount << '\n';

    std::cout << "Average runtime: "
        << averageTime << " ms\n";

    std::cout << "Minimum runtime: "
        << minTime << " ms\n";

    std::cout << "Maximum runtime: "
        << maxTime << " ms\n";

    return 0;
}