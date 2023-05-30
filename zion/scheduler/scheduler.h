#pragma once

#include "scheduler/process.h"
#include "scheduler/thread.h"

namespace sched {

// Create the scheduler object in a disabled state,
// processes can be added but will not be scheduled.
void InitScheduler();

// Enables the scheduler such that processes will yield on ticks.
void EnableScheduler();

// Preempts the current thread and flags it as runnable in the queue.
// Generally used by the timer to move to the next timeslice.
void Preempt();

// Current thread yields and is not rescheduled until some external process
// adds it.
// Used when a thread blocks or exits.
void Yield();

void EnqueueThread(Thread* thread);

Process& CurrentProcess();
Thread& CurrentThread();

}  // namespace sched
