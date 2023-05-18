#include "boot/boot_info.h"

#include "debug/debug.h"

namespace boot {
static volatile struct limine_memmap_request gMemmapRequest {
  .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = 0,
};

const limine_memmap_response& GetMemoryMap() {
  if (!gMemmapRequest.response) {
    panic("No memmap from limine.");
  }
  return *gMemmapRequest.response;
}

static volatile struct limine_hhdm_request gHhdmRequest {
  .id = LIMINE_HHDM_REQUEST, .revision = 0, .response = 0,
};

uint64_t GetHigherHalfDirectMap() {
  if (!gHhdmRequest.response) {
    panic("No HHDM response from limine.");
  }
  return gHhdmRequest.response->offset;
}

}  // namespace boot
