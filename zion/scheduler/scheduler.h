#pragma once

#include "scheduler/process.h"
#include "scheduler/thread.h"

namespace sched {

// Create the scheduler object in a disabled state,
// processes can be added but will not be scheduled.
void InitScheduler();

// Enables the scheduler such that processes will yield on ticks.
void EnableScheduler();

void Yield();

// Scheduler will take ownership
// of the created process.
void InsertProcess(Process* proc);

Process& CurrentProcess();
Thread& CurrentThread();

}  // namespace sched
