#include "xhci/endpoint.h"

#include <mammoth/util/debug.h>

#include "xhci/trb.h"

void Endpoint::Initialize(XhciEndpointContext* context,
                          glcr::UniquePtr<mmth::PortClient> client) {
  enabled_ = true;
  context_ = context;
  client_ = glcr::Move(client);

  trb_ring_ = glcr::MakeUnique<TrbRingWriter>();
  recv_mem_ = mmth::OwnedMemoryRegion::ContiguousPhysical(0x1000, &recv_phys_);

  context_->tr_dequeue_ptr = trb_ring_->PhysicalAddress() | 1;

  context_->error_and_type = (0x3 << 1) | (0x7 << 3) | (0x8 << 16);
  for (uint64_t i = 0; i < 10; i++) {
    trb_ring_->EnqueueTrb(CreateNormalTrb(recv_phys_ + offset_, 8));
    offset_ += 8;
  }
}

void Endpoint::TransferComplete(uint64_t trb_phys) {
  uint64_t phys_offset =
      (trb_phys - trb_ring_->PhysicalAddress()) / sizeof(XhciTrb);
  uint64_t data = *((uint64_t*)recv_mem_.vaddr() + phys_offset);
  client_->Write(data);
  trb_ring_->EnqueueTrb(CreateNormalTrb(recv_phys_ + offset_, 8));
  offset_ += 8;
}
