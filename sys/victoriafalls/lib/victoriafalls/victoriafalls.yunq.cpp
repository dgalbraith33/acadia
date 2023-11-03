// Generated file -- DO NOT MODIFY.
#include "victoriafalls.yunq.h"

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
void OpenFileRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));
}

void OpenFileRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));
}

uint64_t OpenFileRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  // Write path.
  ExtPointer path_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)path().length(),
  };

  bytes.WriteStringAt(offset + next_extension, path());
  next_extension += path_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), path_ptr);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t OpenFileRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write path.
  ExtPointer path_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)path().length(),
  };

  bytes.WriteStringAt(offset + next_extension, path());
  next_extension += path_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), path_ptr);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
void OpenFileResponse::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));
  // Parse size.
  set_size(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse memory.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_memory(0);
}

void OpenFileResponse::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));
  // Parse size.
  set_size(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse memory.
  uint64_t memory_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 2));

  set_memory(caps.At(memory_ptr));
}

uint64_t OpenFileResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  // Write path.
  ExtPointer path_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)path().length(),
  };

  bytes.WriteStringAt(offset + next_extension, path());
  next_extension += path_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), path_ptr);
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), size());
  // Write memory.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), 0);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t OpenFileResponse::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 3;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write path.
  ExtPointer path_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)path().length(),
  };

  bytes.WriteStringAt(offset + next_extension, path());
  next_extension += path_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), path_ptr);
  // Write size.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), size());
  // Write memory.
  caps.WriteAt(next_cap, memory());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), next_cap++);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}