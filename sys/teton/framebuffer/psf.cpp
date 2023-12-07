#include "framebuffer/psf.h"

#include <glacier/memory/move.h>
#include <mammoth/util/debug.h>

#define PSF_DEBUG 0

namespace {

const uint32_t kMagic = 0x864AB572;

}

Psf::Psf(glcr::StringView path)
    : psf_file_(mmth::File::Open(path)),
      header_(reinterpret_cast<PsfHeader*>(psf_file_.raw_ptr())) {
  EnsureValid();
}

void Psf::DumpHeader() {
#if PSF_DEBUG
  dbgln("Magic: {x}", header_->magic);
  dbgln("Version: {x}", header_->version);
  dbgln("Header Sz: {x}", header_->headersize);
  dbgln("Flags: {x}", header_->flags);
  dbgln("Length: {x}", header_->numglyph);
  dbgln("Glyph Size: {x}", header_->bytesperglyph);
  dbgln("Height: {x}", header_->height);
  dbgln("Width: {x}", header_->width);
#endif
}

void Psf::EnsureValid() {
  if (header_->magic != kMagic) {
    dbgln("PSF: Magic value: {x}", header_->magic);
    crash("PSF: Invalid magic value", glcr::INVALID_ARGUMENT);
  }

  if (header_->version != 0) {
    crash("PSF non-zero version", glcr::INVALID_ARGUMENT);
  }

  if (header_->height != 0x10) {
    crash("PSF height other than 16 not handled", glcr::UNIMPLEMENTED);
  }

  if (header_->width != 0x8) {
    crash("PSF width other than 8 not handled", glcr::UNIMPLEMENTED);
  }
}
