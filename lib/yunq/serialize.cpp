#include "serialize.h"

namespace yunq {

glcr::Status CheckHeader(const glcr::ByteBuffer& buffer) {
  // TODO: Check ident.
  // TODO: Parse core size.
  // TODO: Parse extension size.
  // TODO: Check CRC32
  // TODO: Parse options.
  return glcr::Status::Ok();
}

void WriteHeader(glcr::ByteBuffer& bytes, uint64_t offset, uint32_t core_size,
                 uint32_t extension_size) {
  bytes.WriteAt<uint32_t>(
      offset + 0, 0xDEADBEEF);  // TODO: Chose a more unique ident sequence.
  bytes.WriteAt<uint32_t>(offset + 4, core_size);
  bytes.WriteAt<uint32_t>(offset + 8, extension_size);
  bytes.WriteAt<uint32_t>(offset + 12, 0);  // TODO: Calculate CRC32.
}

}  // namespace yunq
