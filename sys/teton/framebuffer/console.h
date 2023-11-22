#pragma once

#include <glacier/string/string_view.h>

#include "framebuffer/framebuffer.h"
#include "framebuffer/psf.h"

class Console {
 public:
  explicit Console(Framebuffer& fb, Psf& psf) : framebuf_(fb), psf_(psf) {}

  void WriteChar(char c);
  void WriteString(glcr::StringView str);

  uint32_t rows() { return framebuf_.height() / (psf_.height() + 1); }
  uint32_t cols() { return framebuf_.width() / (psf_.width() + 1); }

 private:
  // TODO: Don't store a reference here.
  Framebuffer& framebuf_;
  Psf& psf_;
  uint64_t cursor_pos_ = 0;

  void CursorIncr();
  void CursorReturn();
};
