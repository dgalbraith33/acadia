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

static volatile struct limine_module_request gModuleRequest {
  .id = LIMINE_MODULE_REQUEST, .revision = 0, .response = 0,
};

const limine_module_response& GetModules() {
  if (!gModuleRequest.response) {
    panic("No module response from limine");
  }
  return *gModuleRequest.response;
}

static volatile struct limine_rsdp_request gRsdpRequest {
  .id = LIMINE_RSDP_REQUEST, .revision = 0, .response = 0,
};

void* GetRsdpAddr() {
  if (!gRsdpRequest.response) {
    panic("No rsdp response from limine");
  }
  return gRsdpRequest.response->address;
}

static volatile struct limine_framebuffer_request gFramebufferRequest {
  .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = 0,
};

const limine_framebuffer& GetFramebuffer() {
  if (!gFramebufferRequest.response) {
    panic("No framebuffer response from limine");
  }
  if (gFramebufferRequest.response->framebuffer_count < 1) {
    panic("No framebuffers in response from limine.");
  }

  return *gFramebufferRequest.response->framebuffers[0];
}

}  // namespace boot
