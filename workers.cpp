#include "workers.h"
#include "config.h"

#include <atomic>
#include <mutex>

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