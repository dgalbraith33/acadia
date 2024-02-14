#include "xhci/trb_ring.h"

#include <mammoth/util/debug.h>

#include "xhci/trb.h"

TrbRing::TrbRing() {
  uint64_t number_trbs = 0x1000 / sizeof(XhciTrb);
  page_ = mmth::OwnedMemoryRegion::ContiguousPhysical(0x1000, &phys_address_);
  trb_list_ = glcr::ArrayView<XhciTrb>(
      reinterpret_cast<XhciTrb*>(page_.vaddr()), number_trbs);

  // Point the end of the command ring back to the start.
  trb_list_[trb_list_.size() - 1] = CreateLinkTrb(phys_address_);
}

void TrbRingWriter::EnqueueTrb(const XhciTrb& trb) {
  uint64_t ptr = enqueue_ptr_++;
  if (enqueue_ptr_ == trb_list_.size()) {
    crash("Not implemented: queue wrapping", glcr::UNIMPLEMENTED);
  }

  trb_list_[ptr] = trb;
}
