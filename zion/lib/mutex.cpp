#include "lib/mutex.h"

#include "debug/debug.h"
#include "scheduler/scheduler.h"

void Mutex::Lock() {
  while (__atomic_fetch_or(&lock_, 0x1, __ATOMIC_SEQ_CST) == 0x1) {
    dbgln("Lock sleep: %s", name_);
    gScheduler->Preempt();
  }
}
