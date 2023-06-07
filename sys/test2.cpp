#include <mammoth/debug.h>
#include <mammoth/thread.h>
#include <zcall.h>

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

int main(uint64_t bootstrap_cap) {
  dbgln("Main thread");

  const char* a = "a";
  const char* b = "bee";
  const char* c = "cee";
  const char* d = "dee";
  Thread t1(thread_entry, a);
  Thread t2(thread_entry, b);

  uint64_t num_bytes = 10;
  uint64_t num_caps;
  uint8_t bytes[10];
  uint64_t type;
  check(ZChannelRecv(bootstrap_cap, num_bytes, bytes, 0, 0, &type, &num_bytes,
                     &num_caps));
  dbgln(reinterpret_cast<char*>(bytes));
  return 0;
}
