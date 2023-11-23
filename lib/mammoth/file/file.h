#pragma once

#include <glacier/memory/move.h>
#include <glacier/string/string_view.h>

#include "mammoth/util/memory_region.h"

namespace mmth {

class File {
 public:
  static File Open(glcr::StringView path);

  glcr::StringView as_str();

  void* raw_ptr();
  uint8_t* byte_ptr();

 private:
  OwnedMemoryRegion file_data_;

  File(OwnedMemoryRegion&& file) : file_data_(glcr::Move(file)) {}
};

}  // namespace mmth
