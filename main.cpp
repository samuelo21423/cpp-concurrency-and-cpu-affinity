#include "benchmark.h"
#include "workers.h"

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