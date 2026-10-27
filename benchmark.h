#pragma once

void runBenchmark(
    const char* name,
    void (*workerFunction)(),
    void (*resetFunction)(),
    long (*getCountFunction)());