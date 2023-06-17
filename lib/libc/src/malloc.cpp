#include <zcall.h>
#include <zglobal.h>

#include "stdlib.h"

namespace {
class NaiveAllocator {
 public:
  constexpr static uint64_t kSize = 0x4000;
  NaiveAllocator() {}
  bool is_init() { return next_addr_ != 0; }
  void Init() {
    uint64_t vmmo_cap;
    uint64_t err = ZMemoryObjectCreate(kSize, &vmmo_cap);
    if (err != 0) {
      ZProcessExit(err);
    }
    err = ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, &next_addr_);
    max_addr_ = next_addr_ + kSize;
  }

  void* Allocate(size_t size) {
    uint64_t addr = next_addr_;
    next_addr_ += size;
    if (next_addr_ >= max_addr_) {
      return 0;
    }
    return reinterpret_cast<void*>(addr);
  }

 private:
  uint64_t next_addr_ = 0;
  uint64_t max_addr_ = 0;
};

NaiveAllocator gAlloc;

}  // namespace

void* malloc(size_t size) {
  if (!gAlloc.is_init()) {
    gAlloc.Init();
  }
  return gAlloc.Allocate(size);
}
