#include "framebuffer/framebuffer.h"

Framebuffer::Framebuffer(const yellowstone::FramebufferInfo& info)
    : fb_info_(info), cursor_pos_(0) {
  uint64_t buff_size_bytes = fb_info_.height() * fb_info_.pitch();
  fb_memory_ = mmth::OwnedMemoryRegion::DirectPhysical(fb_info_.address_phys(),
                                                       buff_size_bytes);
  fb_ = reinterpret_cast<uint32_t*>(fb_memory_.vaddr());
}

void Framebuffer::DrawPixel(uint32_t row, uint32_t col, uint32_t pixel) {
  // Div by 4 because pitch is in bytes and fb_ is a 32bit array.
  fb_[(row * fb_info_.pitch() / 4) + col] = pixel;
}

void Framebuffer::DrawGlyph(uint8_t* glyph) {
  uint32_t gl_width = 8;
  uint32_t gl_height = 16;

  for (uint8_t r = 0; r < gl_height; r++) {
    for (uint8_t c = 0; c < gl_width; c++) {
      if (((glyph[r] >> c) % 2) == 1) {
        DrawPixel(r, gl_width - c - 1, 0xFFFF'FFFF);
      } else {
        DrawPixel(r, gl_width - c - 1, 0);
      }
    }
  }
};
