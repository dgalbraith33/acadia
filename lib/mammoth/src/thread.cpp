#include "include/mammoth/thread.h"

#include "zcall.h"

namespace {

extern "C" void thread_entry(Thread::Entry entry, void* arg1) {
  entry(arg1);

  ZThreadExit();
}

}  // namespace

Thread::Thread(Entry e, const void* arg1) {
  ZThreadCreate(Z_INIT_PROC_SELF, &thread_cap_);
  ZThreadStart(thread_cap_, reinterpret_cast<uint64_t>(thread_entry),
               reinterpret_cast<uint64_t>(e), reinterpret_cast<uint64_t>(arg1));
}
