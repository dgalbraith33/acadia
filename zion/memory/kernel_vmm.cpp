#include "memory/kernel_vmm.h"

#include "debug/debug.h"
#include "memory/paging_util.h"

namespace {

KernelVmm* gKernelVmm = nullptr;

}
KernelVmm::KernelVmm() {
  if (gKernelVmm) {
    panic("KernelVmm double init.");
  }
  gKernelVmm = this;
  stack_manager_ = glcr::MakeUnique<KernelStackManager>();
  stack_manager_->SetupInterruptStack();
}

uint64_t KernelVmm::AcquireSlabHeapRegion(uint64_t slab_size_bytes) {
  return gKernelVmm->AcquireSlabHeapRegionInternal(slab_size_bytes);
}

uint64_t KernelVmm::AcquireKernelStack() {
  return gKernelVmm->stack_manager_->AllocateKernelStack();
}

uint64_t KernelVmm::AcquireSlabHeapRegionInternal(uint64_t slab_size_bytes) {
  uint64_t next_slab = next_slab_heap_page_;
  if (next_slab >= kKernelBuddyHeapEnd) {
    panic("Slab heap overrun");
  }
  next_slab_heap_page_ += slab_size_bytes;
  // TODO: Consider handling this as a part of a page-fault handler
  // rather than auto mapping all over the place.
  EnsureResident(next_slab, slab_size_bytes);
  return next_slab;
}
