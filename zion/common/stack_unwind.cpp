#include "common/stack_unwind.h"

#include "debug/debug.h"

namespace {

bool IsValid(uint64_t* rbp) { return rbp && *rbp != kStackBaseSentinel; }

}  // namespace

void StackUnwind(uint64_t rbp) {
  dbgln("-- Begin Stack --");
  uint64_t* rbp_ptr = reinterpret_cast<uint64_t*>(rbp);
  while (IsValid(rbp_ptr)) {
    uint64_t rip = *(rbp_ptr + 1);
    dbgln("RIP: {x}", rip);
    rbp_ptr = reinterpret_cast<uint64_t*>(*rbp_ptr);
  }
  dbgln("-- End Stack --");
}
