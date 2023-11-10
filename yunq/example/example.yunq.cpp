// Generated file -- DO NOT MODIFY.
#include "example.yunq.h"

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
  // Parse options.
  auto options_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 1))

  options_.Resize(options_pointer.length);
  for (uint64_t i = offset + options_pointer.offset;
       i < offset + options_pointer.offset + (sizeof(uint64_t) * options_pointer.length);
       i += sizeof(uint64_t)) {
    options_.PushBack(bytes.At<uint64_t>(i));
  }

}

void OpenFileRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));
  // Parse options.
  auto options_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 1))

  options_.Resize(options_pointer.length);
  for (uint64_t i = offset + options_pointer.offset;
       i < offset + options_pointer.offset + (sizeof(uint64_t) * options_pointer.length);
       i += sizeof(uint64_t)) {
    options_.PushBack(bytes.At<uint64_t>(i));
  }

}

uint64_t OpenFileRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 2;
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
  // Write options.
  ExtPointer options_ptr{
    .offset = next_extension,
    .length = (uint32_t)(options().size() * sizeof(uint64_t)),
  };

  next_extension += options_ptr.length;
  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 1), options_ptr);

  for (uint64_t i = 0; i < options().size(); i++) {
    uint32_t ext_offset = offset + options_ptr.offset + (i * sizeof(uint64_t));
    bytes.WriteAt<uint64_t>(ext_offset, options().at(i));
  }

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t OpenFileRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 2;
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
  // Write options.
  ExtPointer options_ptr{
    .offset = next_extension,
    .length = (uint32_t)(options().size() * sizeof(uint64_t)),
  };

  next_extension += options_ptr.length;
  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 1), options_ptr);

  for (uint64_t i = 0; i < options().size(); i++) {
    uint32_t ext_offset = offset + options_ptr.offset + (i * sizeof(uint64_t));
    bytes.WriteAt<uint64_t>(ext_offset, options().at(i));
  }

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
void File::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));
  // Parse attrs.
  set_attrs(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse mem_cap.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_mem_cap(0);
}

void File::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  CheckHeader(bytes);
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));
  // Parse attrs.
  set_attrs(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse mem_cap.
  uint64_t mem_cap_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 2));

  set_mem_cap(caps.At(mem_cap_ptr));
}

uint64_t File::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
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
  // Write attrs.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), attrs());
  // Write mem_cap.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), 0);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t File::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
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
  // Write attrs.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 1), attrs());
  // Write mem_cap.
  caps.WriteAt(next_cap, mem_cap());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 2), next_cap++);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}