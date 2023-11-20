#pragma once

#include <mammoth/memory_region.h>
#include <yellowstone/yellowstone.yunq.h>

class Framebuffer {
 public:
  Framebuffer(const FramebufferInfo& info);

  void DrawPixel(uint32_t row, uint32_t col, uint32_t pixel);

 private:
  // FIXME: Implement Yunq copy or move so we
  // don't have to store a reference here.
  const FramebufferInfo& fb_info_;

  OwnedMemoryRegion fb_memory_;
  uint32_t* fb_;
};
