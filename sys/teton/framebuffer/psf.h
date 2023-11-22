#pragma once

#include <mammoth/memory_region.h>

struct PsfHeader {
  uint32_t magic;         /* magic bytes to identify PSF */
  uint32_t version;       /* zero */
  uint32_t headersize;    /* offset of bitmaps in file, 32 */
  uint32_t flags;         /* 0 if there's no unicode table */
  uint32_t numglyph;      /* number of glyphs */
  uint32_t bytesperglyph; /* size of each glyph */
  uint32_t height;        /* height in pixels */
  uint32_t width;         /* width in pixels */
};

class Psf {
 public:
  Psf(OwnedMemoryRegion&& psf_file);

  void DumpHeader();

  uint32_t size() { return header_->numglyph; }

  uint8_t* glyph(uint32_t index) {
    return reinterpret_cast<uint8_t*>(psf_file_.vaddr() + header_->headersize +
                                      (index * header_->bytesperglyph));
  }

 private:
  OwnedMemoryRegion psf_file_;
  PsfHeader* header_;

  void EnsureValid();
};
