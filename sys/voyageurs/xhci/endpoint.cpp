#include "xhci/endpoint.h"

#include <mammoth/util/debug.h>

void Endpoint::Initialize(XhciEndpointContext* context) {
  enabled_ = true;
  context_ = context;

  trb_ring_ = glcr::MakeUnique<TrbRingWriter>();
  recv_mem_ = mmth::OwnedMemoryRegion::ContiguousPhysical(0x1000, &recv_phys_);

  context_->tr_dequeue_ptr = trb_ring_->PhysicalAddress() | 1;

  context_->error_and_type = (0x3 << 1) | (0x7 << 3) | (0x8 << 16);
  trb_ring_->EnqueueTrb({
      .parameter = recv_phys_,
      .status = 8,
      .type_and_cycle = 1 | (1 << 2) | (1 << 5) | (1 << 10),
      .control = 0,
  });
}

void Endpoint::TransferComplete(uint64_t trb_phys) {
  dbgln("Data: {x}", *(uint64_t*)recv_mem_.vaddr());
  trb_ring_->EnqueueTrb({
      .parameter = recv_phys_,
      .status = 8,
      .type_and_cycle = 1 | (1 << 2) | (1 << 5) | (1 << 10),
      .control = 0,
  });
}
