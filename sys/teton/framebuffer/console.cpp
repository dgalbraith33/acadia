#include "framebuffer/console.h"

void Console::WriteChar(char c) {
  uint64_t row = cursor_pos_ / cols();
  uint64_t fb_row = row * (psf_.height() + 1);
  uint64_t col = cursor_pos_ % cols();
  uint64_t fb_col = col * (psf_.width() + 1);

  uint8_t* glyph = psf_.glyph(c);

  for (uint8_t r = fb_row; r < fb_row + psf_.height(); r++) {
    for (uint8_t c = fb_col; c < fb_col + psf_.width(); c++) {
      uint8_t glyph_offset = psf_.width() - (c - fb_col) - 1;
      if ((glyph[r] & (1 << glyph_offset))) {
        framebuf_.DrawPixel(r, c, 0xFFFFFFF);
      } else {
        framebuf_.DrawPixel(r, c, 0);
      }
    }
  }

  cursor_pos_++;
}
void Console::WriteString(glcr::StringView str) {
  for (uint64_t i = 0; i < str.size(); i++) {
    WriteChar(str[i]);
  }
}
