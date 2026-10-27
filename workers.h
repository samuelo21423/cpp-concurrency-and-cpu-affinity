#pragma once

void worker();
void mutexWorker();
void atomicWorker();

void resetCount();
long getCount();

void resetAtomicCount();
long getAtomicCount();