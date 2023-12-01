#include "serialize.h"

namespace yunq {
namespace {

const uint64_t kIdentByte = 0x33441122;

}  // namespace

glcr::Status CheckHeader(const glcr::ByteBuffer& buffer, uint64_t offset) {
  if (buffer.At<uint32_t>(offset + 0) != kIdentByte) {
    return glcr::InvalidArgument("Trying to parse an invalid yunq message.");
  }
  // TODO: Parse core size.
  // TODO: Parse extension size.
  // TODO: Check CRC32
  // TODO: Parse options.
  return glcr::Status::Ok();
}

void WriteHeader(glcr::ByteBuffer& bytes, uint64_t offset, uint32_t core_size,
                 uint32_t extension_size) {
  bytes.WriteAt<uint32_t>(offset + 0, kIdentByte);
  bytes.WriteAt<uint32_t>(offset + 4, core_size);
  bytes.WriteAt<uint32_t>(offset + 8, extension_size);
  bytes.WriteAt<uint32_t>(offset + 12, 0);  // TODO: Calculate CRC32.
}

}  // namespace yunq
