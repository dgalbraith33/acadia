#include "framebuffer/framebuffer.h"

#include <mammoth/memory_region.h>

Framebuffer::Framebuffer(const FramebufferInfo& info) : fb_info_(info) {
  uint64_t buff_size_bytes = fb_info_.height() * fb_info_.pitch();
  MappedMemoryRegion region = MappedMemoryRegion::DirectPhysical(
      fb_info_.address_phys(), buff_size_bytes);
  fb_ = reinterpret_cast<uint32_t*>(region.vaddr());
}

void Framebuffer::DrawPixel(uint32_t row, uint32_t col, uint32_t pixel) {
  // Div by 4 because pitch is in bytes and fb_ is a 32bit array.
  fb_[(row * fb_info_.pitch() / 4) + col] = pixel;
}
