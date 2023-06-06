

#include "zcall.h"
#include "zerrors.h"

#define CHECK(expr)       \
  {                       \
    uint64_t code = expr; \
    if (code != Z_OK) {   \
      ZDebug("crash!");   \
      return 1;           \
    }                     \
  }

void thread_entry(char* a, char* b) {
  ZDebug("In thread");
  ZDebug(a);
  ZDebug(b);

  ZThreadExit();
}

int main() {
  ZDebug("Testing");
  uint64_t t1, t2;
  CHECK(ZThreadCreate(Z_INIT_PROC_SELF, &t1));
  CHECK(ZThreadCreate(Z_INIT_PROC_SELF, &t2));

  const char* a = "a";
  const char* b = "bee";
  const char* c = "cee";
  const char* d = "dee";
  CHECK(ZThreadStart(t1, reinterpret_cast<uint64_t>(thread_entry),
                     reinterpret_cast<uint64_t>(a),
                     reinterpret_cast<uint64_t>(b)));
  CHECK(ZThreadStart(t2, reinterpret_cast<uint64_t>(thread_entry),
                     reinterpret_cast<uint64_t>(c),
                     reinterpret_cast<uint64_t>(d)));
  return 0;
}
