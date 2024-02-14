#pragma once

#include <glacier/container/array_view.h>
#include <mammoth/util/memory_region.h>

#include "xhci/xhci.h"

class TrbRing {
 public:
  TrbRing();

  uint64_t PhysicalAddress() { return phys_address_; }

 protected:
  uint64_t phys_address_;
  mmth::OwnedMemoryRegion page_;
  glcr::ArrayView<XhciTrb> trb_list_;
};

class TrbRingWriter : public TrbRing {
 public:
  void EnqueueTrb(const XhciTrb& trb);

 private:
  uint64_t enqueue_ptr_ = 0;
};
