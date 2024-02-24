#pragma once

#include <glacier/memory/unique_ptr.h>

#include "xhci/trb_ring.h"
#include "xhci/xhci.h"

class Endpoint {
 public:
  Endpoint() {}

  void Initialize(XhciEndpointContext* context);

  bool Enabled() { return enabled_; }

  void TransferComplete(uint64_t trb_phys);

 private:
  bool enabled_ = false;

  XhciEndpointContext* context_ = nullptr;
  glcr::UniquePtr<TrbRingWriter> trb_ring_;

  uint64_t recv_phys_;
  mmth::OwnedMemoryRegion recv_mem_;
  uint64_t offset_ = 0;
};
