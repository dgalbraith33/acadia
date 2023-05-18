#include "scheduler/thread.h"

#include "scheduler/process.h"

uint64_t Thread::pid() { return process_->id(); }
