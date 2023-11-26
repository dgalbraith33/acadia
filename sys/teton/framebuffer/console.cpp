#include "framebuffer/console.h"

#include <mammoth/util/debug.h>

void Console::WriteChar(char c) {
  if (c == '\n') {
    CursorReturn();
    return;
  }

  uint64_t row = cursor_pos_ / cols();
  if (row >= rows()) {
    crash("Unimplemented console scroll.", glcr::UNIMPLEMENTED);
  }
  uint64_t fb_row = row * (psf_.height() + 1);
  uint64_t col = cursor_pos_ % cols();
  uint64_t fb_col = col * (psf_.width() + 1);

  uint8_t* glyph = psf_.glyph(c);

  for (uint32_t r = fb_row; r < fb_row + psf_.height(); r++) {
    for (uint32_t j = fb_col; j < fb_col + psf_.width(); j++) {
      uint8_t glyph_offset = psf_.width() - (j - fb_col) - 1;
      if ((glyph[r - fb_row] & (1 << glyph_offset))) {
        framebuf_.DrawPixel(r, j, 0xFFFFFFF);
      } else {
        framebuf_.DrawPixel(r, j, 0);
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

void Console::CursorReturn() {
  cursor_pos_ -= cursor_pos_ % cols();
  cursor_pos_ += cols();
}
