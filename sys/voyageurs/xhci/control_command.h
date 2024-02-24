#pragma once

#include <glacier/memory/shared_ptr.h>
#include <mammoth/sync/semaphore.h>
#include <mammoth/util/memory_region.h>

#include "xhci/descriptors.h"
#include "xhci/xhci.h"

template <typename Output>
class ReadControlCommand {
 public:
  ReadControlCommand() {
    output_mem_ =
        mmth::OwnedMemoryRegion::ContiguousPhysical(0x1000, &output_phys_);
  }

  XhciTrb SetupTrb() {
    uint64_t request_type = RequestConstants<Output>::RequestType();
    uint64_t request = RequestConstants<Output>::Request() << 8;
    uint64_t value = RequestConstants<Output>::Value() << 16;
    // TODO: May need to be non-0 for string descriptors.
    uint64_t index = (uint64_t)0 << 32;
    uint64_t length = (uint64_t)0x1000 << 48;
    return {
        .parameter = request_type | request | value | index | length,
        .status = 8,
        .type_and_cycle = 1 | (1 << 5) | (1 << 6) | (2 << 10),
        // IN Data Stage
        .control = 3,
    };
  }

  XhciTrb DataTrb() {
    return {
        .parameter = output_phys_,
        .status = 0x1000,
        .type_and_cycle = 1 | (1 << 5) | (3 << 10),
        .control = 1,
    };
  }

  XhciTrb StatusTrb() {
    return {
        .parameter = 0,
        .status = 0,
        .type_and_cycle = 1 | (1 << 5) | (4 << 10),
        .control = 0,
    };
  }

  Output* AwaitResult() {
    if (!is_complete_) {
      semaphore_->Wait();
    }
    is_complete_ = true;
    return reinterpret_cast<Output*>(output_mem_.vaddr());
  }

  glcr::SharedPtr<mmth::Semaphore> CompletionSemaphore() { return semaphore_; }

 private:
  uint64_t output_phys_;
  mmth::OwnedMemoryRegion output_mem_;

  bool is_complete_ = false;
  glcr::SharedPtr<mmth::Semaphore> semaphore_ =
      glcr::MakeShared<mmth::Semaphore>();
};
