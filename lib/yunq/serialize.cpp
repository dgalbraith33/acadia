#include "serialize.h"

namespace yunq {
namespace {

const uint64_t kIdentByte = 0x33441122;

}  // namespace

void WriteHeader(glcr::ByteBuffer& bytes, uint64_t offset, uint32_t core_size,
                 uint32_t extension_size) {
  bytes.WriteAt<uint32_t>(offset + 0, kIdentByte);
  bytes.WriteAt<uint32_t>(offset + 4, core_size);
  bytes.WriteAt<uint32_t>(offset + 8, extension_size);
  bytes.WriteAt<uint32_t>(offset + 12, 0);  // TODO: Calculate CRC32.
}

}  // namespace yunq
