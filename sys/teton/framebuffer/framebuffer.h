#pragma once

#include <mammoth/util/memory_region.h>
#include <yellowstone/yellowstone.yunq.h>

class Framebuffer {
 public:
  Framebuffer(const FramebufferInfo& info);

  void DrawPixel(uint32_t row, uint32_t col, uint32_t pixel);

  void DrawGlyph(uint8_t* glyph);

  uint64_t width() { return fb_info_.width(); }
  uint64_t height() { return fb_info_.height(); }

 private:
  // FIXME: Implement Yunq copy or move so we
  // don't have to store a reference here.
  const FramebufferInfo& fb_info_;

  mmth::OwnedMemoryRegion fb_memory_;
  uint32_t* fb_;
  uint32_t cursor_pos_;
};
