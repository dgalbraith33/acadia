#pragma once

#include <glacier/container/vector.h>
#include <glacier/memory/move.h>
#include <glacier/status/error_or.h>
#include <glacier/string/string.h>
#include <glacier/string/string_view.h>

#include "mammoth/util/memory_region.h"

namespace mmth {

// Intended for use in yellowstone since it already has the VFS cap.
void SetVfsCap(z_cap_t vfs_cap);

class File {
 public:
  static File Open(glcr::StringView path);

  uint64_t size() { return size_; }

  glcr::StringView as_str();

  void* raw_ptr();
  uint8_t* byte_ptr();

 private:
  OwnedMemoryRegion file_data_;
  uint64_t size_;

  File(OwnedMemoryRegion&& file, uint64_t size)
      : file_data_(glcr::Move(file)), size_(size) {}
};

// TODO: Move this to a separate file.
glcr::ErrorOr<glcr::Vector<glcr::String>> ListDirectory(glcr::StringView path);

}  // namespace mmth
