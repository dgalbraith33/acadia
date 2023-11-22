#include "proc/thread.h"

#include <zcall.h>

#include "mammoth/debug.h"
#include "mammoth/init.h"

namespace {

extern "C" void thread_entry(Thread::Entry entry, void* arg1) {
  entry(arg1);

  (void)ZThreadExit();
}

}  // namespace

Thread::Thread(Entry e, const void* arg1) {
  check(ZThreadCreate(gSelfProcCap, &thread_cap_));
  check(ZThreadStart(thread_cap_, reinterpret_cast<uint64_t>(thread_entry),
                     reinterpret_cast<uint64_t>(e),
                     reinterpret_cast<uint64_t>(arg1)));
}

glcr::ErrorCode Thread::Join() {
  return static_cast<glcr::ErrorCode>(ZThreadWait(thread_cap_));
}
