// Generated file -- DO NOT MODIFY.
#include "victoriafalls.yunq.h"

#include <yunq/serialize.h>


namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status OpenFileRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status OpenFileRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status OpenFileRequest::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes));
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));

  return glcr::Status::Ok();
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
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

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
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status OpenFileResponse::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse memory.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_memory(0);
  return glcr::Status::Ok();
}

glcr::Status OpenFileResponse::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  // Parse memory.
  uint64_t memory_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 2));

  set_memory(caps.At(memory_ptr));
  return glcr::Status::Ok();
}

glcr::Status OpenFileResponse::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes));
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));
  // Parse size.
  set_size(bytes.At<uint64_t>(offset + header_size + (8 * 1)));
  // Parse memory.
  // Skip Cap.

  return glcr::Status::Ok();
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
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

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
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status GetDirectoryRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status GetDirectoryRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status GetDirectoryRequest::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes));
  // Parse path.
  auto path_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_path(bytes.StringAt(offset + path_pointer.offset, path_pointer.length));

  return glcr::Status::Ok();
}

uint64_t GetDirectoryRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
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
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t GetDirectoryRequest::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
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
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
glcr::Status Directory::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status Directory::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(bytes, offset));
  return glcr::Status::Ok();
}

glcr::Status Directory::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  RETURN_ERROR(yunq::CheckHeader(bytes));
  // Parse filenames.
  auto filenames_pointer = bytes.At<ExtPointer>(offset + header_size + (8 * 0));

  set_filenames(bytes.StringAt(offset + filenames_pointer.offset, filenames_pointer.length));

  return glcr::Status::Ok();
}

uint64_t Directory::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  // Write filenames.
  ExtPointer filenames_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)filenames().length(),
  };

  bytes.WriteStringAt(offset + next_extension, filenames());
  next_extension += filenames_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), filenames_ptr);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t Directory::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write filenames.
  ExtPointer filenames_ptr{
    .offset = next_extension,
    // FIXME: Check downcast of str length.
    .length = (uint32_t)filenames().length(),
  };

  bytes.WriteStringAt(offset + next_extension, filenames());
  next_extension += filenames_ptr.length;

  bytes.WriteAt<ExtPointer>(offset + header_size + (8 * 0), filenames_ptr);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

