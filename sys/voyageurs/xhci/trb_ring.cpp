#include "xhci/trb_ring.h"

#include <mammoth/util/debug.h>

#include "xhci/trb.h"

TrbRing::TrbRing() {
  uint64_t number_trbs = 0x1000 / sizeof(XhciTrb);
  page_ = mmth::OwnedMemoryRegion::ContiguousPhysical(0x1000, &phys_address_);

  // Zero out data.
  uint64_t* page_ptr = reinterpret_cast<uint64_t*>(page_.vaddr());
  for (uint64_t i = 0; i < 0x1000 / sizeof(uint64_t); i++) {
    page_ptr[i] = 0;
  }
  trb_list_ = glcr::ArrayView<XhciTrb>(
      reinterpret_cast<XhciTrb*>(page_.vaddr()), number_trbs);

  // Point the end of the command ring back to the start.
  trb_list_[trb_list_.size() - 1] = CreateLinkTrb(phys_address_);
}

XhciTrb TrbRing::GetTrbFromPhysical(uint64_t address) {
  uint64_t offset = address - phys_address_;
  if (offset >= 0x1000) {
    crash("Invalid offset in GetTrbFromPhysical", glcr::INVALID_ARGUMENT);
  }
  offset /= sizeof(XhciTrb);
  return trb_list_[offset];
}

uint64_t TrbRingWriter::EnqueueTrb(const XhciTrb& trb) {
  uint64_t ptr = enqueue_ptr_++;
  if (enqueue_ptr_ == trb_list_.size()) {
    crash("Not implemented: queue wrapping", glcr::UNIMPLEMENTED);
  }

  trb_list_[ptr] = trb;
  return phys_address_ + (ptr * sizeof(XhciTrb));
}

bool TrbRingReader::HasNext() {
  return (trb_list_[dequeue_ptr_].type_and_cycle & 0x1) == cycle_bit_;
}

XhciTrb TrbRingReader::Read() { return trb_list_[dequeue_ptr_++]; }
