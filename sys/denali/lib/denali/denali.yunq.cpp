// Generated file -- DO NOT MODIFY.
#include "denali.yunq.h"

namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

void CheckHeader(const glcr::ByteBuffer& bytes) {
  // TODO: Check ident.
  // TODO: Parse core size.
  // TODO: Parse extension size.
  // TODO: Check CRC32
  // TODO: Parse options.
}

void WriteHeader(glcr::ByteBuffer& bytes, uint64_t offset, uint32_t core_size, uint32_t extension_size) {
  bytes.WriteAt<uint32_t>(offset + 0, 0xDEADBEEF);  // TODO: Chose a more unique ident sequence.
  bytes.WriteAt<uint32_t>(offset + 4, core_size);
  bytes.WriteAt<uint32_t>(offset + 8, extension_size);
  bytes.WriteAt<uint32_t>(offset + 12, 0); // TODO: Calculate CRC32.
  bytes.WriteAt<uint64_t>(offset + 16, 0); // TODO: Add options.
}

}  // namespace
void ReadRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  ParseFromBytesInternal(bytes, offset);
}

void ReadRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  ParseFromBytesInternal(bytes, offset);
}

void ReadRequest::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse device_id.
  set_device_id(bytes.At<uint64_t>(offset + header_size + (8 * 0)));
  // Parse lba.
  set_lba(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse size.
  set_size(bytes.At<uint64_t>(offset + header_size + (8 * 2)));

}

uint64_t ReadRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write lba.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), lba());
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), size());

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t ReadRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write lba.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), lba());
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), size());

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
void ReadManyRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  ParseFromBytesInternal(bytes, offset);
}

void ReadManyRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  ParseFromBytesInternal(bytes, offset);
}

void ReadManyRequest::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse device_id.
  set_device_id(bytes.At<uint64_t>(offset + header_size + (8 * 0)));
  // Parse lba.
  auto lba_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 1));

  lba_.Resize(lba_pointer.length / sizeof(uint64_t));
  for (uint64_t i = offset + lba_pointer.offset;
       i < offset + lba_pointer.offset + lba_pointer.length;
       i += sizeof(uint64_t)) {
    lba_.PushBack(bytes.At<uint64_t>(i));
  }


}

uint64_t ReadManyRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 2;
  const uint32_t core_size = next_extension;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write lba.
  ExtPointer lba_ptr{
    .offset = next_extension,
    .length = (uint32_t)(lba().size() * sizeof(uint64_t)),
  };

  next_extension += lba_ptr.length;
  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 1), lba_ptr);

  for (uint64_t i = 0; i < lba().size(); i++) {
    uint32_t ext_offset = offset + lba_ptr.offset + (i * sizeof(uint64_t));
    bytes.WriteAt<uint64_t>(ext_offset, lba().at(i));
  }

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t ReadManyRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 2;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write lba.
  ExtPointer lba_ptr{
    .offset = next_extension,
    .length = (uint32_t)(lba().size() * sizeof(uint64_t)),
  };

  next_extension += lba_ptr.length;
  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 1), lba_ptr);

  for (uint64_t i = 0; i < lba().size(); i++) {
    uint32_t ext_offset = offset + lba_ptr.offset + (i * sizeof(uint64_t));
    bytes.WriteAt<uint64_t>(ext_offset, lba().at(i));
  }

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
void ReadResponse::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  ParseFromBytesInternal(bytes, offset);
  // Parse memory.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_memory(0);
}

void ReadResponse::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  ParseFromBytesInternal(bytes, offset);
  // Parse memory.
  uint64_t memory_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 2));

  set_memory(caps.At(memory_ptr));
}

void ReadResponse::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse device_id.
  set_device_id(bytes.At<uint64_t>(offset + header_size + (8 * 0)));
  // Parse size.
  set_size(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse memory.
  // Skip Cap.

}

uint64_t ReadResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), size());
  // Write memory.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), 0);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t ReadResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write device_id.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), device_id());
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), size());
  // Write memory.
  caps.WriteAt(next_cap, memory());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), next_cap++);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}