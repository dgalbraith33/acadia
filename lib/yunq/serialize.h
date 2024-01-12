#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/status/status.h>

namespace yunq {

void WriteHeader(glcr::ByteBuffer& bytes, uint64_t offset, uint32_t core_size,
                 uint32_t extension_size);

}  // namespace yunq
