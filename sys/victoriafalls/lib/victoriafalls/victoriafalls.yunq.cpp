// Generated file -- DO NOT MODIFY.
#include "victoriafalls.yunq.h"

#include <yunq/message_view.h>
#include <yunq/serialize.h>


namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status OpenFileRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status OpenFileRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status OpenFileRequest::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse path.
  ASSIGN_OR_RETURN(path_, message.ReadField<glcr::String>(0));

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
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse memory.
  ASSIGN_OR_RETURN(memory_, message.ReadCapability(2));
  return glcr::Status::Ok();
}

glcr::Status OpenFileResponse::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse memory.
  ASSIGN_OR_RETURN(memory_, message.ReadCapability(2, caps));
  return glcr::Status::Ok();
}

glcr::Status OpenFileResponse::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse path.
  ASSIGN_OR_RETURN(path_, message.ReadField<glcr::String>(0));
  // Parse size.
  ASSIGN_OR_RETURN(size_, message.ReadField<uint64_t>(1));
  // Parse memory.

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
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status GetDirectoryRequest::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status GetDirectoryRequest::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse path.
  ASSIGN_OR_RETURN(path_, message.ReadField<glcr::String>(0));

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
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Directory::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Directory::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse filenames.
  ASSIGN_OR_RETURN(filenames_, message.ReadField<glcr::String>(0));

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

