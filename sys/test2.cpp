#include "mammoth/debug.h"
#include "mammoth/thread.h"

#define CHECK(expr)       \
  {                       \
    uint64_t code = expr; \
    if (code != Z_OK) {   \
      ZDebug("crash!");   \
      return 1;           \
    }                     \
  }

void thread_entry(void* a) {
  dbgln("In thread");
  dbgln(static_cast<const char*>(a));
}

int main() {
  dbgln("Main thread");

  const char* a = "a";
  const char* b = "bee";
  const char* c = "cee";
  const char* d = "dee";
  Thread t1(thread_entry, a);
  Thread t2(thread_entry, b);
  return 0;
}
